#pragma once

namespace Anker {

////////////////////////////////////////////////////////////
// Enum reflection utilities

// A convenience alias for the entries array. We use std::span to erase the size
// template parameter.
template <typename Enum>
using EnumEntries = std::span<const std::pair<Enum, const char*>>;

// Enum attribute for refl-cpp that stores the entries array (as std::span).
template <typename Enum>
struct EnumAttr : refl::attr::usage::field {
	explicit constexpr EnumAttr(EnumEntries<Enum> e) : entries(e) {}
	EnumEntries<Enum> entries;
};
template <typename Enum, size_t N>
EnumAttr(std::array<std::pair<Enum, const char*>, N>) -> EnumAttr<Enum>;

////////////////////////////////////////////////////////////
// Enums as flags

// This macro defines the wrapper alias along with the necessary operators to
// combine single flags.
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

// Flags wrapper for enums. This allows type-safe enums to be used like bit
// flags. For simplicity, only | and & bit-operators are supported.
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
#define ANKER_ENUM_TO_FROM_STRING(Enum) \
	inline const std::unordered_map<Enum, const char*> Enum##ToStringLookup{Enum##Entries.begin(), \
	                                                                        Enum##Entries.end()}; \
	inline const char* to_string(Enum e) \
	{ \
		if (auto it = Enum##ToStringLookup.find(e); it != Enum##ToStringLookup.end()) { \
			return it->second; \
		} else { \
			return "invalid"; \
		} \
	} \
	inline const auto Enum##FromStringLookup = [] { \
		std::unordered_map<std::string_view, Enum> lookup; \
		for (const auto& entry : Enum##Entries) { \
			lookup[entry.second] = entry.first; \
		} \
		return lookup; \
	}(); \
	inline void from_string_impl(std::string_view input, std::optional<Enum>& result) \
	{ \
		if (auto it = Enum##FromStringLookup.find(input); it != Enum##FromStringLookup.end()) { \
			result = it->second; \
		} else { \
			result.reset(); \
		} \
	}

} // namespace Anker