#pragma once

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
constexpr Vec2T<T> rotate(Vec2T<T> v, T angle)
{
	return glm::rotate(glm::vec<2, T>(v), angle);
}

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
// Vector 3D

template <typename T>
struct Vec3T {
	constexpr Vec3T() = default;
	constexpr Vec3T(T v) : x(v), y(v), z(v) {}
	constexpr Vec3T(T x, T y, T z) : x(x), y(y), z(z) {}

	constexpr Vec3T(const Vec3T&) = default;
	constexpr Vec3T& operator=(const Vec3T&) = default;
	constexpr Vec3T(Vec3T&&) noexcept = default;
	constexpr Vec3T& operator=(Vec3T&&) noexcept = default;

	constexpr Vec3T(glm::vec<3, T> v) : x(v.x), y(v.y), z(v.z) {}
	constexpr operator glm::vec<3, T>() const { return {x, y, z}; }

	constexpr Vec3T(b2Vec3 v) requires(std::is_same_v<T, float>) : x(v.x), y(v.y), z(v.z) {}
	constexpr operator b2Vec3() const requires(std::is_same_v<T, float>) { return {x, y, z}; }

	template <typename TT>
	explicit constexpr operator Vec3T<TT>() const
	{
		return {TT(x), TT(y), TT(z)};
	}

	constexpr double length() const { return glm::length(glm::vec<3, T>(*this)); }
	constexpr double lengthSquared() const { return glm::length2(glm::vec<3, T>(*this)); }

	friend constexpr auto operator<=>(Vec3T, Vec3T) = default;

	T x = 0;
	T y = 0;
	T z = 0;

	static const Vec3T Zero, One;
};

template <typename T>
const Vec3T<T> Vec3T<T>::Zero{0, 0, 0};
template <typename T>
const Vec3T<T> Vec3T<T>::One{1, 1, 1};

template <typename T>
constexpr Vec3T<T> pow(const Vec3T<T>& v, T exp)
{
	return glm::pow(glm::vec<3, T>(v), glm::vec<3, T>(exp));
}

template <typename T>
constexpr Vec3T<T> pow(const Vec3T<T>& v, const Vec3T<T>& exp)
{
	return glm::pow(glm::vec<3, T>(v), glm::vec<3, T>(exp));
}

