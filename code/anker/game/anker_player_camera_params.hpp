#pragma once

namespace Anker {

struct PlayerCameraParams {
	// distance determines how far the camera is pulled out.
	float distance = 4.0f;

	// baseOffset determines how far the camera is offset relative to the
	// player's position.
	Vec2 baseOffset = {0.0f, 1.0f};

	// lookOffset determines how far the camera is offset horizontally in the
	// direction the player is looking.
	float lookOffset = 0.3f;

	// baseSpeed determines how fast the camera is tracking the player normally.
	Vec2 baseSpeed = {6.0f, 4.5f};

	// fallingOffset determines the camera's vertical offset when the player is
	// falling. This offset is scaled with the player's vertical velocity.
	float fallingOffset = 0.25f;

	// fallingOffsetDelay determines how long the player needs to fall before
	// fallingOffset kicks in.
	float fallingOffsetDelay = 0.4f;

	// dashingSpeed determines the camera's horizontal tracking speed when the
	// player is dashing.
	float dashingSpeed = 20.0f;
};

} // namespace Anker

REFL_TYPE(Anker::PlayerCameraParams)
REFL_FIELD(distance)
REFL_FIELD(baseOffset)
REFL_FIELD(lookOffset)
REFL_FIELD(baseSpeed)
REFL_FIELD(fallingOffset)
REFL_FIELD(fallingOffsetDelay)
REFL_FIELD(dashingSpeed)
REFL_END
