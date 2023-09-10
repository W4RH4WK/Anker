#pragma once

#include <anker/core/anker_asset.hpp>
#include <anker/graphics/anker_render_device.hpp>

namespace Anker {

struct Transform;
class AssetCache;

// Render system used to draw debug gizmos. Gizmos need to be re-added each
// frame --- internal state is cleared after draw.
class GizmoRenderer {
  public:
	GizmoRenderer(RenderDevice&, AssetCache&);
	GizmoRenderer(const GizmoRenderer&) = delete;
	GizmoRenderer& operator=(const GizmoRenderer&) = delete;
	GizmoRenderer(GizmoRenderer&&) noexcept = delete;
	GizmoRenderer& operator=(GizmoRenderer&&) noexcept = delete;

	void addLine(const Vec2& from, const Vec2& to, const Vec4& color = Vec4(1));

	void addTriangle(const Vec2& v0, const Vec2& v1, const Vec2& v2, const Vec4& color = Vec4(1));

	void addRect(const Rect2&, const Vec4& color = Vec4(1));

	void addGrid(float size, Vec4 color = {0.2f, 0.2f, 0.2f, 1});

	void draw();

  private:
	struct Vertex {
		Vec2 position;
		Vec4 color;
	};

	RenderDevice& m_renderDevice;

	AssetPtr<VertexShader> m_vertexShader;
	AssetPtr<PixelShader> m_pixelShader;
	GpuBuffer m_vertexBuffer;

	std::vector<Vertex> m_verticesForLines;
	std::vector<Vertex> m_verticesForTriangles;
};

} // namespace Anker
