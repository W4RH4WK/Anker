#include <anker/platform/anker_platform.hpp>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_syswm.h>

#include <imgui_impl_sdl2.h>

#include <anker/core/anker_data_loader.hpp>
#include <anker/core/anker_data_loader_filesystem.hpp>

namespace Anker::Platform {

static std::optional<DataLoaderFilesystem> g_assetDataLoaderFs;

static SDL_Window* g_sdlWindow;
static HWND g_nativeWindow;

static bool g_shouldShutdown = false;
static bool g_showCursor = true;
static bool g_relativeCursorMode = false;

static Vec2 g_cursorPosition;
static Vec2 g_cursorDelta;
static Vec2 g_scrollDelta;

static std::array<int, SDL_NUM_SCANCODES> g_keyMapping;
static std::array<int, 6> g_mouseButtonMapping;
static std::array<bool, EnumEntries<MkbInput>.size()> g_mkbState;
static bool g_suppressMkbInput = false;

static SDL_GameController* g_gamepad;
static Vec2 g_gamepadLeftStick, g_gamepadRightStick;

static void sdlLog(void*, int category, SDL_LogPriority priority, const char* message)
{
	using namespace Anker::Log;

	std::string categoryText;
	switch (category) {
	case SDL_LOG_CATEGORY_APPLICATION: categoryText = "Application"; break;
	case SDL_LOG_CATEGORY_ERROR: categoryText = "Error"; break;
	case SDL_LOG_CATEGORY_ASSERT: categoryText = "Assert"; break;
	case SDL_LOG_CATEGORY_SYSTEM: categoryText = "System"; break;
	case SDL_LOG_CATEGORY_AUDIO: categoryText = "Audio"; break;
	case SDL_LOG_CATEGORY_VIDEO: categoryText = "Video"; break;
	case SDL_LOG_CATEGORY_RENDER: categoryText = "Render"; break;
	case SDL_LOG_CATEGORY_INPUT: categoryText = "Input"; break;
	case SDL_LOG_CATEGORY_TEST: categoryText = "Test"; break;
	default: categoryText = fmt::format("category({})", category);
	}

	Severity severity = Severity::Info;
	switch (priority) {
	case SDL_LOG_PRIORITY_VERBOSE: severity = Severity::Trace; break;
	case SDL_LOG_PRIORITY_DEBUG: severity = Severity::Trace; break;
	case SDL_LOG_PRIORITY_WARN: severity = Severity::Warning; break;
	case SDL_LOG_PRIORITY_ERROR:
	case SDL_LOG_PRIORITY_CRITICAL: severity = Severity::Error; break;
	default: break;
	}

	ANKER_LOG_MESSAGE(severity, "SDL {}: {}", categoryText, message);
}

static float normalizeAxis(i16 value)
{
	return value < 0 ? float(-value) / std::numeric_limits<i16>::min() //
	                 : float(value) / std::numeric_limits<i16>::max();
}

void initialize()
{
	{
		AllocConsole();
		std::FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);
	}

	SDL_LogSetOutputFunction(sdlLog, nullptr);
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		ANKER_FATAL("Failed to initialize SDL2: {}", SDL_GetError());
	}

	g_gamepad = SDL_GameControllerOpen(0);

	// Set up mouse / keyboard input lookup table.
	g_keyMapping.fill(int(MkbInput::Unkown));
#define ANKER_INPUTS_SDL_KEY(_input, _scancode) g_keyMapping[_scancode] = int(_input);
#define ANKER_INPUTS_SDL_MOUSE(_input, _button) g_mouseButtonMapping[_button] = int(_input);
#include "anker_inputs_sdl.inc"

	g_assetDataLoader.addSource(&g_assetDataLoaderFs.emplace("assets"));
}

void finalize()
{
	g_assetDataLoader.removeSource(&*g_assetDataLoaderFs);
	g_assetDataLoaderFs.reset();
	SDL_Quit();
}

void tick()
{
	g_scrollDelta = Vec2(0);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch (event.type) {
		case SDL_KEYDOWN: g_mkbState[g_keyMapping[event.key.keysym.scancode]] = true; break;
		case SDL_KEYUP: g_mkbState[g_keyMapping[event.key.keysym.scancode]] = false; break;
		case SDL_MOUSEBUTTONDOWN: g_mkbState[g_mouseButtonMapping[event.button.button]] = true; break;
		case SDL_MOUSEBUTTONUP: g_mkbState[g_mouseButtonMapping[event.button.button]] = false; break;
		case SDL_MOUSEWHEEL:
			g_scrollDelta.x += float(event.wheel.x);
			g_scrollDelta.y += float(event.wheel.y);
			break;
		case SDL_QUIT: g_shouldShutdown = true; break;
		}
	}

	g_suppressMkbInput = ImGui::GetIO().WantCaptureKeyboard;

	{
		Vec2i pos;
		SDL_GetMouseState(&pos.x, &pos.y);
		g_cursorPosition = Vec2(pos);

		SDL_GetRelativeMouseState(&pos.x, &pos.y);
		g_cursorDelta = Vec2(pos);
	}

	if (g_gamepad) {
		g_gamepadLeftStick.x = normalizeAxis(SDL_GameControllerGetAxis(g_gamepad, SDL_CONTROLLER_AXIS_LEFTX));
		g_gamepadLeftStick.y = normalizeAxis(SDL_GameControllerGetAxis(g_gamepad, SDL_CONTROLLER_AXIS_LEFTY));
		g_gamepadLeftStick = radialDeadzone(g_gamepadLeftStick, 0.12f);

		g_gamepadRightStick.x = normalizeAxis(SDL_GameControllerGetAxis(g_gamepad, SDL_CONTROLLER_AXIS_RIGHTX));
		g_gamepadRightStick.y = normalizeAxis(SDL_GameControllerGetAxis(g_gamepad, SDL_CONTROLLER_AXIS_RIGHTY));
		g_gamepadRightStick = radialDeadzone(g_gamepadRightStick, 0.12f);
	}

	// Any component that wants to hide the cursor calls hideCursor. If none
	// does, we display the cursor as normal.
	SDL_ShowCursor(g_showCursor ? SDL_ENABLE : SDL_DISABLE);
	g_showCursor = true;

	// Any component that wants relative cursor mode calls relativeCursorMode.
	// If none does, we disable relative cursor mode.
	SDL_SetRelativeMouseMode(SDL_bool(g_relativeCursorMode));
	g_relativeCursorMode = false;
}

