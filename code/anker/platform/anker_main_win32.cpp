#include <anker/core/anker_data_loader.hpp>
#include <anker/core/anker_data_loader_filesystem.hpp>
#include <anker/core/anker_engine.hpp>
#include <anker/platform/anker_platform.hpp>

#include <anker/core/anker_transform.hpp>
#include <anker/editor/anker_editor_camera.hpp>
#include <anker/graphics/anker_sprite.hpp>
#include <anker/physics/anker_physics_body.hpp>

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
	g_engine->editor.emplace();

	g_engine->activeScene = g_engine->createScene();

	g_engine->activeScene->activeCamera().emplace<EditorCamera>();

	{
		auto ground = g_engine->activeScene->createEntity("Ground");
		ground.emplace<Transform2D>();

		auto& physicsBody = ground.emplace<PhysicsBody>();

		b2BodyDef bodyDef;
		bodyDef.type = b2_staticBody;
		bodyDef.position = {0, -10};

		physicsBody.body = g_engine->activeScene->physicsWorld->CreateBody(&bodyDef);

		b2PolygonShape groundBox;
		groundBox.SetAsBox(50, 10);

		physicsBody.body->CreateFixture(&groundBox, 0);
	}

	{
		auto player = g_engine->activeScene->createEntity("Player");
		player.emplace<Transform2D>();
		player.emplace<Sprite>().texture = g_engine->assetCache.loadTexture("textures/player");

		auto& physicsBody = player.emplace<PhysicsBody>();

		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position = {0, 10};
		bodyDef.angle = 15.0f * Degrees;

		physicsBody.body = g_engine->activeScene->physicsWorld->CreateBody(&bodyDef);

		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(1.0f, 1.0f);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;

		physicsBody.body->CreateFixture(&fixtureDef);
	}

	while (!g_platform->shouldShutdown()) {
		g_platform->tick();
		g_engine->tick();
	}

	g_engine.reset();
	g_platform.reset();

	return 0;
}
