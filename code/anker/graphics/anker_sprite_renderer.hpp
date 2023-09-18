#pragma once

#include <anker/core/anker_asset.hpp>
#include <anker/graphics/anker_render_device.hpp>
#include <anker/graphics/anker_render_layers.hpp>

namespace Anker {

class AssetCache;
class Scene;

class SpriteRenderer {
  public:
	SpriteRenderer(RenderDevice&, AssetCache&);
	SpriteRenderer(const SpriteRenderer&) = delete;
	SpriteRenderer& operator=(const SpriteRenderer&) = delete;
	SpriteRenderer(SpriteRenderer&&) noexcept = delete;
	SpriteRenderer& operator=(SpriteRenderer&&) noexcept = delete;

	void collectRenderLayers(const Scene&, std::insert_iterator<std::set<RenderLayer>>);

	void draw(const Scene&, RenderLayer);

  private:
	RenderDevice& m_renderDevice;

	AssetPtr<VertexShader> m_vertexShader;
	AssetPtr<PixelShader> m_pixelShader;
	GpuBuffer m_constantBuffer;
	GpuBuffer m_vertexBuffer;
};

} // namespace Anker
