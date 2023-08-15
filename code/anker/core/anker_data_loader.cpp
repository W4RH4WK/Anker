#include <anker/core/anker_data_loader.hpp>

namespace Anker {

Status DataLoader::load(const fs::path& filepath, ByteBuffer& outBuffer) const
{
	for (auto& source : m_sources) {
		if (source->load(filepath, outBuffer)) {
			return OK;
		}
	}
	ANKER_WARN("{}: Missing!", filepath);
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
	ANKER_CHECK(source);
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

void DataLoader::tick(float dt)
{
	m_modifiedFiles.clear();

	// Query for modifications, but only every second, not every frame.
	if (m_modifiedFilesTimer -= dt; m_modifiedFilesTimer <= 0) {
		m_modifiedFilesTimer = 1;
		auto inserter = std::inserter(m_modifiedFiles, m_modifiedFiles.end());
		for (auto& source : m_sources) {
			source->modifiedFiles(inserter);
		}
	}
}

} // namespace Anker
