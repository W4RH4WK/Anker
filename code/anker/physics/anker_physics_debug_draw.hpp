#pragma once

namespace Anker {

class GizmoRenderer;

class PhysicsDebugDraw : public b2Draw {
  public:
	PhysicsDebugDraw(GizmoRenderer& gizmoRenderer);

	virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

	virtual void DrawCircle(const b2Vec2&, float, const b2Color&) override {}

	virtual void DrawSolidCircle(const b2Vec2&, float, const b2Vec2&, const b2Color&) override {}

	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;

	virtual void DrawTransform(const b2Transform&) override {}

	virtual void DrawPoint(const b2Vec2&, float, const b2Color&) override {}

  private:
	GizmoRenderer& m_gizmos;
};

} // namespace Anker
