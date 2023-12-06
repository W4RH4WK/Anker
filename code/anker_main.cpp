#include <anker/core/anker_engine.hpp>
#include <anker/game/anker_map.hpp>
#include <anker/platform/anker_platform.hpp>

using namespace Anker;

int main(int argc, char* argv[])
{
	Platform::initialize();
	Platform::createMainWindow();

	g_engine.emplace();
	g_engine->editor.emplace();

	{
		std::string mapName = argc > 1 ? argv[1] : "gym";
		g_engine->nextScene = loadMap("maps/" + mapName);
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
