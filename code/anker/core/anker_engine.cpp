#include <anker/core/anker_engine.hpp>

#include <anker/core/anker_scene_node.hpp>
#include <anker/graphics/anker_camera.hpp>

namespace Anker {

Engine::Engine()
    : renderDevice(),
      fontSystem(renderDevice),
      imguiSystem(renderDevice),
      inputSystem(imguiSystem),
      assetCache(renderDevice, fontSystem),
      renderer(renderDevice, assetCache),
      playerControllerSystem(inputSystem),
      physicsSystem(renderer.gizmoRenderer)
{
	ANKER_INFO("Anker Initialized!");
}

void Engine::tick()
{
	ANKER_PROFILE_FRAME_MARK();

	if (!activeScene) {
		ANKER_ERROR("No active scene");
		return;
	}

	float dt = calculateDeltaTime();

	inputSystem.tick(dt);

	assetCache.reloadModifiedAssets();

	imguiSystem.newFrame();

	if (editor) {
		editor->tick(dt, *activeScene);
	}

	// ImGui::ShowDemoWindow();

	playerControllerSystem.tick(dt, *activeScene);
	followerSystem.tick(dt, *activeScene);

	physicsSystem.tick(dt, *activeScene);

	renderer.draw(*activeScene);
	imguiSystem.draw();
	renderDevice.present();
}

ScenePtr Engine::createScene()
{
	auto scene = std::make_unique<Scene>();

	registerSceneNodeCallbacks(scene->registry);

	physicsSystem.addPhysicsWorld(*scene);

	auto camera = scene->createEntity("Camera");
	camera.emplace<SceneNode>();
	camera.emplace<Camera>();
	scene->setActiveCamera(camera);

	return scene;
}

void Engine::onResize(Vec2i size)
{
	ANKER_INFO("onResize size={}", size);
	renderDevice.onResize(size);
	renderer.onResize(size);
}

float Engine::calculateDeltaTime()
{
	static constexpr float FrametimeMin = 1.0f / 600.0f;
	static constexpr float FrametimeMax = 1.0f / 30.0f;

	auto now = Clock::now();
	float dt = std::chrono::duration<float>(now - m_frameTimestamp).count();
	m_frameTimestamp = now;
	return std::clamp(dt, FrametimeMin, FrametimeMax);
}

} // namespace Anker
