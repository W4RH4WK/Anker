#pragma once

#include <anker/graphics/anker_render_device.hpp>

#include <anker/core/anker_asset.hpp>

namespace Anker {

class DataLoader;

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
	AssetCache(DataLoader&, RenderDevice&);

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

	////////////////////////////////////////////////////////////

	// Reload assets that have been modified according to the underlying
	// DataLoader. Not all assets can be reloaded. This will update the asset
	// pointed to by the respective AssetPtr.
	void reloadModifiedAssets();

	void clearUnused();
	void clearAll();

	////////////////////////////////////////////////////////////

	DataLoader& dataLoader() { return m_dataLoader; }
	RenderDevice& renderDevice() { return m_renderDevice; }

  private:
	DataLoader& m_dataLoader;
	RenderDevice& m_renderDevice;

	template <typename T>
	using Cache = StringMap<AssetPtr<T>>;

	Cache<VertexShader> m_vertexShaderCache;
	Cache<PixelShader> m_pixelShaderCache;
	Cache<Texture> m_textureCache;
};

} // namespace Anker
