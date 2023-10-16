#pragma once

#include "anker_type_utils.hpp"

namespace Anker {

// A resize-able buffer for bytes; elements are stored on the heap. u8 is
// preferred to std::byte for better interoperability with low-level C APIs.
using ByteBuffer = std::vector<u8>;

template <typename Container>
concept Spannable = requires(Container & container)
{
	{std::span(container)};
};

// Since we are using u8 instead of std::byte, we provide asBytes and
// asBytesWritable for u8.
template <typename T, usize N>
auto asBytes(std::span<T, N> s) noexcept
{
	using ReturnType = std::span<const u8, N == std::dynamic_extent ? std::dynamic_extent : sizeof(T) * N>;
	return ReturnType{reinterpret_cast<const u8*>(s.data()), s.size_bytes()};
}

template <typename T, usize N>
auto asBytesWritable(std::span<T, N> s) noexcept requires(!std::is_const_v<T>)
{
	using ReturnType = std::span<u8, N == std::dynamic_extent ? std::dynamic_extent : sizeof(T) * N>;
	return ReturnType{reinterpret_cast<u8*>(s.data()), s.size_bytes()};
}

inline auto asStringView(std::span<const u8> s) noexcept
{
	return std::string_view(reinterpret_cast<const char*>(s.data()), s.size());
}

std::string encodeBase64(std::string_view);
std::string decodeBase64(std::string_view);

} // namespace Anker
