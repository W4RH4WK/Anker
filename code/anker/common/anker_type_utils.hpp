#pragma once

namespace Anker {

template <typename...>
constexpr bool AlwaysFalse = false;

template <typename...>
constexpr bool AlwaysTrue = true;

template <class... Ts>
struct overload : Ts... {
	using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

////////////////////////////////////////////////////////////
// Type Iteration

template <typename... Ts, typename F>
constexpr void forEachType(F f)
{
	std::apply([&](auto&&... ts) { (f(ts), ...); }, std::tuple<Ts...>());
}

////////////////////////////////////////////////////////////
// Integer Types

using i8 = std::int8_t;
using u8 = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;
using usize = std::size_t;

////////////////////////////////////////////////////////////
// Type Name

template <typename T>
constexpr const char* typeName()
{
	static_assert(AlwaysFalse<T>, "typeName undefined");
}

// clang-format off
template <> constexpr const char* typeName<i8>() { return "i8"; }
template <> constexpr const char* typeName<u8>() { return "u8"; }
template <> constexpr const char* typeName<i16>() { return "i16"; }
template <> constexpr const char* typeName<u16>() { return "u16"; }
template <> constexpr const char* typeName<i32>() { return "i32"; }
template <> constexpr const char* typeName<u32>() { return "u32"; }
template <> constexpr const char* typeName<i64>() { return "i64"; }
template <> constexpr const char* typeName<u64>() { return "u64"; }
template <> constexpr const char* typeName<float>() { return "float"; }
template <> constexpr const char* typeName<double>() { return "double"; }
template <> constexpr const char* typeName<std::monostate>() { return "std::monostate"; }
// clang-format on

template <typename T>
constexpr const char* typeName() requires(refl::is_reflectable<T>())
{
	return refl::reflect<T>().name.c_str();
}

} // namespace Anker
