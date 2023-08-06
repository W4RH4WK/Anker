#pragma once

#include <anker/graphics/anker_material.hpp>
#include <anker/graphics/anker_render_device.hpp>

#include "anker_data_loader.hpp"

namespace Anker {

// The AssetCache is in charge of loading various different types of assets and
// setting them up so they can be used directly. For example, loading a model
// will automatically create the necessary GPU buffers and textures for the
// model to be drawn by a renderer.
//
// Loaded assets are cached by their identifier. A subsequent load request with
// the same identifier returns a pointer to the previously loaded asset
// instance. Pointers are reference counted.
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

	std::optional<Material> builtinMaterial(std::string_view identifier) const;
	void addBuiltinMaterial(std::string_view identifier, const Material&);

	// Reload assets that have been modified according to the underlying
	// DataLoader. Not all assets can be reloaded. This will update the object
	// pointed to by the respective AssetPtr.
	void reloadModifiedAssets();

	void clearUnused();
	void clearAll();

	////////////////////////////////////////////////////////////

	DataLoader& dataLoader() { return m_loader; }
	RenderDevice& renderDevice() { return m_renderDevice; }

  private:
	DataLoader& m_loader;
	RenderDevice& m_renderDevice;

	template <typename T>
	using Cache = StringMap<AssetPtr<T>>;

	Cache<VertexShader> m_vertexShaderCache;
	Cache<PixelShader> m_pixelShaderCache;
	Cache<Texture> m_textureCache;

	StringMap<Material> m_builtinMaterial;
};

} // namespace Anker
