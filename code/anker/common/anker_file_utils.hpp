#pragma once

#include "anker_status.hpp"
#include "anker_io_utils.hpp"

namespace Anker {

namespace fs = std::filesystem;

Status readFile(const fs::path&, ByteBuffer&);
Status readFile(const fs::path&, std::string&);

Status writeFile(const fs::path&, std::span<const uint8_t>);
Status writeFile(const fs::path&, std::string_view);

fs::path stripFileExtensions(const fs::path&);
std::string stripFileExtensions(const std::string&);
std::string_view stripFileExtensions(std::string_view);

} // namespace Anker
