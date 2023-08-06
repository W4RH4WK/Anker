#pragma once

#include "anker_macros.hpp"

#define ANKER_LOG_MESSAGE(severity, ...) ::Anker::Log::log(severity, __FUNCTION__, __LINE__, ::fmt::format(__VA_ARGS__))

#define ANKER_INFO(...) ANKER_LOG_MESSAGE(::Anker::Log::Severity::Info, __VA_ARGS__)
#define ANKER_WARN(...) ANKER_LOG_MESSAGE(::Anker::Log::Severity::Warning, __VA_ARGS__)
#define ANKER_ERROR(...) ANKER_LOG_MESSAGE(::Anker::Log::Severity::Error, __VA_ARGS__)
#define ANKER_FATAL(...) \
	do { \
		ANKER_LOG_MESSAGE(::Anker::Log::Severity::Fatal, __VA_ARGS__); \
		ANKER_DEBUG_BREAK(); \
	} while (0)

namespace Anker::Log {

enum class Severity {
	Trace = 0,
	Info = 1,
	Warning = 2,
	Error = 3,
	Fatal = 4,
};

inline char toChar(Severity severity)
{
	switch (severity) {
	case Severity::Trace: return 'T';
	case Severity::Info: return 'I';
	case Severity::Warning: return 'W';
	case Severity::Error: return 'E';
	case Severity::Fatal: return 'F';
	}
	return '?';
}

void log(Severity, std::string_view function, long line, std::string_view message);

} // namespace Anker::Log
