#pragma once

namespace Anker {

// A resize-able buffer for bytes; elements are stored on the heap. uint8_t is
// preferred to std::byte for better interoperability with low-level C APIs.
using ByteBuffer = std::vector<uint8_t>;

// Since we are using uint8_t instead of std::byte, we provide asBytes and
// asBytesWritable for uint8_t.
template <class T, size_t N>
auto asBytes(std::span<T, N> s) noexcept
{
	using ReturnType = std::span<const uint8_t, N == std::dynamic_extent ? std::dynamic_extent : sizeof(T) * N>;
	return ReturnType{reinterpret_cast<const uint8_t*>(s.data()), s.size_bytes()};
}

template <class T, size_t N>
auto asBytesWritable(std::span<T, N> s) noexcept requires(!std::is_const_v<T>)
{
	using ReturnType = std::span<uint8_t, N == std::dynamic_extent ? std::dynamic_extent : sizeof(T) * N>;
	return ReturnType{reinterpret_cast<uint8_t*>(s.data()), s.size_bytes()};
}

} // namespace Anker
