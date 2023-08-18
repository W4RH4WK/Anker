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
// Type Name

template <typename T>
constexpr const char* typeName()
{
	static_assert(AlwaysFalse<T>, "typeName undefined");
}

// clang-format off
template <> constexpr const char* typeName<int8_t>() { return "int8"; }
template <> constexpr const char* typeName<uint8_t>() { return "uint8"; }
template <> constexpr const char* typeName<int16_t>() { return "int16"; }
template <> constexpr const char* typeName<uint16_t>() { return "uint16"; }
template <> constexpr const char* typeName<int32_t>() { return "int32"; }
template <> constexpr const char* typeName<uint32_t>() { return "uint32"; }
template <> constexpr const char* typeName<int64_t>() { return "int64"; }
template <> constexpr const char* typeName<uint64_t>() { return "uint64"; }
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
