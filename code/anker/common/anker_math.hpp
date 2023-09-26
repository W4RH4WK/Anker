#pragma once

#include "anker_type_utils.hpp"

namespace Anker {

////////////////////////////////////////////////////////////
// Common Operators

template <typename T>
constexpr T clamp(T value, T min, T max)
{
	if (value < min) {
		return min;
	} else if (value > max) {
		return max;
	} else {
		return value;
	}
}

template <typename T>
constexpr T clamp01(T value)
{
	return clamp(value, T(0), T(1));
}

////////////////////////////////////////////////////////////
// Interpolation

template <typename T>
constexpr T lerp(T t, T start, T end)
{
	return start + (end - start) * t;
}

template <typename T>
constexpr T lerp01(T t)
{
	return lerp(t, T(0), T(1));
}

template <typename T>
constexpr T inverseLerp(T value, T start, T end)
{
	return (value - start) / (end - start);
}

// Interpolates from current to target non-linearly, ease-out.
template <typename T>
constexpr T moveTowards(T current, T target, T speed, float dt, T epsilon = T(1e-6))
{
	if (speed <= T(0)) {
		return target;
	}
	auto distance = target - current;
	if (distance * distance <= epsilon) {
		return target;
	}
	return current + distance * clamp01(speed * dt);
}

////////////////////////////////////////////////////////////
// Angel Conversion

struct Deg2RadTag {};
constexpr Deg2RadTag Deg2Rad;
inline constexpr auto operator*(std::floating_point auto value, Deg2RadTag)
{
	return value * static_cast<decltype(value)>(0.01745329251994329576923690768489);
}
struct Rad2DegTag {};
constexpr Rad2DegTag Rad2Deg;
inline constexpr auto operator*(std::floating_point auto value, Rad2DegTag)
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

	constexpr Vec2T& rotate(T angle) { return *this = glm::rotate(glm::vec<2, T>(*this), angle); }

	constexpr Vec2T& moveTowards(Vec2T target, Vec2T speed, float dt, T epsilon = T(1e-6))
	{
		x = Anker::moveTowards(x, target.x, speed.x, dt, epsilon);
		y = Anker::moveTowards(y, target.y, speed.y, dt, epsilon);
		return *this;
	}

	friend constexpr auto operator<=>(Vec2T, Vec2T) = default;

	T x = 0;
	T y = 0;

	static const Vec2T Up, Down, Left, Right;
	static const Vec2T WorldUp, WorldDown, WorldLeft, WorldRight;
};

// Default coordinate system: -Y is up
template <typename T>
const Vec2T<T> Vec2T<T>::Up{0, -1};
template <typename T>
const Vec2T<T> Vec2T<T>::Down{0, 1};
template <typename T>
const Vec2T<T> Vec2T<T>::Left{-1, 0};
template <typename T>
const Vec2T<T> Vec2T<T>::Right{1, 0};

// World coordinate system: +Y is up
template <typename T>
const Vec2T<T> Vec2T<T>::WorldUp{0, 1};
template <typename T>
const Vec2T<T> Vec2T<T>::WorldDown{0, -1};
template <typename T>
const Vec2T<T> Vec2T<T>::WorldLeft{-1, 0};
template <typename T>
const Vec2T<T> Vec2T<T>::WorldRight{1, 0};

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

	constexpr Vec3T& moveTowards(Vec3T target, Vec3T speed, float dt, T epsilon = T(1e-6))
	{
		x = Anker::moveTowards(x, target.x, speed.x, dt, epsilon);
		y = Anker::moveTowards(y, target.y, speed.y, dt, epsilon);
		z = Anker::moveTowards(z, target.z, speed.z, dt, epsilon);
		return *this;
	}

	friend constexpr auto operator<=>(Vec3T, Vec3T) = default;

	T x = 0;
	T y = 0;
	T z = 0;
};

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

	constexpr Vec4T& moveTowards(Vec4T target, Vec4T speed, float dt, T epsilon = T(1e-6))
	{
		x = Anker::moveTowards(x, target.x, speed.x, dt, epsilon);
		y = Anker::moveTowards(y, target.y, speed.y, dt, epsilon);
		z = Anker::moveTowards(z, target.z, speed.z, dt, epsilon);
		w = Anker::moveTowards(w, target.w, speed.w, dt, epsilon);
		return *this;
	}

	friend constexpr auto operator<=>(Vec4T, Vec4T) = default;

	T x = 0;
	T y = 0;
	T z = 0;
	T w = 0;
};

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
	Rect2T() = default;
	explicit Rect2T(Vec2T<T> size) : size(size) {}
	explicit Rect2T(Vec2T<T> size, Vec2T<T> offset) : size(size), offset(offset) {}

	static constexpr Rect2T fromPoints(Vec2T<T> a, Vec2T<T> b)
	{
		T width = a.x < b.x ? b.x - a.x : a.x - b.x;
		T height = a.y < b.y ? b.y - a.y : a.y - b.y;

		Vec2T<T> offset = {std::min(a.x, b.x), std::min(a.y, b.y)};

		return Rect2T({width, height}, offset);
	}

	explicit operator Vec4T<T>() const { return {size.x, size.y, offset.x, offset.y}; }

	constexpr Vec2T<T> center() const { return {offset.x + size.x / 2, offset.y + size.y / 2}; }

	// Default coordinate system: -Y is up
	constexpr Vec2T<T> topLeft() const { return {offset.x, offset.y}; }
	constexpr Vec2T<T> topRight() const { return {offset.x + size.x, offset.y}; }
	constexpr Vec2T<T> bottomLeft() const { return {offset.x, offset.y + size.y}; }
	constexpr Vec2T<T> bottomRight() const { return {offset.x + size.x, offset.y + size.y}; }

	// World coordinate system: +Y is up
	constexpr Vec2T<T> topLeftWorld() const { return {offset.x, offset.y + size.y}; }
	constexpr Vec2T<T> topRightWorld() const { return {offset.x + size.x, offset.y + size.y}; }
	constexpr Vec2T<T> bottomLeftWorld() const { return {offset.x, offset.y}; }
	constexpr Vec2T<T> bottomRightWorld() const { return {offset.x + size.x, offset.y}; }

	Vec2T<T> size;
	Vec2T<T> offset;
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

template <typename T>
struct fmt::formatter<Anker::Vec4T<T>> : formatter<T> {
	template <typename FormatContext>
	auto format(const Anker::Vec4T<T>& vec, FormatContext& ctx)
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
		out = fmt::format_to(out, ", ");
		ctx.advance_to(out);
		out = formatter<T>::format(vec.w, ctx);
		*out = ')';
		return out;
	}
};
