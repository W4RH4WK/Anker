#include <anker/graphics/anker_gizmo_renderer.hpp>

#include <anker/core/anker_asset_cache.hpp>
#include <anker/core/anker_transform.hpp>

namespace Anker {

GizmoRenderer::GizmoRenderer(RenderDevice& renderDevice, AssetCache& assetCache) : m_renderDevice(renderDevice)
{
	const std::array shaderInputDescription{
	    D3D11_INPUT_ELEMENT_DESC{
	        .SemanticName = "POSITION",
	        .Format = DXGI_FORMAT_R32G32_FLOAT,
	        .AlignedByteOffset = offsetof(GizmoRenderer::Vertex, position),
	        .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
	    },
	    D3D11_INPUT_ELEMENT_DESC{
	        .SemanticName = "COLOR",
	        .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
	        .AlignedByteOffset = offsetof(GizmoRenderer::Vertex, color),
	        .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
	    },
	};

	m_vertexShader = assetCache.loadVertexShader("shaders/gizmo.vs", shaderInputDescription);
	m_pixelShader = assetCache.loadPixelShader("shaders/gizmo.ps");

	m_vertexBuffer.info = {
	    .name = "GizmoRenderer Vertex Buffer",
	    .size = 1024 * sizeof(Vertex),
	    .stride = sizeof(Vertex),
	    .bindFlags = GpuBindFlag::VertexBuffer,
	    .flags = GpuBufferFlag::CpuWriteable,
	};
	if (not m_renderDevice.createBuffer(m_vertexBuffer)) {
		ANKER_FATAL("Failed to create GizmoRenderer Vertex Buffer");
	}
}

void GizmoRenderer::addLine(const Vec2& from, const Vec2& to, const Vec4& color)
{
	m_verticesForLines.emplace_back(from, color);
	m_verticesForLines.emplace_back(to, color);
}

void GizmoRenderer::draw()
{
	if (m_verticesForLines.empty()) {
		return;
	}

	m_renderDevice.fillBuffer(m_vertexBuffer, m_verticesForLines);

	m_renderDevice.bindVertexShader(*m_vertexShader);
	m_renderDevice.bindPixelShader(*m_pixelShader);
	m_renderDevice.setRasterizer({.depthClip = false});
	m_renderDevice.draw(m_vertexBuffer, uint32_t(m_verticesForLines.size()), Topology::LineList);

	m_verticesForLines.clear();
}

} // namespace Anker
