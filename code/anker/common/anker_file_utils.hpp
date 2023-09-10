#pragma once

#include "anker_io_utils.hpp"
#include "anker_status.hpp"

namespace Anker {

namespace fs = std::filesystem;

Status readFile(const fs::path&, ByteBuffer&);
Status readFile(const fs::path&, std::string&);

Status writeFile(const fs::path&, std::span<const uint8_t>);
Status writeFile(const fs::path&, std::string_view);

fs::path stripFileExtensions(const fs::path&);
std::string stripFileExtensions(const std::string&);
std::string_view stripFileExtensions(std::string_view);

// Asset identifiers typically use normalized, relative paths with forward
// slashes. The file extension is stripped to increase portability between
// platforms.
std::string toIdentifier(fs::path);

} // namespace Anker
