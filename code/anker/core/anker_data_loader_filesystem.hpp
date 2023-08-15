#pragma once

#include <anker/core/anker_data_loader.hpp>

namespace Anker {

class DataLoaderFilesystem : public IDataLoaderSource {
  public:
	explicit DataLoaderFilesystem(const fs::path& root);
	~DataLoaderFilesystem();

	DataLoaderFilesystem(const DataLoaderFilesystem&) = delete;
	DataLoaderFilesystem& operator=(const DataLoaderFilesystem&) = delete;
	DataLoaderFilesystem(DataLoaderFilesystem&&) noexcept = delete;
	DataLoaderFilesystem& operator=(DataLoaderFilesystem&&) noexcept = delete;

	Status load(const fs::path&, ByteBuffer& outBuffer) const override;
	bool exists(const fs::path&) const override;
	void modifiedFiles(std::insert_iterator<std::unordered_set<fs::path>>) override;

  private:
	fs::path m_root;

	// We record the modification timestamps of loaded files the check for
	// changes.
	mutable std::unordered_map<fs::path, fs::file_time_type> m_lastWriteTimestamps;
};

} // namespace Anker
