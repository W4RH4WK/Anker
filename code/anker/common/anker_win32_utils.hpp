#pragma once

namespace Anker {

inline std::string win32ErrorMessage(DWORD error)
{
	if (error == 0) {
		return "No Error";
	}
	LPSTR buffer = nullptr;
	DWORD formatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
	size_t size = FormatMessageA(formatFlags, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //
	                             (LPSTR)&buffer, 0, NULL);
	auto message = fmt::format("{:#x} {}", error, std::string_view(buffer, size));
	LocalFree(buffer);
	return message;
}

inline std::string win32LastErrorMessage()
{
	return win32ErrorMessage(GetLastError());
}

} // namespace Anker
