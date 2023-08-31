#pragma once

#include "anker_conversion.hpp"
#include "anker_type_utils.hpp"

namespace Anker {

// Degree converts floats and doubles between degrees and radians. Use * to
// convert to radians (read as "value in degrees"). Use / to convert to degrees
// (read as "value to degrees"). All angles are typically stored in radians.
// Degrees are commonly used for UI and when hard-coding values.
struct DegreesTag {};
constexpr DegreesTag Degrees;
inline constexpr auto operator*(std::floating_point auto value, DegreesTag)
{
	return value * static_cast<decltype(value)>(0.01745329251994329576923690768489);
}
inline constexpr auto operator/(std::floating_point auto value, DegreesTag)
{
	return value * static_cast<decltype(value)>(57.295779513082320876798154814105);
}

////////////////////////////////////////////////////////////
// Vector 2D

template <typename T>
struct Vec2T {
	constexpr Vec2T() = default;
	constexpr Vec2T(T v) : x(v), y(v) {}
	constexpr Vec2T(T x, T y) : x(x), y(y) {}

	constexpr Vec2T(const Vec2T&) = default;
	constexpr Vec2T& operator=(const Vec2T&) = default;
	constexpr Vec2T(Vec2T&&) noexcept = default;
	constexpr Vec2T& operator=(Vec2T&&) noexcept = default;

	constexpr Vec2T(glm::vec<2, T> v) : x(v.x), y(v.y) {}
	constexpr operator glm::vec<2, T>() const { return {x, y}; }

	constexpr Vec2T(b2Vec2 v) requires(std::is_same_v<T, float>) : x(v.x), y(v.y) {}
	constexpr operator b2Vec2() const requires(std::is_same_v<T, float>) { return {x, y}; }

	template <typename TT>
	explicit constexpr operator Vec2T<TT>() const
	{
		return {TT(x), TT(y)};
	}

	constexpr double length() const { return glm::length(glm::vec<2, T>(*this)); }
	constexpr double lengthSquared() const { return glm::length2(glm::vec<2, T>(*this)); }

	constexpr double ratio() const { return double(x) / double(y); }

	constexpr void rotate(T angle) { *this = glm::rotate(glm::vec<2, T>(*this), angle); }

	friend constexpr auto operator<=>(Vec2T, Vec2T) = default;

	T x = 0;
	T y = 0;

