#pragma once

namespace Anker {

class IDataLoaderSource {
  public:
	virtual Status load(ByteBuffer&, const fs::path&) const = 0;
	virtual bool exists(const fs::path&) const = 0;

	// Inserts the file paths of files that have been modified since the
	// previous call to modifiedFiles. Implementation is optional. This is
	// primarily used to enable hot-reloading for certain assets.
	virtual void modifiedFiles(std::insert_iterator<std::unordered_set<fs::path>>) {}

	~IDataLoaderSource() noexcept {}
};

// The DataLoader is a generic loader for binary data that can come from various
// different sources -- most commonly the file system or a compressed archive.
//
// At least one IDataLoaderSource must be added before the loader can be used.
//
// Sources are queried in the order they have been registered. If the first
// source does not contain the requested data, the second one is consulted, and
// so on.
class DataLoader {
  public:
	DataLoader() = default;
	DataLoader(const DataLoader&) = delete;
	DataLoader& operator=(const DataLoader&) = delete;
	DataLoader(DataLoader&&) noexcept = delete;
	DataLoader& operator=(DataLoader&&) noexcept = delete;

	Status load(ByteBuffer& outBuffer, const fs::path&) const;
	bool exists(const fs::path&) const;

	void addSource(IDataLoaderSource*);
	void removeSource(IDataLoaderSource*);
	void clearSources();

	void tick();

	// Provides a set of identifiers that have been modified. The set is cleared
	// and populated on tick.
	const std::unordered_set<fs::path>& modifiedFiles() const { return m_modifiedFiles; }

  private:
	std::vector<IDataLoaderSource*> m_sources;

	std::unordered_set<fs::path> m_modifiedFiles;

	Clock::time_point m_lastModifiedFilesCheck{};
};

// Global loader for loading asset data.
inline DataLoader g_assetDataLoader;

} // namespace Anker
