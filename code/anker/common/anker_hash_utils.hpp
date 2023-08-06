#pragma once

namespace Anker {

struct StringHash {
	using is_transparent = std::true_type;
	auto operator()(std::string_view s) const noexcept { return std::hash<std::string_view>()(s); }
};

// HashMap where the key is an std::string. Lookup can be done with an
// std::string_view.
template <typename T>
using StringMap = std::unordered_map<std::string, T, StringHash, std::equal_to<>>;

} // namespace Anker
