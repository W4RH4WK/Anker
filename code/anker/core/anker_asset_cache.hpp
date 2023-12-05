#pragma once

#include <anker/audio/anker_audio_track.hpp>
#include <anker/audio/anker_audio_stream.hpp>
#include <anker/core/anker_asset.hpp>
#include <anker/graphics/anker_font.hpp>
#include <anker/graphics/anker_render_device.hpp>

namespace Anker {

class FontSystem;

// The AssetCache is in charge of loading various different types of assets and
// setting them up so they can be used directly. For example, loading a texture
// will automatically create the necessary GPU resources.
//
// Loaded assets are cached by their identifier. A subsequent load request with
// the same identifier returns a pointer to the previously loaded asset.
// Pointers are reference counted.
//
// Functions with the Uncached suffix will always bypass the cache.
class AssetCache {
  public:
	AssetCache(RenderDevice&, FontSystem&);

	AssetCache(const AssetCache&) = delete;
	AssetCache& operator=(const AssetCache&) = delete;
	AssetCache(AssetCache&&) noexcept = delete;
	AssetCache& operator=(AssetCache&&) noexcept = delete;

	AssetPtr<VertexShader> loadVertexShader(std::string_view identifier, std::span<const D3D11_INPUT_ELEMENT_DESC>);
	AssetPtr<VertexShader> loadVertexShaderUncached(std::string_view identifier,
	                                                std::span<const D3D11_INPUT_ELEMENT_DESC>);

	AssetPtr<PixelShader> loadPixelShader(std::string_view identifier);
	AssetPtr<PixelShader> loadPixelShaderUncached(std::string_view identifier);

	AssetPtr<Texture> loadTexture(std::string_view identifier);
	AssetPtr<Texture> loadTextureUncached(std::string_view identifier);

	AssetPtr<Font> loadFont(std::string_view identifier);
	AssetPtr<Font> loadFontUncached(std::string_view identifier);

	AssetPtr<AudioTrack> loadAudioTrack(std::string_view identifier);
	AssetPtr<AudioTrack> loadAudioTrackUncached(std::string_view identifier);

	AssetPtr<AudioStream> loadAudioStream(std::string_view identifier);
	AssetPtr<AudioStream> loadAudioStreamUncached(std::string_view identifier);

	////////////////////////////////////////////////////////////

	// Reload assets that have been modified according to the underlying
	// DataLoader. Not all assets can be reloaded. This will update the asset
	// pointed to by the respective AssetPtr.
	void reloadModifiedAssets();

	void clearUnused();
	void clearAll();

	////////////////////////////////////////////////////////////

	RenderDevice& renderDevice() { return m_renderDevice; }
	FontSystem& fontSystem() { return m_fontSystem; }

  private:
	RenderDevice& m_renderDevice;
	FontSystem& m_fontSystem;

	template <typename T>
	using Cache = StringMap<AssetPtr<T>>;

	Cache<VertexShader> m_vertexShaderCache;
	Cache<PixelShader> m_pixelShaderCache;
	Cache<Texture> m_textureCache;
	Cache<Font> m_fontCache;
	Cache<AudioTrack> m_audioTrackCache;
	Cache<AudioStream> m_audioStreamCache;
};

} // namespace Anker
