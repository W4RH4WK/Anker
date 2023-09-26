#pragma once

namespace Anker {

////////////////////////////////////////////////////////////
// toString

template <typename T>
concept HasToStringMemFn = requires(T v)
{
	{v.toString()};
};

template <typename T>
std::string toString(const T& v)
{
	if constexpr (HasToStringMemFn<T>) {
		return v.toString();
	} else {
		// using ADL as fallback
		using std::to_string;
		return std::string{to_string(v)};
	}
}

template <typename T>
concept ToStringable = requires(T v)
{
	{toString(v)};
};

// This is an opt-in formatter, analogue to fmt's ostream_formatter, that
// utilizes the toString utility for formatting.
struct ToStringFmtFormatter : fmt::formatter<fmt::string_view> {
	template <typename T, typename FormatContext>
	auto format(const T& object, FormatContext& ctx)
	{
		return fmt::formatter<fmt::string_view>::format(toString(object), ctx);
	}
};

////////////////////////////////////////////////////////////
// fromString

template <typename T>
concept HasFromStringMemFn = requires(T)
{
	{T::fromString(std::string_view{})};
};

template <typename T>
std::optional<T> fromString(std::string_view input)
{
	if constexpr (HasFromStringMemFn<T>) {
		return T::fromString(input);
	} else {
		std::optional<T> result;
		from_string_impl(input, result);
		return result;
	}
}

template <typename T>
concept FromStringable = requires(T)
{
	{fromString<T>(std::string_view{})};
};

////////////////////////////////////////////////////////////
// ASCII string utilities

namespace Ascii {

inline bool isAscii(std::string_view s)
{
	return std::all_of(s.begin(), s.end(), [](unsigned char c) { return c <= 127; });
}

inline char toLowerChar(char c)
{
	if ('A' <= c && c <= 'Z') {
		return c + ('a' - 'A');
	} else {
		return c;
	}
}

inline char toUpperChar(char c)
{
	if ('a' <= c && c <= 'z') {
		return c - ('a' - 'A');
	} else {
		return c;
	}
}

inline void toLower(std::string& s)
{
	std::transform(s.begin(), s.end(), s.begin(), toLowerChar);
}

inline void toUpper(std::string& s)
{
	std::transform(s.begin(), s.end(), s.begin(), toUpperChar);
}

} // namespace Ascii
} // namespace Anker
