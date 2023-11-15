#pragma once

#include <anker/common/anker_type_utils.hpp>

namespace Anker {

////////////////////////////////////////////////////////////
// Enum reflection utilities

// The EnumEntries template is to be specialized with the corresponding enum
// type for string serialization and reflection.
template <typename Enum>
constexpr std::array<std::pair<Enum, entt::hashed_string>, 0> EnumEntries;

template <typename Enum>
constexpr bool HasEnumEntries = !EnumEntries<Enum>.empty();

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

} // namespace Anker
