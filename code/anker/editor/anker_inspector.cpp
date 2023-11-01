#include <anker/editor/anker_inspector.hpp>

#include <anker/core/anker_components.hpp>
#include <anker/core/anker_engine.hpp>
#include <anker/core/anker_entity_name.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_scene_node.hpp>
#include <anker/graphics/anker_camera.hpp>
#include <anker/graphics/anker_gizmo_renderer.hpp>
#include <anker/graphics/anker_sprite.hpp>

namespace Anker {

using PinnedWindowTag = entt::tag<"Inspector::PinnedWindowTag"_hs>;

void Inspector::tick(float, Scene& scene)
{
	if (!m_enabled) {
		return;
	}

	ImGui::Begin("Inspector", &m_enabled);
	{
		if (ImGui::Button("New Entity")) {
			auto entity = scene.createEntity();
			selectEntity(entity);
		}

		ImGui::Separator();

		std::vector<SceneNode*> rootNodes;
		scene.registry.each([&](EntityID entityID) {
			EntityHandle entity{scene.registry, entityID};

			// Collect SceneNodes (by roots) so we can draw them later.
			if (auto* node = entity.try_get<SceneNode>()) {
				if (!node->hasParent()) {
					rootNodes.push_back(node);
				}
				return;
			}

			if (ImGui::Selectable(entityImGuiLabel(entity).c_str(), selectedEntity(scene) == entity)) {
				selectEntity(entity);
			}
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Copy Entity ID")) {
					ImGui::SetClipboardText(std::to_string(entt::to_integral(entity.entity())).c_str());
				}
				if (ImGui::MenuItem("Delete")) {
					entity.destroy();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Pin Inspector Window")) {
					entity.emplace_or_replace<PinnedWindowTag>();
				}
				ImGui::EndPopup();
			}
		});

		ImGui::Separator();

		for (auto* node : rootNodes) {
			drawSceneNodeRecursive(scene, node);
		}
		if (m_sceneGraphModification) {
			m_sceneGraphModification();
			m_sceneGraphModification = nullptr;
		}
	}
	ImGui::End();

	if (auto entity = selectedEntity(scene)) {
		drawComponentEditorWindow(entity, "Selected Entity");
		drawSelectionGizmo(entity);
	}

	for (auto [entity] : scene.registry.view<PinnedWindowTag>().each()) {
		drawComponentEditorWindow(scene.entityHandle(entity));
	}
}

void Inspector::drawMenuBarEntry()
{
	ImGui::ToggleButton("Inspector", &m_enabled);
}

constexpr auto SelectedEntityTag = "Inspector::SelectedEntity"_hs;

EntityHandle Inspector::selectedEntity(Scene& scene)
{
	if (auto* entity = scene.registry.ctx().find<EntityID>(SelectedEntityTag)) {
		return scene.entityHandle(*entity);
	} else {
		return scene.entityHandle(entt::null);
	}
}

void Inspector::selectEntity(EntityHandle entity)
{
	entity.registry()->ctx().insert_or_assign(SelectedEntityTag, entity.entity());
}

void Inspector::drawSceneNodeRecursive(Scene& scene, const SceneNode* node)
{
	ANKER_CHECK(node);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	if (selectedEntity(scene) == node->entity()) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}
	if (node->children().empty()) {
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	bool opened = ImGui::TreeNodeEx(entityImGuiLabel(node->entity()).c_str(), flags);

	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
		selectEntity(node->entity());
	}

	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Copy Entity ID")) {
			ImGui::SetClipboardText(std::to_string(entt::to_integral(node->entity().entity())).c_str());
		}
		if (ImGui::MenuItem("Clear Parent")) {
			m_sceneGraphModification = [node] { const_cast<SceneNode*>(node)->clearParent(); };
		}
		if (ImGui::MenuItem("Delete")) {
			m_sceneGraphModification = [node] { node->entity().destroy(); };
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Pin Inspector Window")) {
			node->entity().emplace_or_replace<PinnedWindowTag>();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("Anker_SceneNode", &node, sizeof(node));
		ImGui::Text("%s", entityDisplayName(node->entity()).c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Anker_SceneNode")) {
			auto* source = *static_cast<const SceneNode**>(payload->Data);
			m_sceneGraphModification = [source, node] {
				const_cast<SceneNode*>(source)->setParent(const_cast<SceneNode*>(node));
			};
		}
		ImGui::EndDragDropTarget();
	}

	if (opened) {
		// The scene graph is organized from back to front (order of rendering).
		// However, the tree visualization has foreground at the top and
		// background at the bottom.
		for (auto* child : node->children() | std::views::reverse) {
			drawSceneNodeRecursive(scene, child);
		}
		ImGui::TreePop();
	}
}

void Inspector::drawComponentEditorWindow(EntityHandle entity, std::string_view windowName_)
{
	std::string windowName(windowName_);
	if (windowName.empty()) {
		windowName = entityImGuiLabel(entity);
	}

	bool windowIsOpen = true;
	ImGui::Begin(windowName.c_str(), &windowIsOpen);
	if (!windowIsOpen) {
		entity.remove<PinnedWindowTag>();
	}

	// Name widget
	std::string name = entityDisplayName(entity);
	if (ImGui::InputText("name", &name, ImGuiInputTextFlags_EnterReturnsTrue)) {
		if (name.empty()) {
			entity.remove<EntityName>();
		} else {
			entity.get_or_emplace<EntityName>().name = name;
		}
	}

	// Add Component button
	ImGui::SameLine(ImGui::GetWindowWidth() - 30);
	if (ImGui::Button("+##addComp")) {
		ImGui::OpenPopup("addCompMenu");
	}
	if (ImGui::BeginPopup("addCompMenu")) {
		for (auto& component : components()) {
			if (component.flags & ComponentFlag::HideInInspector || component.isPresentIn(entity) || !component.addTo) {
				continue;
			}
			if (ImGui::Selectable(component.name)) {
				component.addTo(entity);
			}
		}
		ImGui::EndPopup();
	}

	ImGui::Separator();

	InspectorWidgetDrawer drawer;
	for (auto& component : components()) {
		if (component.flags & ComponentFlag::HideInInspector || !component.isPresentIn(entity)) {
			continue;
		}

		bool opened = ImGui::TreeNodeEx(component.name, ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen);

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Delete")) {
				component.removeFrom(entity);
			}
			ImGui::EndPopup();
		}

		if (opened) {
			if (component.drawInspectorWidget) {
				component.drawInspectorWidget(drawer, entity);
			} else {
				ImGui::TextDisabled("No Inspector widget defined");
			}
			ImGui::TreePop();
		}
	}

	ImGui::End();
}

void Inspector::drawSelectionGizmo(EntityCHandle entity)
{
	if (auto* node = entity.try_get<SceneNode>()) {
		auto transform = node->globalTransform();

		Rect2 rect;
		if (auto* sprite = entity.try_get<Sprite>(); sprite && sprite->texture) {
			rect.size = Vec2(sprite->texture->info.size) * sprite->textureRect.size / sprite->pixelToMeter;
			rect.size *= transform.scale;
		} else {
			rect.size = {0.25f, 0.25f};
		}
		rect.offset = transform.position - rect.size / 2.0f;

		g_engine->renderSystem.gizmoRenderer.addRect(rect, {0.95f, 0.60f, 0.22f, 1});
	}
}

} // namespace Anker
