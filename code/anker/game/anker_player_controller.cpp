#include <anker/game/anker_player_controller.hpp>

#include <anker/core/anker_engine.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/physics/anker_physics_body.hpp>

namespace Anker {

void PlayerController::tick(float dt, Scene& scene)
{
	const auto& actions = g_engine->inputSystem.actions();

	for (auto [_, physicsBody] : scene.registry.view<PhysicsBody, PlayerController>().each()) {
		physicsBody.body->ApplyForceToCenter(1000.0f * actions.playerMove * dt, true);
	}
}

} // namespace Anker
