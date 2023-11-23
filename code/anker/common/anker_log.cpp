#include <anker/common/anker_log.hpp>

namespace Anker::Log {

static std::mutex g_stdoutLock;

void log(Severity severity, std::string_view function, long line, std::string_view message)
{
	using namespace std::chrono;

	// Using system_clock here since fmt only supports this type for formatting.
	auto now = system_clock::now();

	std::lock_guard lock(g_stdoutLock);
	fmt::print("{0:%R}:{1:%S} {2:} [{3:}:{4:}] {5:}\n",          //
	           now, round<milliseconds>(now.time_since_epoch()), //
	           toChar(severity),                                 //
	           function, line,                                   //
	           message);

	if (severity >= Severity::Warning) {
		std::fflush(stdout);
	}
}

} // namespace Anker::Log

namespace Anker {

// Box2D Log Hook
void b2Log(const char* format, va_list args)
{
	std::array<char, 1024> buffer;
	vsnprintf(buffer.data(), buffer.size(), format, args);
	ANKER_INFO("Box2d: {}", buffer.data());
}

} // namespace Anker
