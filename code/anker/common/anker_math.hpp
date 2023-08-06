#pragma once

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

using Mat4 = glm::mat4;
constexpr Mat4 Mat4Id = glm::identity<Mat4>();

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
