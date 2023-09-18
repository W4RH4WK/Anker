#include <anker/graphics/anker_map_renderer.hpp>

#include <anker/core/anker_asset_cache.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_transform.hpp>
#include <anker/game/anker_map.hpp>
#include <anker/game/anker_parallax.hpp>

namespace Anker {

struct MapRendererConstantBuffer {
	Mat4 transform = Mat4Id; // Mat4 instead of Mat3 because of alignment
	Vec4 color = Vec4(1);
	Vec2 parallax = Vec2(1);
	Vec2 _pad;
};
static_assert(sizeof(MapRendererConstantBuffer) % 16 == 0, "Constant Buffer size must be 16-byte aligned");

MapRenderer::MapRenderer(RenderDevice& renderDevice, AssetCache& assetCache) : m_renderDevice(renderDevice)
{
	m_constantBuffer.info = {
	    .name = "MapRenderer Constant Buffer",
	    .size = sizeof(MapRendererConstantBuffer),
	    .stride = sizeof(MapRendererConstantBuffer),
	    .bindFlags = GpuBindFlag::ConstantBuffer,
	    .flags = GpuBufferFlag::CpuWriteable,
	};
	if (not m_renderDevice.createBuffer(m_constantBuffer)) {
		ANKER_FATAL("Failed to create MapRenderer Constant Buffer");
	}

	m_vertexShader = assetCache.loadVertexShader("shaders/map.vs", Vertex2D::ShaderInputs);
	m_pixelShader = assetCache.loadPixelShader("shaders/map.ps");
}

void MapRenderer::collectRenderLayers(const Scene& scene, std::insert_iterator<std::set<RenderLayer>> inserter)
{
	for (auto [_, layer] : scene.registry.view<MapLayer>().each()) {
		inserter = layer.layer;
	}
}

void MapRenderer::draw(const Scene& scene, RenderLayer layerToRender)
{
	ANKER_PROFILE_ZONE();

	m_renderDevice.bindVertexShader(*m_vertexShader);
	m_renderDevice.bindPixelShader(*m_pixelShader);

	for (auto [entity, layer] : scene.registry.view<MapLayer>().each()) {
		if (layer.layer != layerToRender) {
			continue;
		}

		if (layer.vertexCount == 0 || !layer.texture) {
			continue;
		}

		{
			MapRendererConstantBuffer cb = {
			    .color = layer.color,
			};
			if (auto* transform = scene.registry.try_get<Transform2D>(entity)) {
				cb.transform = Mat3(*transform);
			}
			if (auto* parallax = scene.registry.try_get<Parallax>(entity)) {
				cb.parallax = parallax->factor;
			}
			m_renderDevice.fillBuffer(m_constantBuffer, std::array{cb});
			m_renderDevice.bindBufferVS(1, m_constantBuffer);
			m_renderDevice.bindBufferPS(1, m_constantBuffer);
		}

		m_renderDevice.bindTexturePS(0, *layer.texture);
		m_renderDevice.draw(layer.vertexBuffer, layer.vertexCount);
		m_renderDevice.unbindTexturePS(0);
	}
}

} // namespace Anker
