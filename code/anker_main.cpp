#include <anker/core/anker_engine.hpp>
#include <anker/core/anker_scene_node.hpp>
#include <anker/editor/anker_editor_camera.hpp>
#include <anker/game/anker_follower.hpp>
#include <anker/game/anker_player.hpp>
#include <anker/graphics/anker_camera.hpp>
#include <anker/platform/anker_platform.hpp>

using namespace Anker;

int main()
{
	{
		AllocConsole();
		std::FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);
	}

	Platform::initialize();
	Platform::createMainWindow();

	g_engine.emplace();
	g_engine->editor.emplace();

	g_engine->activeScene = g_engine->createScene();

	auto player = spawnPlayer(*g_engine->activeScene, {10.5f, -8.0f});

	auto camera = g_engine->activeScene->activeCamera();
	camera.get<SceneNode>().setGlobalTransform(player.get<SceneNode>().globalTransform());
	camera.get<Camera>().distance = 3;
	camera.emplace<Follower>(player).speed = 8.0f;

	if (not loadMap(*g_engine->activeScene, "maps/sewers/sewers", g_engine->assetCache)) {
		ANKER_ERROR("Failed to load map");
	}

	for (auto [_, node] : g_engine->activeScene->registry.view<SceneNode>().each()) {
		ANKER_ASSERT(node.validateParentChildLink());
	}

	while (!Platform::shouldShutdown()) {
		Platform::tick();
		g_engine->tick();
	}

	g_engine.reset();

	Platform::destroyMainWindow();
	Platform::finalize();

	return 0;
}
