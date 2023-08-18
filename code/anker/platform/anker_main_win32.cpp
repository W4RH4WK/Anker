#include <anker/core/anker_data_loader.hpp>
#include <anker/core/anker_data_loader_filesystem.hpp>
#include <anker/core/anker_engine.hpp>
#include <anker/core/anker_transform.hpp>
#include <anker/editor/anker_editor_camera.hpp>
#include <anker/graphics/anker_sprite.hpp>

using namespace Anker;

int main()
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

	DataLoader dataLoader;
	DataLoaderFilesystem dataLoaderFs("assets");
	dataLoader.addSource(&dataLoaderFs);

	g_engine.emplace(dataLoader);

	glfwSetWindowSizeCallback(g_engine->window, [](GLFWwindow*, int width, int height) {
		if (width > 0 && height > 0) {
			g_engine->onResize({width, height});
		}
	});

	glfwSetScrollCallback(g_engine->window, [](GLFWwindow*, double, double yoffset) {
		g_engine->inputSystem.onScroll(float(yoffset)); //
	});

	g_engine->editorSystem.emplace(*g_engine);

	g_engine->activeScene = Scene::create();

	{
		auto e = g_engine->activeScene->createEntity("testsprite");
		e.emplace<Transform2D>(Transform2D{
		    .position = {-10.0f, 0.0f},
		    //.rotation = glm::radians(45.0f),
		    //.scale = {0.5f, 0.5f},
		});

		e.emplace<Sprite>().texture = g_engine->assetCache.loadTexture("textures/player");

		g_engine->activeScene->activeCamera.emplace<EditorCamera>();
	}

	while (!g_engine->window.isClosed()) {
		glfwPollEvents();

		g_engine->tick();
	}

	g_engine.reset();

	glfwTerminate();

	return 0;
}
