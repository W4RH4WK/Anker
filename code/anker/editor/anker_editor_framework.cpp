#include <anker/editor/anker_editor_framework.hpp>

#include <anker/core/anker_engine.hpp>
#include <anker/game/anker_map.hpp>

namespace Anker {

void EditorFramework::tick(float dt, Scene& scene)
{
	const auto& actions = g_engine->inputSystem.actions();

	// Reload the current map on button press.
	if (actions.editorMapReload) {
		if (std::string_view mapIdentifier = currentMapIdentifier(); !mapIdentifier.empty()) {
			g_engine->nextScene = loadMap(mapIdentifier);
		}
	}

	// Editor UI can be toggled on/off with a single button press.
	if (actions.editorToggle) {
		m_enabled = !m_enabled;
	}
	if (!m_enabled) {
		return;
	}

	m_inspector.tick(dt, scene);
	m_cameraSystem.tick(dt, scene);

	if (ImGui::BeginMainMenuBar()) {
		m_inspector.drawMenuBarEntry();
		m_cameraSystem.drawMenuBarEntry(scene);

		drawMapsMenuBarEntry();

		ImGui::ToggleButton("PhysDbg", &g_engine->physicsSystem.debugDraw);

		ImGui::EndMainMenuBar();
	}
}

void EditorFramework::drawMapsMenuBarEntry()
{
	// List available maps and load them on click. Since we currently cannot
	// query the AssetDataLoader, we query the file system. This should be fine
	// as this is just an editor feature.

	if (ImGui::BeginMenu("Map")) {
		for (auto& entry : fs::directory_iterator("assets/maps")) {
			if (entry.path().extension() == ".tmj") {
				auto mapIdentifier = toIdentifier(fs::relative(entry.path(), "assets"));

				bool isCurrent = mapIdentifier == currentMapIdentifier();

				if (ImGui::MenuItem(entry.path().filename().string().c_str(), nullptr, &isCurrent)) {
					g_engine->nextScene = loadMap(mapIdentifier);
				}
			}
		}
		ImGui::EndMenu();
	}
}

std::string_view EditorFramework::currentMapIdentifier() const
{
	if (g_engine->activeScene) {
		if (auto* mapIdentifier = g_engine->activeScene->registry.ctx().find<MapIdentifier>()) {
			return mapIdentifier->identifier;
		}
	}
	return {};
}

} // namespace Anker
