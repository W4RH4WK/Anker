#include <anker/editor/anker_inspector.hpp>

#include <anker/core/anker_components.hpp>
#include <anker/core/anker_engine.hpp>
#include <anker/core/anker_entity_name.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_transform.hpp>
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

		scene.registry.each([&](EntityID entityID) {
			EntityHandle entity{scene.registry, entityID};

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
}

void Inspector::drawMenuBarEntry()
{
	ImGui::ToggleButton("Entities", &m_enabled);
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
	if (auto* transform = entity.try_get<Transform2D>()) {
		g_engine->renderer.gizmoRenderer.addRect(
		    Rect2(transform->scale, transform->position - transform->scale / 2.0f), //
		    {0.95f, 0.60f, 0.22f, 1});
	}
}

} // namespace Anker
