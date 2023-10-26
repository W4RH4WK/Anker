#include <anker/platform/anker_platform_glfw.hpp>

#include <imgui_impl_glfw.h>

#include <anker/core/anker_data_loader.hpp>
#include <anker/core/anker_data_loader_filesystem.hpp>
#include <anker/core/anker_engine.hpp>
#include <anker/core/anker_inputs.hpp>

namespace Anker::Platform {

constexpr float Deadzone = 0.12f; // TODO: setting

static GLFWwindow* g_glfwWindow = nullptr;
static NativeWindow g_nativeWindow = nullptr;

static std::optional<DataLoaderFilesystem> g_assetDataLoaderFs;

static bool g_windowHasFocus = true;
static bool g_hideCursor = false;

static std::array<int, MkbInputEntries.size()> g_mkbMapping;

static Vec2 g_cursorPosition;
static Vec2 g_cursorDelta;
static Vec2 g_scrollDelta;

struct GamepadState {
	GLFWgamepadstate pad;
	Vec2 leftStick;
	Vec2 rightStick;
};
static GamepadState g_gamepadState;

void initialize()
{
	{
		AllocConsole();
		std::FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);
	}

	glfwSetErrorCallback([](int error_code, const char* description) { //
		ANKER_ERROR("GLFW: {} {}", error_code, description);
	});

	if (!glfwInit()) {
		ANKER_FATAL("Failed to initialize GLFW");
	}

	g_assetDataLoader.addSource(&g_assetDataLoaderFs.emplace("assets"));

	// Set up mouse / keyboard input lookup table.
	g_mkbMapping.fill(GLFW_KEY_UNKNOWN);
#define ANKER_INPUTS_GLFW_MKB(_input, _glfwInput) g_mkbMapping[int(_input)] = _glfwInput;
#include "anker_inputs_glfw.inc"
}

void finalize()
{
	g_assetDataLoader.removeSource(&*g_assetDataLoaderFs);
	g_assetDataLoaderFs.reset();
	glfwTerminate();
}

static void tickInput()
{
	// Update cursor state
	g_cursorDelta = g_cursorPosition; // previous
	{
		double x = 0, y = 0;
		glfwGetCursorPos(g_glfwWindow, &x, &y);
		g_cursorPosition = {float(x), float(y)};
	}
	g_cursorDelta = g_cursorPosition - g_cursorDelta;

	// Update gamepad state
	if (glfwGetGamepadState(GLFW_JOYSTICK_1, &g_gamepadState.pad)) {
		g_gamepadState.leftStick = {g_gamepadState.pad.axes[GLFW_GAMEPAD_AXIS_LEFT_X],
		                            -g_gamepadState.pad.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]};
		g_gamepadState.leftStick = radialDeadzone(g_gamepadState.leftStick, Deadzone);

		g_gamepadState.rightStick = {g_gamepadState.pad.axes[GLFW_GAMEPAD_AXIS_RIGHT_X],
		                             -g_gamepadState.pad.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]};
		g_gamepadState.rightStick = radialDeadzone(g_gamepadState.rightStick, Deadzone);
	} else {
		g_gamepadState = {};
	}

	// Any component that wants to hide the cursor calls hideCursor. If none
	// does, we display the cursor as normal.
	glfwSetInputMode(g_glfwWindow, GLFW_CURSOR, g_hideCursor ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	g_hideCursor = false;
}

void tick()
{
	g_assetDataLoader.tick();

	g_scrollDelta = Vec2(0);

	glfwPollEvents();

	tickInput();
}

bool shouldShutdown()
{
	return glfwWindowShouldClose(g_glfwWindow);
}

void createMainWindow()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	g_glfwWindow = glfwCreateWindow(1280, 720, "Anker", nullptr, nullptr);
	if (!g_glfwWindow) {
		ANKER_FATAL("Could not create window");
	}

	glfwSetWindowSizeCallback(g_glfwWindow, [](GLFWwindow*, int width, int height) {
		if (width > 0 && height > 0 && g_engine) {
			g_engine->onResize({width, height});
		}
	});

	glfwSetWindowFocusCallback(g_glfwWindow, [](GLFWwindow*, int focused) { g_windowHasFocus = focused; });

	glfwSetScrollCallback(g_glfwWindow, [](GLFWwindow*, double xoffset, double yoffset) { //
		g_scrollDelta += Vec2(float(xoffset), float(yoffset));
	});

#if ANKER_PLATFORM_WINDOWS
	g_nativeWindow = glfwGetWin32Window(g_glfwWindow);
