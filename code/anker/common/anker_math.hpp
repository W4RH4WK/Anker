#pragma once

#include "anker_conversion.hpp"
#include "anker_type_utils.hpp"

namespace Anker {

using Vec2 = glm::vec2;
constexpr Vec2 Vec2Up = {0, -1};
constexpr Vec2 Vec2Down = {0, 1};
constexpr Vec2 Vec2Left = {-1, 0};
constexpr Vec2 Vec2Right = {1, 0};

using Vec2i = glm::ivec2;
constexpr Vec2i Vec2iUp = {0, -1};
constexpr Vec2i Vec2iDown = {0, 1};
constexpr Vec2i Vec2iLeft = {-1, 0};
constexpr Vec2i Vec2iRight = {1, 0};

using Vec2u = glm::uvec2;

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

template <> inline Vec2 as<Vec2>(const b2Vec2& v) { return std::bit_cast<Vec2>(v); }
template <> inline b2Vec2 as<b2Vec2>(const Vec2& v) { return std::bit_cast<b2Vec2>(v); }
template <> inline Vec4 as<Vec4>(const b2Color& v) { return std::bit_cast<Vec4>(v); }
template <> inline b2Color as<b2Color>(const Vec4& v) { return std::bit_cast<b2Color>(v); }
// clang-format on

} // namespace Anker

template <typename T>
struct fmt::formatter<glm::vec<2, T>> : formatter<T> {
	template <typename FormatContext>
	auto format(const glm::vec<2, T>& vec, FormatContext& ctx)
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
