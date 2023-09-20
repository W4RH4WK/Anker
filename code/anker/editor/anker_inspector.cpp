#include <anker/editor/anker_inspector.hpp>

#include <anker/core/anker_components.hpp>
#include <anker/core/anker_engine.hpp>
#include <anker/core/anker_entity_name.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_scene_node.hpp>
#include <anker/graphics/anker_camera.hpp>
#include <anker/graphics/anker_gizmo_renderer.hpp>

namespace Anker {

void Inspector::tick(float, Scene& scene)
{
	if (!m_enabled) {
		return;
	}

	ImGui::Begin("Inspector", &m_enabled);
	auto windowSize = ImGui::GetContentRegionAvail();

	ImGui::BeginChild("Entities", {windowSize.x * 0.3f, windowSize.y});
	{
		if (ImGui::Button("New Entity")) {
			m_selectedEntity = scene.createEntity();
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

			if (ImGui::Selectable(entityLabel(entity).c_str(), m_selectedEntity && m_selectedEntity == entity)) {
				m_selectedEntity = entity;
			}
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Delete")) {
					entity.destroy();
				}
				ImGui::EndPopup();
			}
		});

		ImGui::Separator();

		for (auto* node : rootNodes) {
			drawSceneNodeRecursive(node);
		}
	}
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("Entity", {windowSize.x * 0.7f, windowSize.y});
	{
		if (m_selectedEntity) {
			if (auto entity = scene.entityHandle(*m_selectedEntity)) {
				drawNameWidget(entity);
				drawAddComponentButton(entity);
				ImGui::Separator();
				drawComponentEditor(entity);
			}
		}
	}
	ImGui::EndChild();

	ImGui::End();

	if (m_selectedEntity) {
		if (auto entity = scene.entityHandle(*m_selectedEntity)) {
			drawSelectionGizmo(entity);
		}
	}

	for (auto [source, node] : m_reparentList) {
		source->setParent(node);
	}
	m_reparentList.clear();
}

void Inspector::drawMenuBarEntry()
{
	ImGui::ToggleButton("Entities", &m_enabled);
}

void Inspector::drawSceneNodeRecursive(SceneNode* node)
{
	ANKER_ASSERT(node);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	if (m_selectedEntity && *m_selectedEntity == node->entity()) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}
	if (node->children().empty()) {
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	bool opened = ImGui::TreeNodeEx(entityLabel(node->entity()).c_str(), flags);

	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
		m_selectedEntity = node->entity();
	}

	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Clear Parent")) {
			node->clearParent();
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Delete")) {
			node->entity().destroy();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("ANKER_SCENENODE", &node, sizeof(node));
		ImGui::Text("%s", entityDisplayName(node->entity()).c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ANKER_SCENENODE")) {
			SceneNode* source = *static_cast<SceneNode**>(payload->Data);
			m_reparentList.push_back({source, node});
		}
		ImGui::EndDragDropTarget();
	}

	if (opened) {
		for (auto* child : node->children()) {
			drawSceneNodeRecursive(child);
		}
		ImGui::TreePop();
	}
}

void Inspector::drawNameWidget(EntityHandle entity)
{
	std::string name = entityDisplayName(entity);
	if (ImGui::InputText("name", &name, ImGuiInputTextFlags_EnterReturnsTrue)) {
		if (name.empty()) {
			entity.remove<EntityName>();
		} else {
			entity.get_or_emplace<EntityName>().name = name;
		}
	}
}

void Inspector::drawAddComponentButton(EntityHandle entity)
{
	ImGui::SameLine(ImGui::GetWindowWidth() - 20);
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
}

void Inspector::drawComponentEditor(EntityHandle entity)
{
	const auto treeFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen;

	EditWidgetDrawer drawer;
	for (auto& component : components()) {
		if (component.flags & ComponentFlag::HideInInspector || !component.isPresentIn(entity)) {
			continue;
		}

		bool opened = ImGui::TreeNodeEx(component.name, treeFlags);

		// context menu
		{
			ImGui::PushID(component.name);
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Delete")) {
					component.removeFrom(entity);
				}
				ImGui::EndPopup();
			}
			ImGui::PopID();
		}

		if (opened) {
			if (component.drawEditWidget) {
				component.drawEditWidget(drawer, entity);
			} else {
				ImGui::TextDisabled("No editWidget defined");
			}
			ImGui::TreePop();
		}
	}
}

void Inspector::drawSelectionGizmo(EntityCHandle entity)
{
	if (auto* node = entity.try_get<SceneNode>()) {
		auto transform = node->globalTransform();

		Rect2 rect;
		rect.size = transform.scale;
		rect.offset = transform.position - transform.scale / 2.0f;

		g_engine->renderer.gizmoRenderer.addRect(rect, {0.95f, 0.60f, 0.22f, 1});
	}
}

} // namespace Anker