#endif

	ANKER_ASSERT(glfwRawMouseMotionSupported());
	glfwSetInputMode(g_glfwWindow, GLFW_RAW_MOUSE_MOTION, true);
}

void destroyMainWindow()
{
	glfwDestroyWindow(g_glfwWindow);
}

Vec2i windowSize()
{
	Vec2i size;
	glfwGetWindowSize(g_glfwWindow, &size.x, &size.y);
	return size;
}

bool windowHasFocus()
{
	return g_windowHasFocus;
}

GLFWwindow* glfwWindow()
{
	return g_glfwWindow;
}

NativeWindow nativeWindow()
{
	return g_nativeWindow;
}

float inputValue(MkbInput input)
{
	auto glfwInput = g_mkbMapping[int(input)];
	if (glfwInput == GLFW_KEY_UNKNOWN) {
		return 0.0f;
	}

	if (isMouseInput(input)) {
		switch (input) {
		case MkbInput::MouseWheelUp: return g_scrollDelta.y > 0;
		case MkbInput::MouseWheelDown: return g_scrollDelta.y < 0;
		case MkbInput::MouseWheelLeft: return g_scrollDelta.x < 0;
		case MkbInput::MouseWheelRight: return g_scrollDelta.x > 0;
		}
		return glfwGetMouseButton(g_glfwWindow, glfwInput) == GLFW_PRESS;
	} else {
		return glfwGetKey(g_glfwWindow, glfwInput) == GLFW_PRESS;
	}
}

float inputValue(GamepadInput input)
{
	auto pos = [](float v) { return v > 0 ? v : 0; };
	auto neg = [](float v) { return v < 0 ? -v : 0; };

	switch (input) {
	case GamepadInput::A: return g_gamepadState.pad.buttons[GLFW_GAMEPAD_BUTTON_A];
	case GamepadInput::B: return g_gamepadState.pad.buttons[GLFW_GAMEPAD_BUTTON_B];
	case GamepadInput::X: return g_gamepadState.pad.buttons[GLFW_GAMEPAD_BUTTON_X];
	case GamepadInput::Y: return g_gamepadState.pad.buttons[GLFW_GAMEPAD_BUTTON_Y];
	case GamepadInput::PadUp: return g_gamepadState.pad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];
	case GamepadInput::PadDown: return g_gamepadState.pad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];
	case GamepadInput::PadLeft: return g_gamepadState.pad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
	case GamepadInput::PadRight: return g_gamepadState.pad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
	case GamepadInput::L1: return g_gamepadState.pad.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER];
	case GamepadInput::R1: return g_gamepadState.pad.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER];
	case GamepadInput::L2: return linearDeadzone(g_gamepadState.pad.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER], Deadzone);
	case GamepadInput::R2: return linearDeadzone(g_gamepadState.pad.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER], Deadzone);
	case GamepadInput::L3: return g_gamepadState.pad.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB];
	case GamepadInput::R3: return g_gamepadState.pad.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB];
	case GamepadInput::Menu: return g_gamepadState.pad.buttons[GLFW_GAMEPAD_BUTTON_START];
	case GamepadInput::View: return g_gamepadState.pad.buttons[GLFW_GAMEPAD_BUTTON_BACK];

	case GamepadInput::LsUp: return pos(g_gamepadState.leftStick.y);
	case GamepadInput::LsDown: return neg(g_gamepadState.leftStick.y);
	case GamepadInput::LsLeft: return neg(g_gamepadState.leftStick.x);
	case GamepadInput::LsRight: return pos(g_gamepadState.leftStick.x);

	case GamepadInput::RsUp: return pos(g_gamepadState.rightStick.y);
	case GamepadInput::RsDown: return neg(g_gamepadState.rightStick.y);
	case GamepadInput::RsLeft: return neg(g_gamepadState.rightStick.x);
	case GamepadInput::RsRight: return pos(g_gamepadState.rightStick.x);

	case GamepadInput::R4:
	case GamepadInput::L4:
	case GamepadInput::R5:
	case GamepadInput::L5: return 0.0f;
	}
	ANKER_ERROR("Unknown GamepadInput input: {}", input);
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
	g_hideCursor = true;
}

void imguiImplInit()
{
	ImGui_ImplGlfw_InitForOther(g_glfwWindow, true);
}

void imguiImplNewFrame()
{
	ImGui_ImplGlfw_NewFrame();
}

void imguiImplShutdown()
{
	ImGui_ImplGlfw_Shutdown();
}

} // namespace Anker::Platform
