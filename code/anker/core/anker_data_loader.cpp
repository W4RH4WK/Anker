#include <anker/core/anker_data_loader.hpp>

namespace Anker {

Status DataLoader::load(ByteBuffer& outBuffer, const fs::path& filepath) const
{
	for (auto& source : m_sources) {
		if (source->exists(filepath)) {
			return source->load(outBuffer, filepath);
		}
	}
	ANKER_ERROR("{}: Missing!", filepath);
	return ReadError;
}

bool DataLoader::exists(const fs::path& filepath) const
{
	for (auto& source : m_sources) {
		if (source->exists(filepath)) {
			return true;
		}
	}
	return false;
}

void DataLoader::addSource(IDataLoaderSource* source)
{
	ANKER_ASSERT(source);
	m_sources.emplace_back(source);
}

void DataLoader::removeSource(IDataLoaderSource* source)
{
	std::erase(m_sources, source);
}

void DataLoader::clearSources()
{
	m_sources.clear();
}

void DataLoader::tick()
{
	using namespace std::chrono_literals;

	m_modifiedFiles.clear();

	// Query for modifications, but only every second, not every frame.
	if (auto now = Clock::now(); now - m_lastModifiedFilesCheck > 1s) {
		m_lastModifiedFilesCheck = now;

		auto inserter = std::inserter(m_modifiedFiles, m_modifiedFiles.end());
		for (auto& source : m_sources) {
			source->modifiedFiles(inserter);
		}
	}
}

} // namespace Anker
