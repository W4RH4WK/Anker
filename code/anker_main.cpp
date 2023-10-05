#include <anker/core/anker_engine.hpp>
#include <anker/game/anker_map.hpp>
#include <anker/platform/anker_platform.hpp>

using namespace Anker;

int main(int argc, char* argv[])
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

	{
		std::string mapName = "maps/gym";
		if (argc > 1) {
			mapName = std::string("maps/") + argv[1];
		}
		g_engine->nextScene = loadMap(mapName);
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
