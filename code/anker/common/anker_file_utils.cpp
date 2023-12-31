#include <anker/common/anker_file_utils.hpp>

namespace Anker {

template <typename Buffer>
static Status readFileIntoBuffer(Buffer& buffer, const fs::path& filepath)
{
	ANKER_PROFILE_ZONE_T(filepath.string());

	std::ifstream file(filepath, std::ios::binary);
	if (!file) {
		ANKER_ERROR("{}: std::ifstream failed: {}", filepath, //
		            std::system_error(errno, std::generic_category()).what());
		return ReadError;
	}

	file.seekg(0, std::ios_base::end);
	buffer.resize(file.tellg());

	file.seekg(0, std::ios_base::beg);
	file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
	if (!file) {
		ANKER_ERROR("{}: std::ifstream::read failed: {}", filepath, //
		            std::system_error(errno, std::generic_category()).what());
		return ReadError;
	}

	return Ok;
}

Status readFile(ByteBuffer& buffer, const fs::path& filepath)
{
	return readFileIntoBuffer(buffer, filepath);
}

Status readFile(std::string& buffer, const fs::path& filepath)
{
	return readFileIntoBuffer(buffer, filepath);
}

Status writeFile(std::span<const u8> data, const fs::path& filepath)
{
	return writeFile({reinterpret_cast<const char*>(data.data()), data.size()}, filepath);
}

Status writeFile(std::string_view data, const fs::path& filepath)
{
	ANKER_PROFILE_ZONE_T(filepath.string());

	std::ofstream file(filepath, std::ios::binary);
	if (!file) {
		ANKER_ERROR("{}: std::ofstream failed: {}", filepath, //
		            std::system_error(errno, std::generic_category()).what());
		return WriteError;
	}

	file.write(data.data(), data.size());
	if (!file) {
		ANKER_ERROR("{}: std::ofstream::write failed: {}", filepath, //
		            std::system_error(errno, std::generic_category()).what());
		return WriteError;
	}

	return Ok;
}

fs::path stripFileExtensions(const fs::path& filepath)
{
	return fs::path(filepath).replace_extension();
}

std::string stripFileExtensions(const std::string& filepath)
{
	return filepath.substr(0, filepath.find_last_of('.'));
}

std::string_view stripFileExtensions(std::string_view filepath)
{
	return filepath.substr(0, filepath.find_last_of('.'));
}

std::string toIdentifier(fs::path filepath)
{
	ANKER_CHECK(filepath.is_relative(), "");

	filepath = fs::relative(filepath, ""); // normalize
	filepath.replace_extension();          // strip extension

	std::string result = filepath.string();
	std::ranges::replace(result, '\\', '/');
	return result;
}

} // namespace Anker
