#pragma once

namespace Anker {

std::optional<fs::path> openFileDialog();
std::optional<fs::path> saveFileDialog(const char* extensions = nullptr);

} // namespace Anker
