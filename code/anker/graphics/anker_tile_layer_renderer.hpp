#pragma once

#include <anker/graphics/anker_render_device.hpp>
#include <anker/graphics/anker_vertex.hpp>

namespace Anker {

class AssetCache;
class Scene;
class SceneNode;

class TileLayerRenderer {
  public:
	TileLayerRenderer(RenderDevice&, AssetCache&);
	TileLayerRenderer(const TileLayerRenderer&) = delete;
	TileLayerRenderer& operator=(const TileLayerRenderer&) = delete;
	TileLayerRenderer(TileLayerRenderer&&) noexcept = delete;
	TileLayerRenderer& operator=(TileLayerRenderer&&) noexcept = delete;

	void draw(const Scene&, const SceneNode*);

	using Vertex = Vertex2D;

  private:
	RenderDevice& m_renderDevice;

	GpuBuffer m_constantBuffer;

	AssetPtr<VertexShader> m_vertexShader;
	AssetPtr<PixelShader> m_pixelShader;
};

} // namespace Anker
