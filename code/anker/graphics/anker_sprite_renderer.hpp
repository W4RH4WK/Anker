#pragma once

#include <anker/core/anker_asset.hpp>
#include <anker/graphics/anker_render_device.hpp>

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

	void draw(const Scene&);

  private:
	struct Vertex {
		Vec2 position = Vec2(0);
		Vec2 uv = Vec2(0);
		Vec4 color = Vec4(0);
	};

	RenderDevice& m_renderDevice;

	std::vector<Vertex> m_vertices;

	AssetPtr<VertexShader> m_vertexShader;
	AssetPtr<PixelShader> m_pixelShader;
	GpuBuffer m_vertexBuffer;
};

} // namespace Anker
