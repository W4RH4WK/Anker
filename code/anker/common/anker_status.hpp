#pragma once

#include "anker_enum_utils.hpp"
#include "anker_string_utils.hpp"

namespace Anker {

// Error handling in C++ is a bit of a pain point. There are multiple approaches
// to this. In general, we stick to the old return status + output parameter
// paradigm.
//
// It's recommended to still initialize the output parameter with something
// sensible, like a fallback asset for example.
//
// ErrorCode defines the types of errors that can occur, including the OK to
// indicate the absence of an error.
enum ErrorCode : uint8_t {
	OK = 0,
	UnknownError = 1,
	NotImplementedError,
	ReadError,
	WriteError,
	FormatError,
	GraphicsError,
};
constexpr std::array ErrorCodeEntries{
    std::pair{OK, "OK"},
    std::pair{UnknownError, "UnknownError"},
    std::pair{NotImplementedError, "NotImplementedError"},
    std::pair{ReadError, "ReadError"},
    std::pair{WriteError, "WriteError"},
    std::pair{FormatError, "FormatError"},
    std::pair{GraphicsError, "GraphicsError"},
};
ANKER_ENUM_TO_FROM_STRING(ErrorCode)

// Status is used as a thin wrapper around ErrorCode. The primary goal here is
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
// Example for explicit error handling without using the error:
//
//     if (not doSomething()) {
//         handleError();
//     }
//
// Here, we prefer the `not` keyword to highlight that we are checking for
// failure -- easier to spot than using `!`. Alternatively, you can use `!= OK`.
struct [[nodiscard]] Status {
	constexpr Status() = default;
	constexpr Status(ErrorCode code) : code(code) {}

	constexpr explicit operator bool() const { return code == OK; }

	const char* toString() const { return to_string(code); }
	static std::optional<Status> fromString(std::string_view view) { return Anker::fromString<ErrorCode>(view); }

	ErrorCode code = OK;

	friend constexpr bool operator==(const Status&, const Status&) = default;
};

// A common operation is to just bail and pass on a caught error. This macro can
// be used as a shorthand. Remember, log messages should be emitted immediately
// upon "creating" an error, thus we don't have to emit log messages when
// passing errors on.
#define ANKER_TRY(expr) \
	do { \
		if (::Anker::Status status = (expr); !status) { \
			return status; \
		} \
	} while (0)

} // namespace Anker

template <>
struct fmt::formatter<Anker::ErrorCode> : Anker::ToStringFmtFormatter {};
template <>
struct fmt::formatter<Anker::Status> : Anker::ToStringFmtFormatter {};
