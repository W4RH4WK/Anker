#pragma once

#include <anker/common/anker_enum_utils.hpp>
#include <anker/common/anker_string_utils.hpp>
#include <anker/common/anker_type_utils.hpp>

// Error handling in C++ is a bit of a pain point. There are multiple approaches
// to this. In general, we stick to the old return status + output parameter
// paradigm.
//
// It's recommended to still initialize the output parameter with something
// sensible, like a fallback asset for example.

namespace Anker {

// StatusCode defines the types of errors that can occur, including the Ok to
// indicate the absence of an error.
enum StatusCode : u8 {
	Ok = 0,
	UnknownError,
	NotImplementedError,
	InvalidArgumentError,
	ReadError,
	WriteError,
	EnumError,
	FormatError,
	GraphicsError,
	FontError,
};

template <>
inline constexpr std::array EnumEntries<StatusCode> = {
    std::pair{Ok, "Ok"_hs},
    std::pair{UnknownError, "UnknownError"_hs},
    std::pair{NotImplementedError, "NotImplementedError"_hs},
    std::pair{InvalidArgumentError, "InvalidArgumentError"_hs},
    std::pair{ReadError, "ReadError"_hs},
    std::pair{WriteError, "WriteError"_hs},
    std::pair{EnumError, "EnumError"_hs},
    std::pair{FormatError, "FormatError"_hs},
    std::pair{GraphicsError, "GraphicsError"_hs},
    std::pair{FontError, "FontError"_hs},
};

// Status is used as a thin wrapper around StatusCode. The primary goal here is
// to invert boolean conversion. Status converts to true on success, and false
// on failure.
//
// Since Status cannot contain any context specific information, log messages
// should be emitted immediately before returning an error. This also removes
// the need for emitting log messages when passing on a caught error.
//
// Example for explicit error handling:
//
//     if (Status status = doSomething(); !status) {
//         handleError(status);
//     }
//
// Example for basic error checking:
//
//     if (not doSomething()) {
//         handleError();
//     }
//
// Here we prefer the `not` keyword over `!` to highlight that we are checking
// for Status failure.
//
// Example for suppressing no-discard warnings:
//
//     std::ignore = doSomething();
//
struct [[nodiscard]] Status {
	constexpr Status() = default;
	constexpr Status(StatusCode code) : code(code) {}

	constexpr explicit operator bool() const { return code == Ok; }

	std::string toString() const { return Anker::toString(code); }
	static bool fromString(Status& status, std::string_view view) { return Anker::fromString(status.code, view); }

	StatusCode code = Ok;

	friend constexpr bool operator==(const Status&, const Status&) = default;
};

// A common operation is to just bail and pass on a caught error. This macro can
// be used as a shorthand. Remember, log messages should be emitted immediately
// upon emitting an error, thus we don't have to emit log messages when passing
// errors on.
//
// Note that, while its main purpose is to be used with Status, it can also be
// used for other types like bool and std::optional, given the return type
// matches.
#define ANKER_TRY(expr) \
	do { \
		if (auto status = (expr); !status) [[unlikely]] { \
			return status; \
		} \
	} while (0)

} // namespace Anker

template <>
struct fmt::formatter<Anker::StatusCode> : Anker::ToStringFmtFormatter {};
template <>
struct fmt::formatter<Anker::Status> : Anker::ToStringFmtFormatter {};
