#include <anker/platform/anker_platform_glfw.hpp>

#include <imgui_impl_glfw.h>

#include <anker/core/anker_data_loader.hpp>
#include <anker/core/anker_data_loader_filesystem.hpp>
#include <anker/core/anker_engine.hpp>

namespace Anker::Platform {

static GLFWwindow* g_glfwWindow = nullptr;
static NativeWindow g_nativeWindow = nullptr;

static std::optional<DataLoaderFilesystem> g_assetDataLoaderFs;

static bool g_hideCursor = false;

void initialize()
{
	glfwSetErrorCallback([](int error_code, const char* description) { //
		ANKER_ERROR("GLFW: {} {}", error_code, description);
	});

	if (!glfwInit()) {
		ANKER_FATAL("Failed to initialize GLFW");
	}

	g_assetDataLoader.addSource(&g_assetDataLoaderFs.emplace("assets"));
}

void finalize()
{
	g_assetDataLoader.removeSource(&*g_assetDataLoaderFs);
	g_assetDataLoaderFs.reset();
	glfwTerminate();
}

void tick()
{
	g_assetDataLoader.tick();

	glfwPollEvents();

	// Any component that wants to hide the cursor calls hideCursor. If none
	// does, we display the cursor as normal.
	glfwSetInputMode(g_glfwWindow, GLFW_CURSOR, g_hideCursor ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	g_hideCursor = false;
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

	glfwSetScrollCallback(g_glfwWindow, [](GLFWwindow*, double, double yoffset) {
		if (g_engine) {
			g_engine->inputSystem.onScroll(float(yoffset));
		}
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

GLFWwindow* glfwWindow()
{
	return g_glfwWindow;
}

NativeWindow nativeWindow()
{
	return g_nativeWindow;
}

Vec2 cursorPosition()
{
	double x, y;
	glfwGetCursorPos(g_glfwWindow, &x, &y);
	return {float(x), float(y)};
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
