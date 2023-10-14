#pragma once

namespace Anker {

struct PlayerMovementParameters {
	// moveSpeed determines the player's horizontal movement speed.
	float moveSpeed = 8.0f;

	// moveResponsiveness determines how floaty the horizontal movement feels.
	// Responsiveness is typically lower (but > 0) while in the air.
	float moveResponsiveness = 12.0f;
	float moveResponsivenessInAir = 10.0f;

	// jumps determines how many jumps the player has.
	int jumps = 2;

	// jumpSpeed determines the initial velocity when jumping, therefore how
	// high we can jump.
	float jumpSpeed = 23.5f;

	// Jumps beyond the primary jump use a slightly different strength.
	float jumpSpeedSecondary = 18.8f;

	// Jump height can be reduced by letting go of the jump input, in which
	// case, this deceleration is used until the player starts falling.
	float jumpDeceleration = 225.0f;

	// coyoteTime determines how long the player can still use their primary
	// jump after running of a ledge.
	float coyoteTime = 0.15f;

	// gravity determines how fast the player accelerates towards the ground.
	float gravity = 56.25f;

	// Falling speed is capped at this value.
	float maxFallSpeed = 30.0f;
};

} // namespace Anker

REFL_TYPE(Anker::PlayerMovementParameters)
REFL_FIELD(moveSpeed)
REFL_FIELD(moveResponsiveness)
REFL_FIELD(moveResponsivenessInAir)
REFL_FIELD(jumps)
REFL_FIELD(jumpSpeed)
REFL_FIELD(jumpSpeedSecondary)
REFL_FIELD(jumpDeceleration)
REFL_FIELD(coyoteTime)
REFL_FIELD(gravity)
REFL_FIELD(maxFallSpeed)
REFL_END
