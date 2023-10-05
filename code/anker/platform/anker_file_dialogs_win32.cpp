#include <anker/platform/anker_file_dialogs.hpp>

#include <commdlg.h>

#include <anker/platform/anker_platform.hpp>

namespace Anker {

std::optional<fs::path> openFileDialog()
{
	char filepath[MAX_PATH] = "";

	OPENFILENAME params{
	    .lStructSize = sizeof(OPENFILENAME),
	    .hwndOwner = Platform::nativeWindow(),
	    .lpstrFilter = "All Files (*.*)\0*.*\0",
	    .lpstrFile = filepath,
	    .nMaxFile = sizeof(filepath),
	    .Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR,
	};

	if (GetOpenFileName(&params)) {
		return filepath;
	} else {
		return std::nullopt;
	}
}

std::optional<fs::path> saveFileDialog(const char* extension)
{
	char filepath[MAX_PATH] = "";

	OPENFILENAME params{
	    .lStructSize = sizeof(OPENFILENAME),
	    .hwndOwner = Platform::nativeWindow(),
	    .lpstrFilter = "All Files (*.*)\0*.*\0",
	    .lpstrFile = filepath,
	    .nMaxFile = sizeof(filepath),
	    .Flags = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR,
	    .lpstrDefExt = extension,
	};

	if (GetSaveFileName(&params)) {
		return filepath;
	} else {
		return std::nullopt;
	}
}

} // namespace Anker
