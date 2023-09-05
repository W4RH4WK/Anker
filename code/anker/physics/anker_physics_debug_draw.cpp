#include <anker/physics/anker_physics_debug_draw.hpp>

#include <anker/graphics/anker_gizmo_renderer.hpp>

namespace Anker {

PhysicsDebugDraw::PhysicsDebugDraw(GizmoRenderer& gizmoRenderer) : m_gizmos(gizmoRenderer)
{
	SetFlags(e_shapeBit);
}

void PhysicsDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	for (int i = 1; i < vertexCount; ++i) {
		m_gizmos.addLine(vertices[i - 1], vertices[i], color);
	}
	m_gizmos.addLine(vertices[vertexCount - 1], vertices[0], color);
}

void PhysicsDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	DrawPolygon(vertices, vertexCount, color);

	Vec4 fillColor = Vec4(color) * 0.5f;
	for (int i = 1; i < vertexCount - 1; ++i) {
		m_gizmos.addTriangle(vertices[0], vertices[i], vertices[i + 1], fillColor);
	}
}

void PhysicsDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	m_gizmos.addLine(p1, p2, color);
}

} // namespace Anker
