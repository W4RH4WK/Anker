#include <anker/game/anker_player_controller.hpp>

#include <anker/core/anker_engine.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/graphics/anker_sprite.hpp>
#include <anker/physics/anker_physics_body.hpp>

namespace Anker {

void PlayerController::tickIsGrounded(float, const PhysicsBody& body)
{
	isGrounded = false;

	// Ground contact is ignored while the player is moving upwards.
	if (velocity.y > 0) {
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
			isGrounded = true;
			break;
		}
	}
}

void PlayerController::tickMove(float dt)
{
	float responsiveness = moveResponsiveness;
	if (!isGrounded) {
		responsiveness = moveResponsivenessInAir;
	}

	float moveInput = g_engine->inputSystem.actions().playerMove().x;
	velocity.x -= responsiveness * (velocity.x - moveSpeed * moveInput);
}

void PlayerController::tickJumping(float dt, const PhysicsBody& body)
{
	const Action& jumpInput = g_engine->inputSystem.actions().playerJump;

	if (isGrounded && jumpInput.downThisFrame()) {
		velocity.y = jumpVelocity;
		isGrounded = false;
	}

	// While the player moves up, deceleration varies depending on whether the
	// jump input is still actuated or not. This allows the player to control
	// how high the jump should be.
	if (velocity.y > 0) {
		if (jumpInput.down()) {
			velocity.y -= gravity * dt;
		} else {
			velocity.y -= jumpDeceleration * dt;
		}

		// Clear vertical velocity when bumping our head on something.
		for (auto* contact : body.touchingContacts) {
			Vec2 normal = contact->GetManifold()->localNormal;
			if (contact->GetFixtureB()->GetBody() == body.body) {
				normal = -normal;
			}
			if (dot(normal, Vec2::WorldUp) >= 0.5f) {
				velocity.y = 0;
				break;
			}
		}
	}
}

void PlayerController::tickFalling(float dt)
{
	if (velocity.y > 0) {
		return;
	}

	if (isGrounded) {
		velocity.y = 0;
	} else {
		velocity.y -= gravity * dt;
	}

	if (velocity.y < -maxFallSpeed) {
		velocity.y = -maxFallSpeed;
	}
}

void PlayerController::tick(float dt, Scene& scene)
{
	for (auto [entity, physicsBody, controller] : scene.registry.view<PhysicsBody, PlayerController>().each()) {
		if (physicsBody.body) {
			controller.tickIsGrounded(dt, physicsBody);
			controller.tickMove(dt);
			controller.tickJumping(dt, physicsBody);
			controller.tickFalling(dt);

			physicsBody.body->SetLinearVelocity(controller.velocity);

			if (auto* sprite = scene.registry.try_get<Sprite>(entity)) {
				if (controller.velocity.x < 0) {
					sprite->flipX = true;
				} else if (controller.velocity.x > 0) {
					sprite->flipX = false;
				}
			}
		}
	}
}

} // namespace Anker
