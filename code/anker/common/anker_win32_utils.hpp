#pragma once

#include <anker/common/anker_type_utils.hpp>

namespace Anker {

inline std::string win32ErrorMessage(u32 error)
{
#if ANKER_PLATFORM_WINDOWS
	if (error == 0) {
		return toString(Ok);
	}

	const DWORD formatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER //
	                        | FORMAT_MESSAGE_FROM_SYSTEM     //
	                        | FORMAT_MESSAGE_IGNORE_INSERTS;

	LPSTR buffer = nullptr;
	usize size = FormatMessageA(formatFlags, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //
	                            (LPSTR)&buffer, 0, NULL);

	std::string message = fmt::format("{:#x} {}", error, std::string_view(buffer, size));
	LocalFree(buffer);
	return message;
#elif ANKER_PLATFORM_LINUX
	return fmt::format("{:#x}", error);
#endif
}

} // namespace Anker
