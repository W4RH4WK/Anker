#include "anker_asset_cache.hpp"

namespace Anker {

AssetCache::AssetCache(DataLoader& loader, RenderDevice& renderDevice) : m_loader(loader), m_renderDevice(renderDevice)
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
	return makeAssetPtr(m_renderDevice.loadVertexShader(identifier, m_loader, shaderInputs));
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
	return makeAssetPtr(m_renderDevice.loadPixelShader(identifier, m_loader));
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
	if (auto error = Texture::load(identifier, m_loader, m_renderDevice, *texture)) {
		ANKER_ERROR("Could not load texture: {} {}", error, identifier);
	}
	return texture;
}

std::optional<Material> AssetCache::builtinMaterial(std::string_view identifier) const
{
	if (auto it = m_builtinMaterial.find(identifier); it != m_builtinMaterial.end()) {
		return it->second;
	} else {
		return std::nullopt;
	}
}

void AssetCache::addBuiltinMaterial(std::string_view identifier, const Material& material)
{
	m_builtinMaterial[std::string{identifier}] = material;
}

void AssetCache::reloadModifiedAssets()
{
	for (const auto& modifiedAssetFilepath : m_loader.modifiedFiles()) {
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
