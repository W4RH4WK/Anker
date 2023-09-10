#include <anker/editor/anker_editor_camera.hpp>

#include <anker/core/anker_engine.hpp>
#include <anker/core/anker_entity_name.hpp>
#include <anker/core/anker_transform.hpp>
#include <anker/graphics/anker_camera.hpp>
#include <anker/platform/anker_platform.hpp>

namespace Anker {

void EditorCameraSystem::tick(float, Scene& scene)
{
	auto activeCamera = scene.activeCamera();
	if (!activeCamera) {
		return;
	}

	auto [transform, camera, editorCamera] = activeCamera.try_get<Transform2D, Camera, EditorCamera>();
	if (!transform || !camera || !editorCamera) {
		return;
	}

	if (ImGui::GetIO().WantCaptureMouse) {
		return;
	}

	auto& actions = g_engine->inputSystem.actions();
	if (actions.editorCameraActivate) {
		Platform::hideCursor();
		transform->position += actions.editorCameraPan * editorCamera->speed * camera->distance;
		camera->distance -= actions.editorCameraZoom;
	}
}

void EditorCameraSystem::drawMenuBarEntry(Scene& scene)
{
	if (ImGui::BeginMenu("Cameras")) {
		for (auto [entityID, _, __] : scene.registry.view<Transform2D, Camera>().each()) {
			auto entity = scene.entityHandle(entityID);
			if (ImGui::MenuItem(entityLabel(entity).c_str())) {
				scene.setActiveCamera(entity);
			}
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Add Camera")) {
			auto camera = scene.createEntity("EditorCamera");
			camera.emplace<Transform2D>();
			camera.emplace<Camera>();
			camera.emplace<EditorCamera>();
			scene.setActiveCamera(camera);
		}

		ImGui::EndMenu();
	}
}

} // namespace Anker
