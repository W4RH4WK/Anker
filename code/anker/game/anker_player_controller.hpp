#pragma once

namespace Anker {

class Scene;

struct PhysicsBody;

struct PlayerController {
	float moveSpeed = 10.0f;
	float moveResponsiveness = 0.32f;
	float moveResponsivenessInAir = 0.16f;

	float jumpVelocity = 23.5f;
	float jumpDeceleration = 225.0f;

	float gravity = 56.25f;
	float maxFallSpeed = 30.0f;

	bool isGrounded = false;

	Vec2 velocity;

	bool isJumping() const { return !isGrounded && velocity.y > 0; }

	void tickIsGrounded(float, const PhysicsBody&);
	void tickMove(float);
	void tickJumping(float, const PhysicsBody&);
	void tickFalling(float);

	static void tick(float, Scene&);
};

} // namespace Anker

REFL_TYPE(Anker::PlayerController)
REFL_FIELD(moveSpeed)
REFL_FIELD(moveResponsiveness)
REFL_FIELD(moveResponsivenessInAir)
REFL_FIELD(isGrounded)
REFL_FIELD(velocity)
REFL_END
