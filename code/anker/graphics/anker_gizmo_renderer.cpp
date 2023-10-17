#include <anker/graphics/anker_gizmo_renderer.hpp>

#include <anker/core/anker_asset_cache.hpp>

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

void GizmoRenderer::addPoint(const Vec2& point, const Vec4& color)
{
	addLine(point + Vec2(-0.05f, 0.0f), point + Vec2(0.05f, 0.0f), color);
	addLine(point + Vec2(0.0f, -0.05f), point + Vec2(0.0f, 0.05f), color);
}

void GizmoRenderer::addLine(const Vec2& from, const Vec2& to, const Vec4& color)
{
	m_verticesForLines.emplace_back(from, color);
	m_verticesForLines.emplace_back(to, color);
}

void GizmoRenderer::addTriangle(const Vec2& v0, const Vec2& v1, const Vec2& v2, const Vec4& color)
{
	m_verticesForTriangles.emplace_back(v0, color);
	m_verticesForTriangles.emplace_back(v1, color);
	m_verticesForTriangles.emplace_back(v2, color);
}

void GizmoRenderer::addRect(const Rect2& rect, const Vec4& color)
{
	addLine(rect.topLeft(), rect.topRight(), color);
	addLine(rect.topRight(), rect.bottomRight(), color);
	addLine(rect.bottomRight(), rect.bottomLeft(), color);
	addLine(rect.bottomLeft(), rect.topLeft(), color);
}

void GizmoRenderer::addGrid(float size, Vec4 color)
{
	for (float x = -size; x <= size; ++x) {
		addLine({x, -size}, {x, size}, color);
	}
	for (float y = -size; y <= size; ++y) {
		addLine({-size, y}, {size, y}, color);
	}
}

void GizmoRenderer::draw()
{
	if (m_verticesForLines.empty() && m_verticesForTriangles.empty()) {
		return;
	}

	m_renderDevice.bindVertexShader(*m_vertexShader);
	m_renderDevice.bindPixelShader(*m_pixelShader);
	m_renderDevice.setRasterizer({.depthClip = false});

	if (!m_verticesForLines.empty()) {
		m_renderDevice.fillBuffer(m_vertexBuffer, m_verticesForLines);
		m_renderDevice.draw(m_vertexBuffer, u32(m_verticesForLines.size()), Topology::LineList);
		m_verticesForLines.clear();
	}

	if (!m_verticesForTriangles.empty()) {
		m_renderDevice.fillBuffer(m_vertexBuffer, m_verticesForTriangles);
		m_renderDevice.draw(m_vertexBuffer, u32(m_verticesForTriangles.size()), Topology::TriangleList);
		m_verticesForTriangles.clear();
	}
}

} // namespace Anker
