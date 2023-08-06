#pragma once

#include "anker_error.hpp"
#include "anker_io_utils.hpp"

namespace Anker {

namespace fs = std::filesystem;

Error readFile(const fs::path&, ByteBuffer&);
Error readFile(const fs::path&, std::string&);

Error writeFile(const fs::path&, std::span<const uint8_t>);
Error writeFile(const fs::path&, std::string_view);

fs::path stripFileExtensions(const fs::path&);
std::string stripFileExtensions(const std::string&);
std::string_view stripFileExtensions(std::string_view);

} // namespace Anker
