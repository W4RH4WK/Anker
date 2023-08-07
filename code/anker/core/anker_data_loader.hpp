#pragma once

namespace Anker {

class IDataLoaderSource;

// The DataLoader is a generic loader for binary data that can come from various
// different sources. Most commonly the file-system or a compressed archive.
//
// At least one IDataLoaderSource must be added before the loader can be used.
//
// Sources are queried in the order they have been registered. If the first
// source does not contain the requested asset, the second one is consulted, and
// so on.
class DataLoader {
  public:
	DataLoader() = default;
	DataLoader(const DataLoader&) = delete;
	DataLoader& operator=(const DataLoader&) = delete;
	DataLoader(DataLoader&&) noexcept = delete;
	DataLoader& operator=(DataLoader&&) noexcept = delete;

	Status load(const fs::path&, ByteBuffer& outBuffer) const;
	bool exists(const fs::path&) const;

	void addSource(IDataLoaderSource*);
	void removeSource(IDataLoaderSource*);
	void clearSources();

	void tick(float dt);

	// Provides a set of identifiers that have been modified since the last
	// tick.
	const std::unordered_set<fs::path>& modifiedFiles() const { return m_modifiedFiles; }

  private:
	std::vector<IDataLoaderSource*> m_sources;

	std::unordered_set<fs::path> m_modifiedFiles;
	float m_modifiedFilesTimer = 0;
};

class IDataLoaderSource {
  public:
	virtual Status load(const fs::path&, ByteBuffer&) const = 0;
	virtual bool exists(const fs::path&) const = 0;

	// Inserts the file paths of files that have been modified since the
	// previous call to modifiedFiles. Implementation is optional. This is
	// primarily used to enable hot-reloading for certain assets.
	virtual void modifiedFiles(std::insert_iterator<std::unordered_set<fs::path>>) {}

	~IDataLoaderSource() noexcept {}
};

} // namespace Anker
