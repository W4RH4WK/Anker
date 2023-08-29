#pragma once

#include "anker_math.hpp"

namespace Anker {

inline b2Vec2 toB2Vec(const Vec2& v) noexcept
{
	return std::bit_cast<b2Vec2>(v);
}

inline b2Color toB2Vec(const Vec4& v) noexcept
{
	return std::bit_cast<b2Color>(v);
}

constexpr Vec2 toVec(const b2Vec2& v) noexcept
{
	return std::bit_cast<Vec2>(v);
}

constexpr Vec4 toVec(const b2Color& v) noexcept
{
	return std::bit_cast<Vec4>(v);
}

} // namespace Anker
