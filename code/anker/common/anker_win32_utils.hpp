#pragma once

namespace Anker {

inline std::string win32ErrorMessage(uint32_t error)
{
#if ANKER_PLATFORM_WINDOWS
	if (error == 0) {
		return to_string(Ok);
	}

	const DWORD formatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER //
	                        | FORMAT_MESSAGE_FROM_SYSTEM     //
	                        | FORMAT_MESSAGE_IGNORE_INSERTS;

	LPSTR buffer = nullptr;
	size_t size = FormatMessageA(formatFlags, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //
	                             (LPSTR)&buffer, 0, NULL);

	std::string message = fmt::format("{:#x} {}", error, std::string_view(buffer, size));
	LocalFree(buffer);
	return message;
#elif ANKER_PLATFORM_LINUX
	return fmt::format("{:#x}", error);
#endif
}

} // namespace Anker
