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
	float jumpSpeed = 20.0f;

	// Jumps beyond the primary jump use a slightly different strength.
	float jumpSpeedSecondary = 18.0f;

	// Jump height can be reduced by letting go of the jump input, in which
	// case, this deceleration is used until the player starts falling.
	float jumpDeceleration = 225.0f;

	// coyoteTime determines how long the player can still use their primary
	// jump after running of a ledge.
	float coyoteTime = 0.15f;

	// dashes determines how many dashes the player has.
	int dashes = 1;

	// dashSpeed determines the strength of a dash.
	float dashSpeed = 30.0f;

	// dashSpeedBackwards determines the strength of the backwards dash.
	float dashSpeedBackwards = 20.0f;

	// dashTime determines how long the dash persists.
	float dashTime = 0.05f;

	// dashTimeBackwards determines how long the backwards dash persists.
	float dashTimeBackwards = 0.02f;

	// dashCooldown determines how long the player has to wait before dashing
	// again.
	float dashCooldown = 0.6f;

	// dashBackwardsCooldown determines how long the player has to wait before
	// dashing again after doing a backwards dash.
	float dashBackwardsCooldown = 0.4f;

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
REFL_FIELD(dashes)
REFL_FIELD(dashSpeed)
REFL_FIELD(dashSpeedBackwards)
REFL_FIELD(dashTime)
REFL_FIELD(dashTimeBackwards)
REFL_FIELD(dashCooldown)
REFL_FIELD(dashBackwardsCooldown)
REFL_FIELD(gravity)
REFL_FIELD(maxFallSpeed)
REFL_END