template <typename T>
constexpr Vec3T<T>& operator+=(Vec3T<T>& a, Vec3T<T> b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

template <typename T>
constexpr Vec3T<T>& operator-=(Vec3T<T>& a, Vec3T<T> b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

template <typename T, typename C>
constexpr Vec3T<T>& operator*=(Vec3T<T>& v, C c)
{
	v.x *= c;
	v.y *= c;
	v.z *= c;
	return v;
}

template <typename T>
constexpr Vec3T<T>& operator*=(Vec3T<T>& a, Vec3T<T> b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	return a;
}

template <typename T, typename C>
constexpr Vec3T<T>& operator/=(Vec3T<T>& v, C c)
{
	v.x /= c;
	v.y /= c;
	v.z /= c;
	return v;
}

template <typename T>
constexpr Vec3T<T>& operator/=(Vec3T<T>& a, Vec3T<T> b)
{
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;
	return a;
}

// clang-format off
template <typename T>
constexpr Vec3T<T> operator-(Vec3T<T> v) { return {-v.x, -v.y, -v.z}; }
template <typename T>
constexpr Vec3T<T> operator+(Vec3T<T> a, Vec3T<T> b) { return a += b; }
template <typename T>
constexpr Vec3T<T> operator-(Vec3T<T> a, Vec3T<T> b) { return a -= b; }
template <typename T, typename C>
constexpr Vec3T<T> operator*(C c, Vec3T<T> v) { return v *= c; }
template <typename T, typename C>
constexpr Vec3T<T> operator*(Vec3T<T> v, C c) { return v *= c; }
template <typename T>
constexpr Vec3T<T> operator*(Vec3T<T> a, Vec3T<T> b) { return a *= b; }
template <typename T, typename C>
constexpr Vec3T<T> operator/(Vec3T<T> v, C c) { return v /= c; }
template <typename T, typename C>
constexpr Vec3T<T> operator/(C c, Vec3T<T> v) { return {c / v.x, c / v.y, c / v.z}; }
template <typename T>
constexpr Vec3T<T> operator/(Vec3T<T> a, Vec3T<T> b) { return a /= b; }
// clang-format on

using Vec3 = Vec3T<float>;
using Vec3i = Vec3T<int>;
using Vec3u = Vec3T<unsigned>;
using Vec3d = Vec3T<double>;

////////////////////////////////////////////////////////////
// Vector 4D

template <typename T>
struct Vec4T {
	constexpr Vec4T() = default;
	constexpr Vec4T(T v) : x(v), y(v), z(v), w(v) {}
	constexpr Vec4T(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

	constexpr Vec4T(const Vec4T&) = default;
	constexpr Vec4T& operator=(const Vec4T&) = default;
	constexpr Vec4T(Vec4T&&) noexcept = default;
	constexpr Vec4T& operator=(Vec4T&&) noexcept = default;

	constexpr Vec4T(glm::vec<4, T> v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
	constexpr operator glm::vec<4, T>() const { return {x, y, z, w}; }

	constexpr Vec4T(b2Color v) requires(std::is_same_v<T, float>) : x(v.r), y(v.g), z(v.b), w(v.a) {}
	constexpr operator b2Color() const requires(std::is_same_v<T, float>) { return {x, y, z, w}; }

	template <typename TT>
	explicit constexpr operator Vec4T<TT>() const
	{
		return {TT(x), TT(y), TT(z), TT(w)};
	}

	constexpr double length() const { return glm::length(glm::vec<4, T>(*this)); }
	constexpr double lengthSquared() const { return glm::length2(glm::vec<4, T>(*this)); }

	friend constexpr auto operator<=>(Vec4T, Vec4T) = default;

	T x = 0;
	T y = 0;
	T z = 0;
	T w = 0;

	static const Vec4T Zero, One;
};

template <typename T>
const Vec4T<T> Vec4T<T>::Zero{0, 0, 0, 0};
template <typename T>
const Vec4T<T> Vec4T<T>::One{1, 1, 1, 1};

template <typename T>
constexpr Vec4T<T> pow(const Vec4T<T>& v, T exp)
{
	return glm::pow(glm::vec<4, T>(v), glm::vec<4, T>(exp));
}

template <typename T>
constexpr Vec4T<T> pow(const Vec4T<T>& v, const Vec4T<T>& exp)
{
	return glm::pow(glm::vec<4, T>(v), glm::vec<4, T>(exp));
}

template <typename T>
constexpr Vec4T<T>& operator+=(Vec4T<T>& a, Vec4T<T> b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;
	return a;
}

template <typename T>
constexpr Vec4T<T>& operator-=(Vec4T<T>& a, Vec4T<T> b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;
	return a;
}

template <typename T, typename C>
constexpr Vec4T<T>& operator*=(Vec4T<T>& v, C c)
{
	v.x *= c;
	v.y *= c;
	v.z *= c;
	v.w *= c;
	return v;
}

template <typename T>
constexpr Vec4T<T>& operator*=(Vec4T<T>& a, Vec4T<T> b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	a.w *= b.w;
	return a;
}

template <typename T, typename C>
constexpr Vec4T<T>& operator/=(Vec4T<T>& v, C c)
{
	v.x /= c;
	v.y /= c;
	v.z /= c;
	v.w /= c;
	return v;
}

template <typename T>
constexpr Vec4T<T>& operator/=(Vec4T<T>& a, Vec4T<T> b)
{
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;
	a.w /= b.w;
	return a;
}

// clang-format off
template <typename T>
constexpr Vec4T<T> operator-(Vec4T<T> v) { return {-v.x, -v.y, -v.z, -v.w}; }
template <typename T>
constexpr Vec4T<T> operator+(Vec4T<T> a, Vec4T<T> b) { return a += b; }
template <typename T>
constexpr Vec4T<T> operator-(Vec4T<T> a, Vec4T<T> b) { return a -= b; }
template <typename T, typename C>
constexpr Vec4T<T> operator*(C c, Vec4T<T> v) { return v *= c; }
template <typename T, typename C>
constexpr Vec4T<T> operator*(Vec4T<T> v, C c) { return v *= c; }
template <typename T>
constexpr Vec4T<T> operator*(Vec4T<T> a, Vec4T<T> b) { return a *= b; }
template <typename T, typename C>
constexpr Vec4T<T> operator/(Vec4T<T> v, C c) { return v /= c; }
template <typename T, typename C>
constexpr Vec4T<T> operator/(C c, Vec4T<T> v) { return {c / v.x, c / v.y, c / v.z, c / v.w}; }
template <typename T>
constexpr Vec4T<T> operator/(Vec4T<T> a, Vec4T<T> b) { return a /= b; }
// clang-format on

using Vec4 = Vec4T<float>;
using Vec4i = Vec4T<int>;
using Vec4u = Vec4T<unsigned>;
using Vec4d = Vec4T<double>;

////////////////////////////////////////////////////////////
// Rect 2D

template <typename T>
struct Rect2T {
	static constexpr Rect2T fromPoints(Vec2T<T> a, Vec2T<T> b)
	{
		T width = a.x < b.x ? b.x - a.x : a.x - b.x;
		T height = a.y < b.y ? b.y - a.y : a.y - b.y;

		Vec2T<T> offset = {std::min(a.x, b.x), std::min(a.y, b.y)};

		return {{width, height}, offset};
	}

	Vec2T<T> size = Vec2T<T>::Zero;
	Vec2T<T> offset = Vec2T<T>::Zero;
};

using Rect2 = Rect2T<float>;
using Rect2i = Rect2T<int>;
using Rect2u = Rect2T<unsigned>;
using Rect2d = Rect2T<double>;

////////////////////////////////////////////////////////////
// Other Common Math Types

using Quat = glm::quat;
constexpr Quat QuatId = glm::identity<Quat>();

using Mat2 = glm::mat2;
constexpr Mat2 Mat2Id = glm::identity<Mat2>();

using Mat3 = glm::mat3;
constexpr Mat3 Mat3Id = glm::identity<Mat3>();

using Mat4 = glm::mat4;
constexpr Mat4 Mat4Id = glm::identity<Mat4>();

////////////////////////////////////////////////////////////
// Color Handling

inline Vec3 gammaReverse(Vec3 color, float gamma = 2.2f)
{
	return pow(color, gamma);
}

inline Vec4 gammaReverse(const Vec4& color, float gamma = 2.2f)
{
	return pow(color, {gamma, gamma, gamma, 1});
}

////////////////////////////////////////////////////////////
// Meta

// clang-format off
template <> constexpr const char* typeName<Vec2>() { return "Vec2"; }
template <> constexpr const char* typeName<Vec2i>() { return "Vec2i"; }
template <> constexpr const char* typeName<Vec2u>() { return "Vec2u"; }
template <> constexpr const char* typeName<Vec2d>() { return "Vec2d"; }
template <> constexpr const char* typeName<Vec3>() { return "Vec3"; }
template <> constexpr const char* typeName<Vec3i>() { return "Vec3i"; }
template <> constexpr const char* typeName<Vec3u>() { return "Vec3u"; }
template <> constexpr const char* typeName<Vec3d>() { return "Vec3d"; }
template <> constexpr const char* typeName<Vec4>() { return "Vec4"; }
template <> constexpr const char* typeName<Vec4i>() { return "Vec4i"; }
template <> constexpr const char* typeName<Vec4u>() { return "Vec4u"; }
template <> constexpr const char* typeName<Vec4d>() { return "Vec4d"; }
template <> constexpr const char* typeName<Quat>() { return "Quat"; }
template <> constexpr const char* typeName<Mat2>() { return "Mat2"; }
template <> constexpr const char* typeName<Mat3>() { return "Mat3"; }
template <> constexpr const char* typeName<Mat4>() { return "Mat4"; }
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
struct fmt::formatter<Anker::Vec3T<T>> : formatter<T> {
	template <typename FormatContext>
	auto format(const Anker::Vec3T<T>& vec, FormatContext& ctx)
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
