#include "anker_sprite_renderer.hpp"

#include <anker/core/anker_asset_cache.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_transform.hpp>

#include "anker_sprite.hpp"

namespace Anker {

struct SpriteRendererConstantBuffer {
	Mat4 transform = Mat4Id; // Mat4 instead of Mat3 because of alignment
};
static_assert(sizeof(SpriteRendererConstantBuffer) % 16 == 0, "Constant Buffer size must be 16-byte aligned");

SpriteRenderer::SpriteRenderer(RenderDevice& renderDevice, AssetCache& assetCache) : m_renderDevice(renderDevice)
{
	const std::array shaderInputDescription{
	    D3D11_INPUT_ELEMENT_DESC{
	        .SemanticName = "POSITION",
	        .Format = DXGI_FORMAT_R32G32_FLOAT,
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

	m_constantBuffer.info = {
	    .name = "SpriteRenderer Constant Buffer",
	    .size = sizeof(SpriteRendererConstantBuffer),
	    .bindFlags = GpuBindFlag::ConstantBuffer,
	    .flags = GpuBufferFlag::CpuWriteable,
	};
	m_renderDevice.createBuffer(m_constantBuffer);

	const std::array vertices = {
	    Vertex{{0.5f, -0.5f}, {1, 1}},
	    Vertex{{0.5f, 0.5f}, {1, 0}},
	    Vertex{{-0.5f, -0.5f}, {0, 1}},
	    Vertex{{-0.5f, 0.5f}, {0, 0}},
	};

	m_vertexBuffer.info = {
	    .name = "SpriteRenderer Vertex Buffer",
	    .size = uint32_t(vertices.size() * sizeof(vertices[0])),
	    .bindFlags = GpuBindFlag::VertexBuffer,
	};

	m_renderDevice.createBuffer<Vertex>(m_vertexBuffer, vertices);
}

void SpriteRenderer::draw(const Scene& scene)
{
	m_renderDevice.bindVertexShader(*m_vertexShader);
	m_renderDevice.bindPixelShader(*m_pixelShader);

	for (const auto& [_, transform, sprite] : scene.registry.view<Transform2D, Sprite>().each()) {
		if (!sprite.texture) {
			continue;
		}

		{
			auto* cb = m_renderDevice.mapBuffer<SpriteRendererConstantBuffer>(m_constantBuffer);
			*cb = SpriteRendererConstantBuffer{
			    .transform = transform.mat3(),
			};
			m_renderDevice.unmapBuffer(m_constantBuffer);
			m_renderDevice.bindBufferVS(1, m_constantBuffer);
			m_renderDevice.bindBufferPS(1, m_constantBuffer);
		}

		m_renderDevice.bindTexturePS(0, *sprite.texture);

		m_renderDevice.draw(m_vertexBuffer, 4, Topology::TriangleStrip);

		m_renderDevice.unbindTexturePS(0);
	}
}

} // namespace Anker
