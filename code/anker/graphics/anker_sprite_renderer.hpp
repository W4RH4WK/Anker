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
	struct Vertex {
		Vec2 position = Vec2(0);
		Vec2 uv = Vec2(0);
	};

	struct InstanceData {
		Mat4 transform = Mat4Id;
		Vec4 textureRect = Vec4(0);
		Vec4 color = Vec4(0);
		Vec2 parallax = Vec2(1);
	};

	RenderDevice& m_renderDevice;

	std::vector<InstanceData> m_instanceData;

	AssetPtr<VertexShader> m_vertexShader;
	AssetPtr<PixelShader> m_pixelShader;
	GpuBuffer m_vertexBuffer;
	GpuBuffer m_instanceBuffer;
};

} // namespace Anker