	static const Vec2T Zero, One;
	static const Vec2T Up, Down, Left, Right;
	static const Vec2T UiUp, UiDown, UiLeft, UiRight;
};

template <typename T>
const Vec2T<T> Vec2T<T>::Zero{0, 0};
template <typename T>
const Vec2T<T> Vec2T<T>::One{1, 1};

template <typename T>
const Vec2T<T> Vec2T<T>::Up{0, 1};
template <typename T>
const Vec2T<T> Vec2T<T>::Down{0, -1};
template <typename T>
const Vec2T<T> Vec2T<T>::Left{-1, 0};
template <typename T>
const Vec2T<T> Vec2T<T>::Right{1, 0};

template <typename T>
const Vec2T<T> Vec2T<T>::UiUp{0, -1};
template <typename T>
const Vec2T<T> Vec2T<T>::UiDown{0, 1};
template <typename T>
const Vec2T<T> Vec2T<T>::UiLeft{-1, 0};
template <typename T>
const Vec2T<T> Vec2T<T>::UiRight{1, 0};

template <typename T>
constexpr Vec2T<T>& operator+=(Vec2T<T>& a, Vec2T<T> b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}

template <typename T>
constexpr Vec2T<T>& operator-=(Vec2T<T>& a, Vec2T<T> b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

template <typename T, typename C>
constexpr Vec2T<T>& operator*=(Vec2T<T>& v, C c)
{
	v.x *= c;
	v.y *= c;
	return v;
}

template <typename T>
constexpr Vec2T<T>& operator*=(Vec2T<T>& a, Vec2T<T> b)
{
	a.x *= b.x;
	a.y *= b.y;
	return a;
}

template <typename T, typename C>
constexpr Vec2T<T>& operator/=(Vec2T<T>& v, C c)
{
	v.x /= c;
	v.y /= c;
	return v;
}

template <typename T>
constexpr Vec2T<T>& operator/=(Vec2T<T>& a, Vec2T<T> b)
{
	a.x /= b.x;
	a.y /= b.y;
	return a;
}

// clang-format off
template <typename T>
constexpr Vec2T<T> operator-(Vec2T<T> v) { return {-v.x, -v.y}; }
template <typename T>
constexpr Vec2T<T> operator+(Vec2T<T> a, Vec2T<T> b) { return a += b; }
template <typename T>
constexpr Vec2T<T> operator-(Vec2T<T> a, Vec2T<T> b) { return a -= b; }
template <typename T, typename C>
constexpr Vec2T<T> operator*(C c, Vec2T<T> v) { return v *= c; }
template <typename T, typename C>
constexpr Vec2T<T> operator*(Vec2T<T> v, C c) { return v *= c; }
template <typename T>
constexpr Vec2T<T> operator*(Vec2T<T> a, Vec2T<T> b) { return a *= b; }
template <typename T, typename C>
constexpr Vec2T<T> operator/(Vec2T<T> v, C c) { return v /= c; }
template <typename T, typename C>
constexpr Vec2T<T> operator/(C c, Vec2T<T> v) { return {c / v.x, c / v.y}; }
template <typename T>
constexpr Vec2T<T> operator/(Vec2T<T> a, Vec2T<T> b) { return a /= b; }
// clang-format on

using Vec2 = Vec2T<float>;
using Vec2i = Vec2T<int>;
using Vec2u = Vec2T<unsigned>;
using Vec2d = Vec2T<double>;

////////////////////////////////////////////////////////////

using Vec3 = glm::vec3;
constexpr Vec3 Vec3Up = {0, 1, 0};
constexpr Vec3 Vec3Down = {0, -1, 0};
constexpr Vec3 Vec3Left = {-1, 0, 0};
constexpr Vec3 Vec3Right = {1, 0, 0};
constexpr Vec3 Vec3Forward = {0, 0, -1};
constexpr Vec3 Vec3Backward = {0, 0, 1};

using Vec4 = glm::vec4;

using Quat = glm::quat;
constexpr Quat QuatId = glm::identity<Quat>();

using Mat2 = glm::mat2;
constexpr Mat2 Mat2Id = glm::identity<Mat2>();

using Mat3 = glm::mat3;
constexpr Mat3 Mat3Id = glm::identity<Mat3>();

using Mat4 = glm::mat4;
constexpr Mat4 Mat4Id = glm::identity<Mat4>();

inline Vec3 gammaReverse(const Vec3& color, float gamma = 2.2f)
{
	return pow(color, Vec3(gamma));
}

inline Vec4 gammaReverse(const Vec4& color, float gamma = 2.2f)
{
	return pow(color, Vec4(gamma, gamma, gamma, 1));
}

// clang-format off
template <> constexpr const char* typeName<Vec2>() { return "Vec2"; }
template <> constexpr const char* typeName<Vec2i>() { return "Vec2i"; }
template <> constexpr const char* typeName<Vec2u>() { return "Vec2u"; }
template <> constexpr const char* typeName<Vec3>() { return "Vec3"; }
template <> constexpr const char* typeName<Vec4>() { return "Vec4"; }
template <> constexpr const char* typeName<Quat>() { return "Quat"; }
template <> constexpr const char* typeName<Mat2>() { return "Mat2"; }
template <> constexpr const char* typeName<Mat3>() { return "Mat3"; }
template <> constexpr const char* typeName<Mat4>() { return "Mat4"; }

template <> inline Vec4 as<Vec4>(const b2Color& v) { return std::bit_cast<Vec4>(v); }
template <> inline b2Color as<b2Color>(const Vec4& v) { return std::bit_cast<b2Color>(v); }
// clang-format on

} // namespace Anker

template <typename T>
struct fmt::formatter<Anker::Vec2T<T>> : formatter<T> {
	template <typename FormatContext>
	auto format(const Anker::Vec2T<T>& vec, FormatContext& ctx)
	{
		auto out = ctx.out();
		*out = '(';
		ctx.advance_to(out);
		out = formatter<T>::format(vec.x, ctx);
		out = fmt::format_to(out, ", ");
		ctx.advance_to(out);
		out = formatter<T>::format(vec.y, ctx);
		*out = ')';
		return out;
	}
};

template <typename T>
struct fmt::formatter<glm::vec<3, T>> : formatter<T> {
	template <typename FormatContext>
	auto format(const glm::vec<3, T>& vec, FormatContext& ctx)
	{
		auto out = ctx.out();
		*out = '(';
		ctx.advance_to(out);
		out = formatter<T>::format(vec.x, ctx);
		out = fmt::format_to(out, ", ");
		ctx.advance_to(out);
		out = formatter<T>::format(vec.y, ctx);
		out = fmt::format_to(out, ", ");
		ctx.advance_to(out);
		out = formatter<T>::format(vec.z, ctx);
		*out = ')';
		return out;
	}
};
