#pragma once

#include <anker/core/anker_asset_cache.hpp>
#include <anker/core/anker_data_loader.hpp>
#include <anker/core/anker_imgui_system.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/editor/anker_editor_system.hpp>
#include <anker/graphics/anker_render_device.hpp>
#include <anker/graphics/anker_renderer.hpp>
#include <anker/platform/anker_input_system_win32.hpp>
#include <anker/platform/anker_window_win32.hpp>

namespace Anker {

// The main purpose of the Engine class is to coordinate construction (and
// destruction) of the various engine parts. Furthermore, parts can be accessed
// via the engine instance, if necessary. Note that dependencies between parts
// is primarily (but not exclusively) established via constructor parameters.

class Engine {
  public:
	explicit Engine(DataLoader&);

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;
	Engine(Engine&&) noexcept = delete;
	Engine& operator=(Engine&&) noexcept = delete;

	// Tick is called every frame and represents the main-loop of the engine.
	// Many parts feature a tick function that is called by Engine's tick
	// function. A float parameter (commonly named dt) provides the delta-time
	// of the previous frame.
	void tick();

	void onResize(Vec2i size);

	DataLoader& dataLoader;

	Window window;
	RenderDevice renderDevice;
	ImguiSystem imguiSystem;
	InputSystem inputSystem;

	AssetCache assetCache;
	Renderer renderer;

	ScenePtr activeScene;

	std::optional<EditorSystem> editorSystem;

	using Clock = std::chrono::steady_clock;

  private:
	float calculateDeltaTime();

	Clock::time_point m_frameTimestamp = Clock::now();
};

inline std::optional<Engine> g_engine;

} // namespace Anker
