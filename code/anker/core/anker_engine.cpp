#include <anker/core/anker_engine.hpp>

#include <anker/core/anker_transform.hpp>
#include <anker/graphics/anker_camera.hpp>

namespace Anker {

Engine::Engine(DataLoader& dataLoader)
    : dataLoader(dataLoader),
      renderDevice(dataLoader),
      imguiSystem(renderDevice),
      inputSystem(imguiSystem),
      assetCache(dataLoader, renderDevice),
      renderer(renderDevice, assetCache),
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

	dataLoader.tick(dt);
	assetCache.reloadModifiedAssets();

	imguiSystem.newFrame();

	if (editorSystem) {
		editorSystem->tick(dt, *activeScene);
	}

	// ImGui::ShowDemoWindow();

	physicsSystem.tick(dt, *activeScene);

	renderer.draw(*activeScene);
	imguiSystem.draw();
	renderDevice.present();
}

ScenePtr Engine::createScene()
{
	auto scene = std::make_unique<Scene>();

	physicsSystem.addPhysicsWorld(*scene);

	scene->activeCamera = scene->createEntity("Camera");
	scene->activeCamera.emplace<Transform2D>();
	scene->activeCamera.emplace<Camera>();

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
