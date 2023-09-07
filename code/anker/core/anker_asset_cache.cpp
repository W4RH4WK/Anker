#include <anker/core/anker_asset_cache.hpp>

#include <anker/core/anker_data_loader.hpp>

namespace Anker {

AssetCache::AssetCache(DataLoader& dataLoader, RenderDevice& renderDevice)
    : m_dataLoader(dataLoader), m_renderDevice(renderDevice)
{}

AssetPtr<VertexShader> AssetCache::loadVertexShader(std::string_view identifier,
                                                    std::span<const D3D11_INPUT_ELEMENT_DESC> shaderInputs)
{
	if (auto it = m_vertexShaderCache.find(identifier); it != m_vertexShaderCache.end()) {
		return it->second;
	}
	return m_vertexShaderCache[std::string{identifier}] = loadVertexShaderUncached(identifier, shaderInputs);
}

AssetPtr<VertexShader> AssetCache::loadVertexShaderUncached(std::string_view identifier,
                                                            std::span<const D3D11_INPUT_ELEMENT_DESC> shaderInputs)
{
	auto vertexShader = makeAssetPtr<VertexShader>();
	vertexShader->info.inputs.assign(shaderInputs.begin(), shaderInputs.end());
	if (not m_renderDevice.loadVertexShader(*vertexShader, identifier)) {
		return nullptr;
	}
	return vertexShader;
}

AssetPtr<PixelShader> AssetCache::loadPixelShader(std::string_view identifier)
{
	if (auto it = m_pixelShaderCache.find(identifier); it != m_pixelShaderCache.end()) {
		return it->second;
	}
	return m_pixelShaderCache[std::string{identifier}] = loadPixelShaderUncached(identifier);
}

AssetPtr<PixelShader> AssetCache::loadPixelShaderUncached(std::string_view identifier)
{
	auto pixelShader = makeAssetPtr<PixelShader>();
	if (not m_renderDevice.loadPixelShader(*pixelShader, identifier)) {
		return nullptr;
	}
	return pixelShader;
}

AssetPtr<Texture> AssetCache::loadTexture(std::string_view identifier)
{
	if (auto it = m_textureCache.find(identifier); it != m_textureCache.end()) {
		return it->second;
	}
	return m_textureCache[std::string{identifier}] = loadTextureUncached(identifier);
}

AssetPtr<Texture> AssetCache::loadTextureUncached(std::string_view identifier)
{
	auto texture = makeAssetPtr<Texture>();

	// Ignoring status here since a fallback texture is used on failure.
	(void)m_renderDevice.loadTexture(*texture, identifier);

	return texture;
}

AssetPtr<Font> AssetCache::loadFont(std::string_view identifier)
{
	if (auto it = m_fontCache.find(identifier); it != m_fontCache.end()) {
		return it->second;
	}
	return m_fontCache[std::string{identifier}] = loadFontUncached(identifier);
}

AssetPtr<Font> AssetCache::loadFontUncached(std::string_view identifier)
{
	ByteBuffer fontData;
	if (not m_dataLoader.load(std::string{identifier} + ".ttf", fontData)) {
		return nullptr;
	}

	auto font = makeAssetPtr<Font>();
	font->name = identifier;

	if (not Font::load(*font, fontData, m_renderDevice)) {
		return nullptr;
	}

	return font;
}

void AssetCache::reloadModifiedAssets()
{
	for (const auto& modifiedAssetFilepath : m_dataLoader.modifiedFiles()) {
		auto modifiedAssetIdentifier = stripFileExtensions(modifiedAssetFilepath.string());

		if (auto it = m_vertexShaderCache.find(modifiedAssetIdentifier); it != m_vertexShaderCache.end()) {
			ANKER_INFO("Reloading {}", modifiedAssetIdentifier);
			*it->second = *loadVertexShaderUncached(it->first, it->second->info.inputs);
			continue;
		}
		if (auto it = m_pixelShaderCache.find(modifiedAssetIdentifier); it != m_pixelShaderCache.end()) {
			ANKER_INFO("Reloading {}", modifiedAssetIdentifier);
			*it->second = *loadPixelShaderUncached(it->first);
			continue;
		}
		if (auto it = m_textureCache.find(modifiedAssetIdentifier); it != m_textureCache.end()) {
			ANKER_INFO("Reloading {}", modifiedAssetIdentifier);
			*it->second = *loadTextureUncached(it->first);
			continue;
		}
	}
}

void AssetCache::clearUnused()
{
	auto isUnused = [](auto& pair) { return pair.second.use_count() == 1; };
	std::erase_if(m_vertexShaderCache, isUnused);
	std::erase_if(m_pixelShaderCache, isUnused);
	std::erase_if(m_textureCache, isUnused);
	std::erase_if(m_fontCache, isUnused);
}

void AssetCache::clearAll()
{
	m_vertexShaderCache.clear();
	m_pixelShaderCache.clear();
	m_textureCache.clear();
	m_fontCache.clear();
}

} // namespace Anker
