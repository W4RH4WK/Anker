#include "anker_asset_cache.hpp"

#include "anker_data_loader.hpp"

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
	if (not m_renderDevice.loadVertexShader(identifier, shaderInputs, *vertexShader)) {
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
	if (not m_renderDevice.loadPixelShader(identifier, *pixelShader)) {
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
	(void)m_renderDevice.loadTexture(identifier, *texture);
	return texture;
}

void AssetCache::reloadModifiedAssets()
{
	for (const auto& modifiedAssetFilepath : m_dataLoader.modifiedFiles()) {
		auto modifiedAssetIdentifier = stripFileExtensions(modifiedAssetFilepath.string());

		if (auto it = m_vertexShaderCache.find(modifiedAssetIdentifier); it != m_vertexShaderCache.end()) {
			ANKER_INFO("Reloading {}", modifiedAssetIdentifier);
			*it->second = *loadVertexShaderUncached(it->first, it->second->inputLayoutDesc);
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
}

void AssetCache::clearAll()
{
	m_vertexShaderCache.clear();
	m_pixelShaderCache.clear();
	m_textureCache.clear();
}

} // namespace Anker
