#pragma once

namespace Anker {

////////////////////////////////////////////////////////////
// Mouse / Keyboard Input

enum class MkbInput {
#define ANKER_INPUTS_KEY(_name) _name,
#define ANKER_INPUTS_MOUSE(_name) _name,
#include "anker_inputs.inc"
};
template <>
constexpr std::array EnumEntries<MkbInput> = {
#define ANKER_INPUTS_KEY(_name) std::pair(MkbInput::_name, #_name##_hs),
#define ANKER_INPUTS_MOUSE(_name) std::pair(MkbInput::_name, #_name##_hs),
#include "anker_inputs.inc"
};

inline bool isMouseInput(MkbInput input)
{
	return MkbInput::MouseLeft <= input && input <= MkbInput::MouseWheelRight;
}

////////////////////////////////////////////////////////////
// Gamepad Input

enum class GamepadInput {
#define ANKER_INPUTS_GAMEPAD(_name) _name,
#include "anker_inputs.inc"
};
template <>
constexpr std::array EnumEntries<GamepadInput> = {
#define ANKER_INPUTS_GAMEPAD(_name) std::pair(GamepadInput::_name, #_name##_hs),
#include "anker_inputs.inc"
};

////////////////////////////////////////////////////////////
// Deadzones

inline float linearDeadzone(float value, float lower, float upper = 1)
{
	auto abs = std::abs(value);
	auto sign = value < 0.0f ? -1.0f : 1.0f;
	if (abs <= lower) {
		return 0;
	} else if (abs >= upper) {
		return sign;
	} else {
		return sign * (abs - lower) / (upper - lower);
	}
}

inline Vec2 radialDeadzone(Vec2 value, float lower, float upper = 1)
{
	auto len = value.length();
	if (len <= lower) {
		return Vec2(0);
	} else if (len >= upper) {
		return value.normalize();
	} else {
		len = (len - lower) / (upper - lower);
		return static_cast<float>(len) * value.normalize();
	}
}

} // namespace Anker

template <>
struct fmt::formatter<Anker::MkbInput> : Anker::ToStringFmtFormatter {};
template <>
struct fmt::formatter<Anker::GamepadInput> : Anker::ToStringFmtFormatter {};
