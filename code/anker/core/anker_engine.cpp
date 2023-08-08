#include "anker_engine.hpp"

namespace Anker {

Engine::Engine(DataLoader& dataLoader)
    : dataLoader(dataLoader),
      renderDevice(window, dataLoader),
      imguiSystem(window, renderDevice),
      assetCache(dataLoader, renderDevice),
      renderer(renderDevice, assetCache)
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

	dataLoader.tick(dt);
	assetCache.reloadModifiedAssets();

	imguiSystem.newFrame();

	// ImGui::ShowDemoWindow();

	renderer.draw(*activeScene);
	imguiSystem.draw();
	renderDevice.present();
}

void Engine::onResize(Vec2i size)
{
	ANKER_INFO("onResize size={}", size);
	renderDevice.onResize(size);
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