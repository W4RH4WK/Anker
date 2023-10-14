#pragma once

#include <anker/game/anker_player_movement_parameters.hpp>

namespace Anker {

class Scene;
struct PhysicsBody;

class PlayerController {
  public:
	bool isGrounded() const { return m_isGrounded; }
	bool isJumping() const { return !m_isGrounded && m_velocity.y > 0; }
	Vec2 velocity() const { return m_velocity; }

	static void tick(float, Scene&);

	PlayerMovementParameters moveParam;

  private:
	void tickIsGrounded(float, const PhysicsBody&);
	void tickMove(float);
	void tickJumping(float, const PhysicsBody&);
	void tickFalling(float);

	bool m_isGrounded = false;

	Vec2 m_velocity;
};

} // namespace Anker

REFL_TYPE(Anker::PlayerController)
REFL_END
