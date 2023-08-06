#include "anker_log.hpp"

namespace Anker::Log {

void log(Severity severity, std::string_view function, long line, std::string_view message)
{
	using namespace std::chrono;

	// Using system_clock here since fmt only supports this type for formatting.
	auto now = system_clock::now();

	auto formatArgs = fmt::make_format_args(now,
	                                        round<milliseconds>(now.time_since_epoch()), //
	                                        toChar(severity),                            //
	                                        function, line,                              //
	                                        message);

	fmt::vprint("{0:%H:%M}:{1:%S} {2:} [{3:}:{4:}] {5:}\n", formatArgs);
}

} // namespace Anker::Log
