#pragma once

#include <anker/core/anker_engine.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_scene_node.hpp>
#include <anker/game/anker_player_animator.hpp>
#include <anker/game/anker_player_controller.hpp>
#include <anker/graphics/anker_sprite.hpp>
#include <anker/physics/anker_physics_body.hpp>
#include <anker/physics/anker_physics_layers.hpp>

namespace Anker {

using PlayerTag = entt::tag<"Player"_hs>;

inline EntityHandle spawnPlayer(Scene& scene, Vec2 position, SceneNode* parent = nullptr)
{
	EntityHandle player = scene.createEntity("Player");
	player.emplace<PlayerTag>();
	player.emplace<SceneNode>(Transform2D(position), parent);
	player.emplace<Sprite>(Sprite{
	    .offset = {-0.5f, -0.5f},
	    .texture = g_engine->assetCache.loadTexture("textures/player"),
	});

	{
		b2Body* body = player.emplace<PhysicsBody>().body;
		body->SetFixedRotation(true);
		body->SetSleepingAllowed(false);
		body->SetGravityScale(0);

		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(0.25f, 0.4f);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.0f;
		fixtureDef.filter.categoryBits = PhysicsLayers::Player;

		body->CreateFixture(&fixtureDef);
	}

	player.emplace<PlayerController>();
	player.emplace<PlayerAnimator>();

	return player;
}

} // namespace Anker
