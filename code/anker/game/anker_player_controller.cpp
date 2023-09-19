#include <anker/game/anker_player_controller.hpp>

#include <anker/core/anker_scene.hpp>
#include <anker/physics/anker_physics_body.hpp>
#include <anker/platform/anker_input_system.hpp>

namespace Anker {

PlayerControllerSystem::PlayerControllerSystem(InputSystem& inputSystem) : m_inputSystem(inputSystem) {}

void PlayerControllerSystem::tick(float dt, Scene& scene)
{
	const auto& actions = m_inputSystem.actions();

	for (auto [_, physicsBody] : scene.registry.view<PhysicsBody, PlayerController>().each()) {
		physicsBody.body->ApplyForceToCenter(1000.0f * actions.playerMove * dt, true);
	}
}

} // namespace Anker
