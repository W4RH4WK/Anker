#include <anker/core/anker_data_loader.hpp>
#include <anker/core/anker_data_loader_filesystem.hpp>
#include <anker/core/anker_engine.hpp>
#include <anker/platform/anker_platform_win32.hpp>

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

	g_platform.emplace();

	DataLoader dataLoader;
	DataLoaderFilesystem dataLoaderFs("assets");
	dataLoader.addSource(&dataLoaderFs);

	g_engine.emplace(dataLoader);
	g_engine->editorSystem.emplace();

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

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);

	b2Body* groundBody = g_engine->activeScene->physicsWorld.CreateBody(&groundBodyDef);

	b2PolygonShape groundBox;
	groundBox.SetAsBox(50.0f, 10.0f);

	groundBody->CreateFixture(&groundBox, 0.0f);

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 4.0f);

	b2Body* body = g_engine->activeScene->physicsWorld.CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1.0f, 1.0f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;

	body->CreateFixture(&fixtureDef);

	while (!g_platform->shouldShutdown()) {
		g_platform->tick();
		g_engine->tick();

		ANKER_INFO("{} {}", body->GetPosition().x, body->GetPosition().y);
	}

	g_engine.reset();
	g_platform.reset();

	return 0;
}
