#include <anker/game/anker_player_controller.hpp>

#include <anker/core/anker_engine.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/physics/anker_physics_body.hpp>

namespace Anker {

void PlayerController::tick(float dt, Scene& scene)
{
	for (auto [entity, physicsBody, controller] : scene.registry.view<PhysicsBody, PlayerController>().each()) {
		if (physicsBody.body) {
			controller.tickIsGrounded(dt, physicsBody);
			controller.tickMove(dt);
			controller.tickJumping(dt, physicsBody);
			controller.tickFalling(dt);

			physicsBody.body->SetLinearVelocity(controller.velocity());
		}
	}
}

void PlayerController::tickIsGrounded(float, const PhysicsBody& body)
{
	m_isGrounded = false;

	// Ground contact is ignored while the player is moving upwards.
	if (m_velocity.y > 0) {
		return;
	}

	// Check contact points for ground contact. We also take slopes into account
	// unless they are too steep.
	for (auto* contact : body.touchingContacts) {
		Vec2 normal = contact->GetManifold()->localNormal;
		if (contact->GetFixtureB()->GetBody() == body.body) {
			normal = -normal;
		}
		if (dot(normal, Vec2::WorldDown) >= 0.5f) {
			m_isGrounded = true;
			break;
		}
	}
}

void PlayerController::tickMove(float dt)
{
	float responsiveness = moveParam.moveResponsiveness;
	if (!m_isGrounded) {
		responsiveness = moveParam.moveResponsivenessInAir;
	}

	float moveInput = g_engine->inputSystem.actions().playerMove().x;
	m_velocity.x = moveTowards(m_velocity.x, moveParam.moveSpeed * moveInput, responsiveness, dt);
}

void PlayerController::tickJumping(float dt, const PhysicsBody& body)
{
	const Action& jumpInput = g_engine->inputSystem.actions().playerJump;

	if (m_isGrounded && jumpInput.downThisFrame()) {
		m_velocity.y = moveParam.jumpSpeed;
		m_isGrounded = false;
	}

	// While the player moves up, deceleration varies depending on whether the
	// jump input is still actuated or not. This allows the player to control
	// how high the jump should be.
	if (m_velocity.y > 0) {
		if (jumpInput.down()) {
			m_velocity.y -= moveParam.gravity * dt;
		} else {
			m_velocity.y -= moveParam.jumpDeceleration * dt;
		}

		// Clear vertical velocity when bumping our head on something.
		for (auto* contact : body.touchingContacts) {
			Vec2 normal = contact->GetManifold()->localNormal;
			if (contact->GetFixtureB()->GetBody() == body.body) {
				normal = -normal;
			}
			if (dot(normal, Vec2::WorldUp) >= 0.5f) {
				m_velocity.y = 0;
				break;
			}
		}
	}
}

void PlayerController::tickFalling(float dt)
{
	if (m_velocity.y > 0) {
		return;
	}

	if (m_isGrounded) {
		m_velocity.y = 0;
	} else {
		m_velocity.y -= moveParam.gravity * dt;
	}

	if (m_velocity.y < -moveParam.maxFallSpeed) {
		m_velocity.y = -moveParam.maxFallSpeed;
	}
}

} // namespace Anker
