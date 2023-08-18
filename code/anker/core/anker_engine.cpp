#include <anker/core/anker_engine.hpp>

#include <anker/core/anker_entity_name.hpp>
#include <anker/core/anker_transform.hpp>
#include <anker/graphics/anker_camera.hpp>
#include <anker/graphics/anker_sprite.hpp>

namespace Anker {

Engine::Engine(DataLoader& dataLoader)
    : dataLoader(dataLoader),
      renderDevice(dataLoader),
      imguiSystem(renderDevice),
      inputSystem(imguiSystem),
      assetCache(dataLoader, renderDevice),
      renderer(renderDevice, assetCache)
{
	// Registering Transform component here since there is no system directly
	// associated with it.
	componentRegistry.registerComponent<EntityName>("Name", -2);
	componentRegistry.registerComponent<Transform2D>("Transform2D", -1);
	componentRegistry.registerComponent<Camera>("Camera");
	componentRegistry.registerComponent<Sprite>("Sprite");

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

	renderer.draw(*activeScene);
	imguiSystem.draw();
	renderDevice.present();
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
