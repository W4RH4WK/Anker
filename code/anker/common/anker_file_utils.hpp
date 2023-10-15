#pragma once

#include "anker_io_utils.hpp"
#include "anker_status.hpp"

namespace Anker {

namespace fs = std::filesystem;

Status readFile(ByteBuffer&, const fs::path&);
Status readFile(std::string&, const fs::path&);

Status writeFile(std::span<const uint8_t>, const fs::path&);
Status writeFile(std::string_view, const fs::path&);

fs::path stripFileExtensions(const fs::path&);
std::string stripFileExtensions(const std::string&);
std::string_view stripFileExtensions(std::string_view);

// Asset identifiers typically use normalized, relative paths with forward
// slashes. The file extension is stripped to increase portability between
// platforms.
std::string toIdentifier(fs::path);

} // namespace Anker
