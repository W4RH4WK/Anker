#pragma once

#include <anker/core/anker_asset.hpp>

#include "anker_render_device.hpp"

namespace Anker {

class AssetCache;
class Scene;

class SpriteRenderer {
  public:
	explicit SpriteRenderer(RenderDevice&, AssetCache&);
	SpriteRenderer(const SpriteRenderer&) = delete;
	SpriteRenderer& operator=(const SpriteRenderer&) = delete;
	SpriteRenderer(SpriteRenderer&&) noexcept = delete;
	SpriteRenderer& operator=(SpriteRenderer&&) noexcept = delete;

	void draw(const Scene&);

  private:
	struct Vertex {
		Vec2 position;
		Vec2 uv;
	};

	RenderDevice& m_renderDevice;

	AssetPtr<VertexShader> m_vertexShader;
	AssetPtr<PixelShader> m_pixelShader;
	GpuBuffer m_constantBuffer;
	GpuBuffer m_vertexBuffer;
};

} // namespace Anker
