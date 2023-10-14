#pragma once

#include <anker/game/anker_player_movement_parameters.hpp>

namespace Anker {

class Scene;
struct PhysicsBody;

class PlayerController {
  public:
	bool isGrounded() const { return m_isGrounded; }
	bool isJumping() const { return !m_isGrounded && m_velocity.y > 0; }
	bool isFalling() const { return !m_isGrounded && m_velocity.y < 0; }
	bool isDashing() const { return m_dashTimeLeft > 0; }
	Vec2 velocity() const { return m_velocity; }

	static void tick(float, Scene&);

	PlayerMovementParameters moveParam;

  private:
	void tickIsGrounded(float, const PhysicsBody&);
	void tickMove(float);
	void tickJumping(float, const PhysicsBody&);
	void tickDashing(float);
	void tickFalling(float);

	bool m_isGrounded = false;

	int m_jumpsLeft = 1;
	float m_coyoteTimeLeft = 0;

	int m_dashesLeft = 1;
	float m_dashTimeLeft = 0;
	float m_dashCooldownLeft = 0;

	Vec2 m_velocity;
};

} // namespace Anker

REFL_TYPE(Anker::PlayerController)
REFL_FIELD(moveParam)
REFL_END
