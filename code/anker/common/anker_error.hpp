#pragma once

#include "anker_enum_utils.hpp"
#include "anker_string_utils.hpp"

namespace Anker {

// Error handling is typically done by returning an instance of Error. Output
// parameters are used for a function's primary return value instead.
//
// Since Error cannot contain any context specific information, log messages
// should be emitted immediately, before returning the Error.
enum [[nodiscard]] Error : uint8_t {
	Ok = 0,
	UnknownError = 1,
	NotImplementedError,
	ReadError,
	WriteError,
	FormatError,
	GraphicsError,
};
constexpr std::array ErrorEntries{
    std::pair{Ok, "NoError"},
    std::pair{UnknownError, "UnknownError"},
    std::pair{NotImplementedError, "NotImplementedError"},
    std::pair{ReadError, "ReadError"},
    std::pair{WriteError, "WriteError"},
    std::pair{FormatError, "FormatError"},
    std::pair{GraphicsError, "GraphicsError"},
};
ANKER_ENUM_TO_FROM_STRING(Error)

} // namespace Anker

template <>
struct fmt::formatter<Anker::Error> : Anker::ToStringFmtFormatter {};
