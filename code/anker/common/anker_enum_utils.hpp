#pragma once

#include "anker_type_utils.hpp"

namespace Anker {

////////////////////////////////////////////////////////////
// Enum reflection utilities

// A convenience alias for the entries array. We use std::span to erase the size
// template parameter.
template <typename Enum>
using EnumEntries = std::span<const std::pair<Enum, entt::hashed_string>>;

////////////////////////////////////////////////////////////
// Flag Enum

// This macro defines a type that can be used as bit-flags, by wrapping an
// existing enum. The original enum (e.g. GpuBindFlag, note singular) defines
// the flags, while the wrapper type (e.g. GpuBindFlags, note plural) defines
// the necessary operators to combine single flags.
//
// See invocations of this macro across the code-base for examples.
#define ANKER_ENUM_FLAGS(FlagType) \
	using FlagType##s = ::Anker::Flags<FlagType>; \
	inline constexpr ::Anker::Flags<FlagType> operator|(FlagType a, FlagType b) \
	{ \
		return ::Anker::Flags<FlagType>(a) | b; \
	} \
	inline constexpr ::Anker::Flags<FlagType> operator&(FlagType a, FlagType b) \
	{ \
		return ::Anker::Flags<FlagType>(a) & b; \
	}

// Flags wrapper for enums. This allows type-safe enums to be used like
// bit-flags.
template <typename Enum>
class Flags {
  public:
	using MaskType = std::underlying_type_t<Enum>;

	constexpr Flags() = default;
	constexpr Flags(Enum bit) : m_mask(static_cast<MaskType>(bit)) {}
	constexpr explicit Flags(MaskType flags) : m_mask(flags) {}

	constexpr Flags(const Flags<Enum>&) = default;
	constexpr Flags<Enum>& operator=(const Flags<Enum>&) = default;

	constexpr explicit operator MaskType() const { return m_mask; }

	constexpr explicit operator bool() const { return m_mask; }
	constexpr bool operator!() const { return !m_mask; }

	constexpr Flags<Enum> operator~() const { return Flags<Enum>(~m_mask); }

	constexpr Flags<Enum> operator|(Flags<Enum> other) const { return Flags<Enum>(m_mask | other.m_mask); }
	constexpr Flags<Enum> operator&(Flags<Enum> other) const { return Flags<Enum>(m_mask & other.m_mask); }

	constexpr Flags<Enum>& operator|=(Flags<Enum> other)
	{
		m_mask |= other.m_mask;
		return *this;
	}

	constexpr Flags<Enum>& operator&=(Flags<Enum> other)
	{
		m_mask &= other.m_mask;
		return *this;
	}

	friend auto operator<=>(const Flags<Enum>&, const Flags<Enum>&) = default;

  private:
	MaskType m_mask = 0;

	static_assert(MaskType(Enum::None) == 0, "Flag enum requires a None entry");
};

template <typename Enum>
constexpr Flags<Enum> operator|(Enum flag, const Flags<Enum>& flags)
{
	return flags.operator|(flag);
}

template <typename Enum>
constexpr Flags<Enum> operator&(Enum flag, const Flags<Enum>& flags)
{
	return flags.operator&(flag);
}

////////////////////////////////////////////////////////////
// Enum to_string/from_string generator

// Generates the to_string/from_string utility functions for the given enum.
// This macro expects an entries array to be defined.
//
// See invocations of this macro across the code-base for examples.
#define ANKER_ENUM_TO_FROM_STRING(Enum) \
	inline const char* to_string(Enum e) \
	{ \
		static const std::unordered_map<Enum, const char*> lookup{Enum##Entries.begin(), Enum##Entries.end()}; \
		if (auto it = lookup.find(e); it != lookup.end()) { \
			return it->second; \
		} else { \
			return "invalid"; \
		} \
	} \
	inline bool from_string(Enum& result, std::string_view input) \
	{ \
		entt::hashed_string input_hs(input.data(), input.size()); \
		auto it = std::ranges::find_if(Enum##Entries, [&](auto& pair) { return pair.second == input_hs; }); \
		if (it != Enum##Entries.end()) { \
			result = it->first; \
			return true; \
		} else { \
			return false; \
		} \
	}

} // namespace Anker
