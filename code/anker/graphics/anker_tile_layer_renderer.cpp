#include <anker/graphics/anker_tile_layer_renderer.hpp>

#include <anker/core/anker_asset_cache.hpp>
#include <anker/core/anker_entity_name.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_scene_node.hpp>
#include <anker/core/anker_transform.hpp>
#include <anker/game/anker_map.hpp>
#include <anker/graphics/anker_tile_layer.hpp>

namespace Anker {

struct MapRendererConstantBuffer {
	Mat4 transform = Mat4Id; // Mat4 instead of Mat3 because of alignment
	Vec4 color = Vec4(1);
	Vec2 parallax = Vec2(1);
	Vec2 _pad;
};
static_assert(sizeof(MapRendererConstantBuffer) % 16 == 0, "Constant Buffer size must be 16-byte aligned");

TileLayerRenderer::TileLayerRenderer(RenderDevice& renderDevice, AssetCache& assetCache) : m_renderDevice(renderDevice)
{
	m_constantBuffer.info = {
	    .name = "TileLayerRenderer Constant Buffer",
	    .size = sizeof(MapRendererConstantBuffer),
	    .stride = sizeof(MapRendererConstantBuffer),
	    .bindFlags = GpuBindFlag::ConstantBuffer,
	    .flags = GpuBufferFlag::CpuWriteable,
	};
	if (not m_renderDevice.createBuffer(m_constantBuffer)) {
		ANKER_FATAL("Failed to create TileLayerRenderer Constant Buffer");
	}

	m_vertexShader = assetCache.loadVertexShader("shaders/map.vs", Vertex2D::ShaderInputs);
	m_pixelShader = assetCache.loadPixelShader("shaders/map.ps");
}

void TileLayerRenderer::draw(const Scene&, const SceneNode* node)
{
	ANKER_PROFILE_ZONE();

	auto* layer = node->entity().try_get<TileLayer>();
	if (!layer) {
		return;
	}

	m_renderDevice.bindVertexShader(*m_vertexShader);
	m_renderDevice.bindPixelShader(*m_pixelShader);

	{
		MapRendererConstantBuffer cb = {
		    .transform = Mat3(node->globalTransform()),
		    .color = layer->color,
		    .parallax = layer->parallax,
		};
		m_renderDevice.fillBuffer(m_constantBuffer, std::array{cb});
		m_renderDevice.bindBufferVS(1, m_constantBuffer);
		m_renderDevice.bindBufferPS(1, m_constantBuffer);
	}

	for (auto& [vertexBuffer, texture] : layer->parts) {
		if (texture) {
			m_renderDevice.bindTexturePS(0, *texture);
			m_renderDevice.draw(vertexBuffer);
			m_renderDevice.unbindTexturePS(0);
		}
	}
}

} // namespace Anker
