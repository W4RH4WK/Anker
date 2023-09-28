#pragma once

#include <anker/core/anker_engine.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_scene_node.hpp>
#include <anker/game/anker_player_controller.hpp>
#include <anker/graphics/anker_sprite.hpp>
#include <anker/physics/anker_physics_body.hpp>

namespace Anker {

inline EntityHandle spawnPlayer(Scene& scene, Vec2 position, SceneNode* parent = nullptr)
{
	EntityHandle player = scene.createEntity("Player");
	player.emplace<SceneNode>(Transform2D(position), parent);
	player.emplace<Sprite>(Sprite{
	    .offset = {-0.5f, -0.5f},
	    .pixelToMeter = 512,
	    .texture = g_engine->assetCache.loadTexture("textures/player"),
	});

	{
		b2Body* body = player.emplace<PhysicsBody>().body;
		body->SetFixedRotation(true);
		body->SetSleepingAllowed(false);
		body->SetGravityScale(0);

		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(0.25f, 0.5f);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.0f;

		body->CreateFixture(&fixtureDef);
	}

	player.emplace<PlayerController>();

	return player;
}

} // namespace Anker
