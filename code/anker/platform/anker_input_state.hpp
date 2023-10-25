#pragma once

namespace Anker {

enum class KeyInput {
	Left,
	Right,
	Up,
	Down,
	Space,
	Shift,
};
constexpr usize KeyInputCount = 6;

enum class GamepadInput {
	LsLeft,
	LsRight,
	LsUp,
	LsDown,
	A,
	B,
};
constexpr usize GamepadInputCount = 6;

struct InputState {
	std::array<float, KeyInputCount> keys = {};
	std::array<float, GamepadInputCount> gamepad = {};

	Vec2 cursorPosition;
	Vec2 cursorDelta;
	float scrollDelta = 0;

	float& operator[](KeyInput input)
	{
		ANKER_ASSERT_INBOUNDS(input, keys.size());
		return keys[usize(input)];
	}
	float operator[](KeyInput key) const { return const_cast<InputState&>(*this)[key]; }

	float& operator[](GamepadInput input)
	{
		ANKER_ASSERT_INBOUNDS(input, gamepad.size());
		return gamepad[usize(input)];
	}
	float operator[](GamepadInput input) const { return const_cast<InputState&>(*this)[input]; }
};

} // namespace Anker