bool shouldShutdown()
{
	return g_shouldShutdown;
}

void createMainWindow()
{
	g_sdlWindow = SDL_CreateWindow("Anker", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, 0);
	if (!g_sdlWindow) {
		ANKER_FATAL("Could not create main window: {}", SDL_GetError());
	}

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if (!SDL_GetWindowWMInfo(g_sdlWindow, &info)) {
		ANKER_FATAL("Could not get native window handle from SDL window");
	}
	g_nativeWindow = info.info.win.window;
}

void destroyMainWindow()
{
	SDL_DestroyWindow(g_sdlWindow);
	g_sdlWindow = nullptr;
}

Vec2i windowSize()
{
	ANKER_CHECK(g_sdlWindow, {});

	Vec2i size;
	SDL_GetWindowSize(g_sdlWindow, &size.x, &size.y);
	return size;
}

bool windowHasFocus()
{
	ANKER_CHECK(g_sdlWindow, false);
	return SDL_GetWindowFlags(g_sdlWindow) & SDL_WINDOW_INPUT_FOCUS;
}

float inputValue(MkbInput input)
{
	if (g_suppressMkbInput || int(input) >= g_mkbState.size()) {
		return 0;
	}
	return g_mkbState[int(input)];
}

float inputValue(GamepadInput input)
{
	if (!g_gamepad) {
		return 0.0f;
	}

	auto pos = [](float v) { return v > 0 ? v : 0; };
	auto neg = [](float v) { return v < 0 ? -v : 0; };

	switch (input) {
	case GamepadInput::A: return SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_A);
	case GamepadInput::B: return SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_B);
	case GamepadInput::X: return SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_X);
	case GamepadInput::Y: return SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_Y);
	case GamepadInput::PadUp: return SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_DPAD_UP);
	case GamepadInput::PadDown: return SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
	case GamepadInput::PadLeft: return SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	case GamepadInput::PadRight: return SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	case GamepadInput::L1: return SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
	case GamepadInput::R1: return SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
	case GamepadInput::L2: {
		i16 value = SDL_GameControllerGetAxis(g_gamepad, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
		return linearDeadzone(normalizeAxis(value), 0.12f);
	}
	case GamepadInput::R2: {
		i16 value = SDL_GameControllerGetAxis(g_gamepad, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
		return linearDeadzone(normalizeAxis(value), 0.12f);
	}
	case GamepadInput::L3: return SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_LEFTSTICK);
	case GamepadInput::R3: return SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_RIGHTSTICK);
	case GamepadInput::Menu: return SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_START);
	case GamepadInput::View: return SDL_GameControllerGetButton(g_gamepad, SDL_CONTROLLER_BUTTON_BACK);

	case GamepadInput::LsUp: return neg(g_gamepadLeftStick.y);
	case GamepadInput::LsDown: return pos(g_gamepadLeftStick.y);
	case GamepadInput::LsLeft: return neg(g_gamepadLeftStick.x);
	case GamepadInput::LsRight: return pos(g_gamepadLeftStick.x);

	case GamepadInput::RsUp: return neg(g_gamepadRightStick.y);
	case GamepadInput::RsDown: return pos(g_gamepadRightStick.y);
	case GamepadInput::RsLeft: return neg(g_gamepadRightStick.x);
	case GamepadInput::RsRight: return pos(g_gamepadRightStick.x);

	case GamepadInput::R4:
	case GamepadInput::L4:
	case GamepadInput::R5:
	case GamepadInput::L5: return 0.0f;
	}
	return 0.0f;
}

Vec2 cursorPosition()
{
	return g_cursorPosition;
}

Vec2 cursorDelta()
{
	return g_cursorDelta;
}

Vec2 scrollDelta()
{
	return g_scrollDelta;
}

void hideCursor()
{
	g_showCursor = false;
}

void enableRelativeCursorMode()
{
	g_relativeCursorMode = true;
}

void imguiImplInit()
{
	ImGui_ImplSDL2_InitForOther(g_sdlWindow);
}

void imguiImplNewFrame()
{
	ImGui_ImplSDL2_NewFrame();
}

void imguiImplShutdown()
{
	ImGui_ImplSDL2_Shutdown();
}

NativeWindow nativeWindow()
{
	return g_nativeWindow;
}

} // namespace Anker::Platform
