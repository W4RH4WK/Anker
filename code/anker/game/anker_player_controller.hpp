#pragma once

#include <anker/editor/anker_inspector_widget_drawer.hpp>
#include <anker/game/anker_player_movement_params.hpp>

namespace Anker {

class Scene;
struct PhysicsBody;

class PlayerController {
  public:
	bool isGrounded() const { return m_isGrounded; }
	bool isJumping() const { return !m_isGrounded && m_velocity.y > 0; }
	bool isDroppingThrough() const { return m_dropThroughTimeLeft > 0; }
	bool isFalling() const { return !m_isGrounded && m_velocity.y < 0; }
	bool isDashing() const { return m_dashTimeLeft > 0; }
	bool isDashingBackwards() const { return isDashing() && m_dashBackwards; }
	Vec2 velocity() const { return m_velocity; }
	Vec2 lookDirection() const { return m_lookDirection; }

	static void tick(float, Scene&);

	PlayerMovementParams moveParam;

  private:
	void tickIsGrounded(float, const PhysicsBody&);
	void tickMove(float);
	void tickJumping(float, const PhysicsBody&);
	void tickDropThrough(float, const PhysicsBody&);
	void tickDashing(float);
	void tickFalling(float);

	bool m_isGrounded = false;

	int m_jumpsLeft = 1;
	float m_coyoteTimeLeft = 0;

	int m_dashesLeft = 1;
	float m_dashTimeLeft = 0;
	float m_dashCooldownLeft = 0;
	bool m_dashBackwards = false;
	Vec2 m_dashDirection;

	float m_dropThroughTimeLeft = 0;

	Vec2 m_velocity;
	Vec2 m_lookDirection = Vec2::WorldRight;
};

inline bool serialize(InspectorWidgetDrawer draw, PlayerController& controller)
{
	ImGui::Text("Velocity %f %f", controller.velocity().x, controller.velocity().y);
	ImGui::Text("Grounded: %d", controller.isGrounded());
	ImGui::Text("Jumping:  %d", controller.isJumping());
	ImGui::Text("Dropping: %d", controller.isDroppingThrough());
	ImGui::Text("Falling:  %d", controller.isFalling());
	ImGui::Text("Dashing:  %d", controller.isDashing());

	return draw.field("moveParam", controller.moveParam);
}

} // namespace Anker

REFL_TYPE(Anker::PlayerController)
REFL_FIELD(moveParam)
REFL_END
