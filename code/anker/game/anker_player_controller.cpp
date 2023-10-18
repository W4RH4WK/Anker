#include <anker/game/anker_player_controller.hpp>

#include <anker/core/anker_engine.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/physics/anker_physics_body.hpp>
#include <anker/physics/anker_physics_layers.hpp>

namespace Anker {

void PlayerController::tick(float dt, Scene& scene)
{
	for (auto [entity, physicsBody, controller] : scene.registry.view<PhysicsBody, PlayerController>().each()) {
		if (physicsBody.body) {
			controller.tickIsGrounded(dt, physicsBody);
			controller.tickMove(dt);
			controller.tickJumping(dt, physicsBody);
			controller.tickDropThrough(dt, physicsBody);
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
		Vec2 normal = normalFromContact(contact, body.body);
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
	if (moveInput > 0) {
		m_lookDirection = Vec2::Right;
	} else if (moveInput < 0) {
		m_lookDirection = Vec2::Left;
	}
}

void PlayerController::tickJumping(float dt, const PhysicsBody& body)
{
	const Actions& actions = g_engine->inputSystem.actions();

	if (actions.playerJump.downThisFrame() && m_jumpsLeft > 0 && !actions.playerMoveDown.down()) {
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
		if (actions.playerJump.down()) {
			m_velocity.y -= moveParam.gravity * dt;
		} else {
			m_velocity.y -= moveParam.jumpDeceleration * dt;
		}

		// Clear vertical velocity when bumping our head on something.
		for (auto* contact : body.touchingContacts) {
			Vec2 normal = normalFromContact(contact, body.body);
			if (dot(normal, Vec2::WorldUp) >= 0.5f) {
				m_velocity.y = 0;
				break;
			}
		}
	}
}

void PlayerController::tickDropThrough(float dt, const PhysicsBody& body)
{
	const Actions& actions = g_engine->inputSystem.actions();

	if (isGrounded() && actions.playerMoveDown.down() && actions.playerJump.downThisFrame()) {
		m_dropThroughTimeLeft = moveParam.dropThroughTime;
	}

	// Update fixtures, but only set filter data if mask has changed.
	for (b2Fixture* fixture = body.body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		b2Filter filter = fixture->GetFilterData();
		u16 mask = filter.maskBits;
		if (isDroppingThrough()) {
			mask &= ~PhysicsLayers::MapPlatforms;
		} else {
			mask |= PhysicsLayers::MapPlatforms;
		}
		if (mask != filter.maskBits) {
			filter.maskBits = mask;
			fixture->SetFilterData(filter);
		}
	}

	m_dropThroughTimeLeft -= dt;
}

void PlayerController::tickDashing(float dt)
{
	const Actions& actions = g_engine->inputSystem.actions();

	bool dashInput = actions.playerDash.downThisFrame();
	if (dashInput && m_dashesLeft > 0 && m_dashCooldownLeft <= 0) {
		Vec2 moveInput = actions.playerMove();
		m_dashBackwards = moveInput.x == 0;
		if (m_dashBackwards) {
			m_dashDirection = -m_lookDirection;
			m_dashTimeLeft = moveParam.dashTimeBackwards;
			m_dashCooldownLeft = moveParam.dashBackwardsCooldown;
		} else {
			m_dashDirection = moveInput.x > 0 ? Vec2::WorldRight : Vec2::WorldLeft;
			m_dashTimeLeft = moveParam.dashTime;
			m_dashCooldownLeft = moveParam.dashCooldown;
		}
		m_dashesLeft--;
	}

	if (isGrounded() && m_dashCooldownLeft <= 0) {
		m_dashesLeft = moveParam.dashes;
	}

	if (isDashing()) {
		float speed = moveParam.dashSpeed;
		if (m_dashBackwards) {
			speed = moveParam.dashSpeedBackwards;
		}
		m_velocity = speed * m_dashDirection;
	}

	m_dashTimeLeft -= dt;
	m_dashCooldownLeft -= dt;
}

void PlayerController::tickFalling(float dt)
{
	if (isJumping() || isDashing()) {
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
