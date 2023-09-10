#pragma once

#include <anker/graphics/anker_render_device.hpp>
#include <anker/graphics/anker_render_layers.hpp>

namespace Anker {

class AssetCache;
class Scene;

class MapRenderer {
  public:
	MapRenderer(RenderDevice&, AssetCache&);
	MapRenderer(const MapRenderer&) = delete;
	MapRenderer& operator=(const MapRenderer&) = delete;
	MapRenderer(MapRenderer&&) noexcept = delete;
	MapRenderer& operator=(MapRenderer&&) noexcept = delete;

	void collectRenderLayers(const Scene&, std::insert_iterator<std::set<RenderLayer>>);

	void draw(const Scene&, RenderLayer);

	struct Vertex {
		Vec2 position;
		Vec2 uv;
	};

  private:
	RenderDevice& m_renderDevice;

	GpuBuffer m_constantBuffer;

	AssetPtr<VertexShader> m_vertexShader;
	AssetPtr<PixelShader> m_pixelShader;
};

} // namespace Anker
