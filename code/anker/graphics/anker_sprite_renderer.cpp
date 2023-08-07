#include "anker_sprite_renderer.hpp"

#include <anker/core/anker_asset_cache.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_transform.hpp>

#include "anker_sprite.hpp"

namespace Anker {

SpriteRenderer::SpriteRenderer(RenderDevice& renderDevice, AssetCache& assetCache) : m_renderDevice(renderDevice)
{
	const std::array shaderInputDescription{
	    D3D11_INPUT_ELEMENT_DESC{
	        .SemanticName = "POSITION",
	        .Format = DXGI_FORMAT_R32G32B32_FLOAT,
	        .AlignedByteOffset = offsetof(Vertex, position),
	        .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
	    },
	    D3D11_INPUT_ELEMENT_DESC{
	        .SemanticName = "TEXCOORD",
	        .SemanticIndex = 0,
	        .Format = DXGI_FORMAT_R32G32_FLOAT,
	        .AlignedByteOffset = offsetof(Vertex, uv),
	        .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
	    },
	};

	m_vertexShader = assetCache.loadVertexShader("shaders/basic_2d.vs", shaderInputDescription);
	m_pixelShader = assetCache.loadPixelShader("shaders/basic_2d.ps");

	m_vertexBuffer.info = {
	    .name = "SpriteRenderer Vertex Buffer",
	    .size = 4 * sizeof(Vertex),
	    .bindFlags = GpuBindFlag::VertexBuffer,
	    .flags = GpuBufferFlag::CpuWriteable,
	};
	m_renderDevice.createBufferFor<Vertex>(m_vertexBuffer);
}

void SpriteRenderer::draw(const Scene& scene)
{
	m_renderDevice.bindVertexShader(*m_vertexShader);
	m_renderDevice.bindPixelShader(*m_pixelShader);

	for (const auto& [_, transform, sprite] : scene.registry.view<Transform2D, Sprite>().each()) {
		if (!sprite.texture) {
			continue;
		}

		std::array vertices = {
		    Vertex{{1.0f, -1.0f}, {1, 1}},
		    Vertex{{1.0f, 1.0f}, {1, 0}},
		    Vertex{{-1.0f, -1.0f}, {0, 1}},
		    Vertex{{-1.0f, 1.0f}, {0, 0}},
		};

		Vec2 textureSize = sprite.texture->info.size;

		for (auto& vertex : vertices) {
			vertex.position.x *= textureSize.x / textureSize.y;
			vertex.position = transform * vertex.position;
		}

		std::ranges::copy(vertices, m_renderDevice.mapBuffer<Vertex>(m_vertexBuffer));
		m_renderDevice.unmapBuffer(m_vertexBuffer);

		m_renderDevice.bindTexturePS(0, *sprite.texture);

		m_renderDevice.draw(m_vertexBuffer, uint32_t(vertices.size()), Topology::TriangleStrip);

		m_renderDevice.unbindTexturePS(0);
	}
}

} // namespace Anker
