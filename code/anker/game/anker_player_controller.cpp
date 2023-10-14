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
			controller.tickDashing(dt);
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
		if (dot(normal, Vec2::WorldDown) >= 0.75f) {
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

	if (jumpInput.downThisFrame() && m_jumpsLeft > 0) {
		if (m_jumpsLeft == moveParam.jumps) {
			m_velocity.y = moveParam.jumpSpeed;
		} else {
			m_velocity.y = moveParam.jumpSpeedSecondary;
		}
		m_isGrounded = false;
		m_jumpsLeft--;
	}

	// Update jumps
	if (isGrounded()) {
		m_jumpsLeft = moveParam.jumps;
	}

	// Update coyote time
	if (isFalling()) {
		m_coyoteTimeLeft -= dt;
	} else {
		m_coyoteTimeLeft = moveParam.coyoteTime;
	}

	// We lose the primary jump when falling (after coyote time).
	if (isFalling() && m_coyoteTimeLeft <= 0 && m_jumpsLeft == moveParam.jumps) {
		m_jumpsLeft--;
	}

	// While the player moves up, deceleration varies depending on whether the
	// jump input is still actuated or not. This allows the player to control
	// how high the jump should be.
	if (isJumping()) {
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

void PlayerController::tickDashing(float dt)
{
	float direction = g_engine->inputSystem.actions().playerMove().x;
	bool dashInput = g_engine->inputSystem.actions().playerDash.downThisFrame();
	if (dashInput && m_dashesLeft > 0 && m_dashCooldownLeft <= 0) {
		m_dashesLeft--;
		m_dashTimeLeft = moveParam.dashTime;
		m_dashCooldownLeft = moveParam.dashCooldown;
	}

	m_dashTimeLeft -= dt;
	m_dashCooldownLeft -= dt;

	if (isGrounded()) {
		m_dashesLeft = moveParam.dashes;
	}

	if (isDashing()) {
		m_velocity = {direction * moveParam.dashSpeed, 0};
	}
}

void PlayerController::tickFalling(float dt)
{
	if (m_velocity.y > 0 || isDashing()) {
		return;
	}

	if (isGrounded()) {
		m_velocity.y = 0;
	} else {
		m_velocity.y -= moveParam.gravity * dt;
	}

	if (m_velocity.y < -moveParam.maxFallSpeed) {
		m_velocity.y = -moveParam.maxFallSpeed;
	}
}

} // namespace Anker
