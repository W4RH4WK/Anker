#include <anker/core/anker_asset_cache.hpp>

#include <anker/core/anker_data_loader.hpp>
#include <anker/graphics/anker_font_system.hpp>

namespace Anker {

AssetCache::AssetCache(RenderDevice& renderDevice, FontSystem& fontSystem)
    : m_renderDevice(renderDevice), m_fontSystem(fontSystem)
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
	(void)m_renderDevice.loadVertexShader(*vertexShader, identifier);
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
	(void)m_renderDevice.loadPixelShader(*pixelShader, identifier);
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

	if (not m_renderDevice.loadTexture(*texture, identifier)) {
		ANKER_ERROR("{}: Missing, using fallback!", identifier);
		*texture = m_renderDevice.fallbackTexture();
	}

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
	auto font = makeAssetPtr<Font>();

	if (not m_fontSystem.loadFont(*font, identifier)) {
		ANKER_WARN("{}: Missing, using fallback!", identifier);
		*font = m_fontSystem.systemFont();
	}

	return font;
}

AssetPtr<AudioTrack> AssetCache::loadAudioTrack(std::string_view identifier)
{
	if (auto it = m_audioTrackCache.find(identifier); it != m_audioTrackCache.end()) {
		return it->second;
	}
	return m_audioTrackCache[std::string{identifier}] = loadAudioTrackUncached(identifier);
}

AssetPtr<AudioTrack> AssetCache::loadAudioTrackUncached(std::string_view identifier)
{
	auto track = makeAssetPtr<AudioTrack>();
	(void)track->load(identifier);
	return track;
}

AssetPtr<AudioStream> AssetCache::loadAudioStream(std::string_view identifier)
{
	if (auto it = m_audioStreamCache.find(identifier); it != m_audioStreamCache.end()) {
		return it->second;
	}
	return m_audioStreamCache[std::string{identifier}] = loadAudioStreamUncached(identifier);
}

AssetPtr<AudioStream> AssetCache::loadAudioStreamUncached(std::string_view identifier)
{
	auto track = makeAssetPtr<AudioStream>();
	(void)track->load(identifier);
	return track;
}

void AssetCache::reloadModifiedAssets()
{
	for (const auto& modifiedAssetFilepath : g_assetDataLoader.modifiedFiles()) {
		auto modifiedAssetIdentifier = toIdentifier(modifiedAssetFilepath);

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
	std::erase_if(m_audioTrackCache, isUnused);
	std::erase_if(m_audioStreamCache, isUnused);
}

void AssetCache::clearAll()
{
	m_vertexShaderCache.clear();
	m_pixelShaderCache.clear();
	m_textureCache.clear();
	m_fontCache.clear();
	m_audioTrackCache.clear();
	m_audioStreamCache.clear();
}

} // namespace Anker
