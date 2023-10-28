#include <anker/editor/anker_editor_camera.hpp>

#include <anker/core/anker_engine.hpp>
#include <anker/core/anker_entity_name.hpp>
#include <anker/core/anker_scene_node.hpp>
#include <anker/graphics/anker_camera.hpp>
#include <anker/platform/anker_platform.hpp>

namespace Anker {

void EditorCameraSystem::tick(float, Scene& scene)
{
	auto activeCamera = scene.activeCamera();
	if (!activeCamera) {
		return;
	}

	auto [node, camera, editorCamera] = activeCamera.try_get<SceneNode, Camera, EditorCamera>();
	if (!node || !camera || !editorCamera) {
		return;
	}

	// g_engine->renderer.gizmoRenderer.addGrid(32);

	if (ImGui::GetIO().WantCaptureMouse) {
		return;
	}

	auto& actions = g_engine->inputSystem.actions();
	if (actions.editorCameraActivate.down()) {
		Platform::hideCursor();
		Platform::enableRelativeCursorMode();

		auto transform = node->globalTransform();
		transform.position += actions.editorCameraPan * editorCamera->speed * camera->distance;
		node->setGlobalTransform(transform);

		camera->distance -= actions.editorCameraZoom;
	}
}

void EditorCameraSystem::drawMenuBarEntry(Scene& scene)
{
	if (ImGui::BeginMenu("Cameras")) {
		for (auto [entityID, _, __] : scene.registry.view<SceneNode, Camera>().each()) {
			auto entity = scene.entityHandle(entityID);
			if (ImGui::MenuItem(entityImGuiLabel(entity).c_str())) {
				scene.setActiveCamera(entity);
			}
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Add Camera")) {
			auto camera = scene.createEntity("EditorCamera");
			camera.emplace<SceneNode>();
			camera.emplace<Camera>();
			camera.emplace<EditorCamera>();
			scene.setActiveCamera(camera);
		}

		ImGui::EndMenu();
	}
}

} // namespace Anker
