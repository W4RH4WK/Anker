#include <anker/core/anker_data_loader.hpp>
#include <anker/core/anker_data_loader_filesystem.hpp>
#include <anker/core/anker_engine.hpp>
#include <anker/core/anker_scene_node.hpp>
#include <anker/game/anker_player_controller.hpp>
#include <anker/graphics/anker_camera.hpp>
#include <anker/platform/anker_platform.hpp>

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

	Platform::initialize();
	Platform::createMainWindow();

	g_engine.emplace();
	g_engine->editor.emplace();

	g_engine->activeScene = g_engine->createScene();

	g_engine->activeScene->activeCamera().emplace<EditorCamera>();
	g_engine->activeScene->activeCamera().get<SceneNode>().setLocalTransform(Transform2D(Vec2{10.5f, -14.0f}));
	g_engine->activeScene->activeCamera().get<Camera>().distance = 3;

	// auto font = g_engine->assetCache.loadFont("fonts/FTAnchorYard-Regular");

	{
		auto player = g_engine->activeScene->createEntity("Player");
		player.emplace<SceneNode>();
		player.emplace<PlayerController>();
		player.emplace<Sprite>(Sprite{
		    .layer = LayerPlayer,
		    .offset = {-0.5f, -0.5f},
		    .pixelToMeter = 512,
		    .texture = g_engine->assetCache.loadTexture("textures/player"),
		});

		auto& physicsBody = player.emplace<PhysicsBody>();

		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position = {10.5f, -8};
		bodyDef.fixedRotation = true;
		bodyDef.allowSleep = false;

		physicsBody.body = g_engine->activeScene->physicsWorld->CreateBody(&bodyDef);

		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(0.25f, 0.5f);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 1.0f;

		physicsBody.body->CreateFixture(&fixtureDef);
	}

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
