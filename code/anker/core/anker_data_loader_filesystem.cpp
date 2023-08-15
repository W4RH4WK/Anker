#include <anker/core/anker_data_loader_filesystem.hpp>

namespace Anker {

DataLoaderFilesystem::DataLoaderFilesystem(const fs::path& root) : m_root(root)
{
	if (!fs::exists(root)) {
		ANKER_WARN("Source does not exist: {}", root);
		return;
	}
}

DataLoaderFilesystem::~DataLoaderFilesystem() {}

Status DataLoaderFilesystem::load(const fs::path& filepath, ByteBuffer& outBuffer) const
{
	ANKER_TRY(readFile(m_root / filepath, outBuffer));

	// Record modification time for comparison.
	std::error_code lastWriteTimeError;
	auto lastWrite = fs::last_write_time(m_root / filepath, lastWriteTimeError);
	if (!lastWriteTimeError) {
		m_lastWriteTimestamps[filepath] = lastWrite;
	}

	return OK;
}

bool DataLoaderFilesystem::exists(const fs::path& filepath) const
{
	return fs::exists(m_root / filepath);
}

void DataLoaderFilesystem::modifiedFiles(std::insert_iterator<std::unordered_set<fs::path>> inserter)
{
	for (const auto& [filepath, timestamp] : m_lastWriteTimestamps) {
		std::error_code err;
		if (auto writeTime = fs::last_write_time(m_root / filepath, err); !err && writeTime > timestamp) {
			inserter = filepath;
		}
	}
}

} // namespace Anker
