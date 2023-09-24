#include <anker/game/anker_player_controller.hpp>

#include <anker/core/anker_engine.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/physics/anker_physics_body.hpp>

namespace Anker {

void PlayerController::tick(float, Scene& scene)
{
	const auto& actions = g_engine->inputSystem.actions();

	for (auto [_, physicsBody] : scene.registry.view<PhysicsBody, PlayerController>().each()) {
		if (physicsBody.body) {
			Vec2 velocity = 6.0f * actions.playerMove + 10.0f * Vec2::WorldDown;
			physicsBody.body->SetLinearVelocity(velocity);
		}
	}
}

} // namespace Anker
