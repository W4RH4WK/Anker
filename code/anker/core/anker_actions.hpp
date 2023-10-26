#pragma once

#include <anker/core/anker_inputs.hpp>

namespace Anker {

constexpr float InputAnalogToDigitalThreshold = 0.3f;
constexpr float InputHoldThreshold = 0.3f;

// An Action is represented as a single floating-point value between 0 and 1.
// This value can be interpreted as a digital input (i.e. button), including
// specific states like tapped or holding.
class Action {
  public:
	Action() = default;
	Action(MkbInput bindingMkb) : bindingMkb1(bindingMkb) {}
	Action(MkbInput bindingMkb, GamepadInput bindingGamepad) : bindingMkb1(bindingMkb), bindingGamepad1(bindingGamepad)
	{}

	// These functions interpret the floating-point value as button input:
	bool down() const { return value() > InputAnalogToDigitalThreshold; }
	bool downThisFrame() const { return down() && m_previousTime == 0; }
	bool up() const { return value() == 0; }
	bool upThisFrame() const { return up() && m_previousTime > 0; }
	bool tapped() const { return upThisFrame() && m_previousTime < InputHoldThreshold; }
	bool holding() const { return down() && m_currentTime >= InputHoldThreshold; }
	bool holdingThisFrame() const { return holding() && m_previousTime < InputHoldThreshold; }

	// Button inputs are typically queried via downThisFrame. We provide a
	// shorthand for this common case.
	explicit operator bool() const { return downThisFrame(); }

	float value() const { return m_consumed ? 0.0f : m_value; }

	// Consume suppresses the actuation for a single frame. The value will
	// appears as if no actuation is taking place.
	void consume() { m_consumed = true; }

	void tick(float dt);

	std::optional<MkbInput> bindingMkb1, bindingMkb2;
	std::optional<GamepadInput> bindingGamepad1, bindingGamepad2;

  private:
	float m_value = 0;
	float m_currentTime = 0;
	float m_previousTime = 0;
	bool m_consumed = false;
};

// All input actions are combined into this struct. The actions are updated by
// the input system.
struct Actions {
	Action playerMoveLeft = {MkbInput::Left, GamepadInput::PadLeft};
	Action playerMoveRight = {MkbInput::Right, GamepadInput::PadRight};
	Action playerMoveUp = {MkbInput::Up, GamepadInput::PadUp};
	Action playerMoveDown = {MkbInput::Down, GamepadInput::PadDown};
	Action playerJump = {MkbInput::Space, GamepadInput::A};
	Action playerDash = {MkbInput::LeftShift, GamepadInput::B};

	Vec2 playerMove() const
	{
		return (Vec2::WorldLeft * playerMoveLeft.value()     //
		        + Vec2::WorldRight * playerMoveRight.value() //
		        + Vec2::WorldUp * playerMoveUp.value()       //
		        + Vec2::WorldDown * playerMoveDown.value())  //
		    .clampLength(1);
	}

	Action editorToggle = {MkbInput::F1};
	Action editorMapReload = {MkbInput::F5};
	Action editorCameraActivate = {MkbInput::MouseRight};
	Vec2 editorCameraPan;
	float editorCameraZoom = 0;
};

} // namespace Anker

REFL_TYPE(Anker::Actions)
REFL_FIELD(playerMoveLeft)
REFL_FIELD(playerMoveRight)
REFL_FIELD(playerMoveUp)
REFL_FIELD(playerMoveDown)
REFL_FIELD(playerJump)
REFL_FIELD(playerDash)
REFL_FIELD(editorToggle)
REFL_FIELD(editorMapReload)
REFL_FIELD(editorCameraActivate)
REFL_END
