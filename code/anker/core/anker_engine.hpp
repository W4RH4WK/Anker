#pragma once

#include <anker/audio/anker_audio_system.hpp>
#include <anker/core/anker_asset_cache.hpp>
#include <anker/core/anker_data_loader.hpp>
#include <anker/core/anker_imgui_system.hpp>
#include <anker/core/anker_input_system.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/editor/anker_editor_framework.hpp>
#include <anker/graphics/anker_font_system.hpp>
#include <anker/graphics/anker_render_device.hpp>
#include <anker/graphics/anker_render_system.hpp>
#include <anker/physics/anker_physics_system.hpp>

namespace Anker {

// The main purpose of the Engine class is to coordinate construction (and
// destruction) of the various engine parts. Furthermore, parts can be accessed
// via the engine instance, if necessary. Note that dependencies between parts
// is primarily (but not exclusively) established via constructor parameters.

class Engine {
  public:
	explicit Engine();

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;
	Engine(Engine&&) noexcept = delete;
	Engine& operator=(Engine&&) noexcept = delete;

	// Tick is called every frame and represents the main-loop of the engine.
	// Many parts feature a tick function that is called by Engine's tick
	// function. A float parameter (commonly named dt) provides the delta-time
	// of the previous frame.
	void tick();

	ScenePtr createScene();

	void onResize(Vec2i size);

	RenderDevice renderDevice;
	FontSystem fontSystem;
	ImguiSystem imguiSystem;
	InputSystem inputSystem;
	AudioSystem audioSystem;

	AssetCache assetCache;
	RenderSystem renderSystem;

	PhysicsSystem physicsSystem;

	ScenePtr activeScene;
	ScenePtr nextScene;

	std::optional<EditorFramework> editor;

	using Clock = std::chrono::steady_clock;

  private:
	float calculateDeltaTime();

	void switchScene();

	Clock::time_point m_frameTimestamp = Clock::now();
};

inline std::optional<Engine> g_engine;

} // namespace Anker
