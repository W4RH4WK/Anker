#include "anker_file_utils.hpp"

namespace Anker {

template <typename Buffer>
static Error readFileIntoBuffer(const fs::path& filepath, Buffer& outData)
{
	ANKER_PROFILE_ZONE_T(filepath.string());

	std::ifstream file(filepath, std::ios::binary);
	if (!file) {
		return ReadError;
	}

	file.seekg(0, std::ios_base::end);
	outData.resize(file.tellg());

	file.seekg(0, std::ios_base::beg);
	file.read(reinterpret_cast<char*>(outData.data()), outData.size());
	if (!file) {
		return ReadError;
	}

	return Ok;
}

Error readFile(const fs::path& filepath, ByteBuffer& outData)
{
	return readFileIntoBuffer(filepath, outData);
}

Error readFile(const fs::path& filepath, std::string& outData)
{
	return readFileIntoBuffer(filepath, outData);
}

Error writeFile(const fs::path& filepath, std::span<const uint8_t> data)
{
	return writeFile(filepath, {reinterpret_cast<const char*>(data.data()), data.size()});
}

Error writeFile(const fs::path& filepath, std::string_view data)
{
	ANKER_PROFILE_ZONE_T(filepath.string());

	std::ofstream file(filepath, std::ios::binary);
	if (!file) {
		return WriteError;
	}

	file.write(data.data(), data.size());
	if (!file) {
		return WriteError;
	}

	return Ok;
}

fs::path stripFileExtensions(const fs::path& filepath)
{
	return stripFileExtensions(filepath.string());
}

std::string stripFileExtensions(const std::string& filepath)
{
	return filepath.substr(0, filepath.find_last_of('.'));
}

std::string_view stripFileExtensions(std::string_view filepath)
{
	return filepath.substr(0, filepath.find_last_of('.'));
}

} // namespace Anker
