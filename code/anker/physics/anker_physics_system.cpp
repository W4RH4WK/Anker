#include <anker/physics/anker_physics_system.hpp>

#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_transform.hpp>
#include <anker/graphics/anker_gizmo_renderer.hpp>
#include <anker/physics/anker_physics_body.hpp>

namespace Anker {

////////////////////////////////////////////////////////////
// Box2D Debug Draw Adapter

class B2DebugDraw : public b2Draw {
  public:
	B2DebugDraw(GizmoRenderer& gizmoRenderer) : m_gizmos(gizmoRenderer) { SetFlags(e_shapeBit); }

	virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override
	{
		for (int i = 1; i < vertexCount; ++i) {
			m_gizmos.addLine(vertices[i - 1], vertices[i], color);
		}
		m_gizmos.addLine(vertices[vertexCount - 1], vertices[0], color);
	}

	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override
	{
		DrawPolygon(vertices, vertexCount, color);

		Vec4 fillColor = Vec4(color) * 0.5f;
		for (int i = 1; i < vertexCount - 1; ++i) {
			m_gizmos.addTriangle(vertices[0], vertices[i], vertices[i + 1], fillColor);
		}
	}

	virtual void DrawCircle(const b2Vec2&, float, const b2Color&) override {}

	virtual void DrawSolidCircle(const b2Vec2&, float, const b2Vec2&, const b2Color&) override {}

	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override
	{
		m_gizmos.addLine(p1, p2, color);
	}

	virtual void DrawTransform(const b2Transform&) override {}

	virtual void DrawPoint(const b2Vec2&, float, const b2Color&) override {}

  private:
	GizmoRenderer& m_gizmos;
};

////////////////////////////////////////////////////////////
// Physics System

PhysicsSystem::PhysicsSystem(GizmoRenderer& gizmoRenderer)
{
	m_debugDraw = std::make_unique<B2DebugDraw>(gizmoRenderer);
}

void PhysicsSystem::tick(float dt, Scene& scene)
{
	auto& physicsWorld = scene.registry.ctx().get<b2World>();
	physicsWorld.Step(dt, 6, 2);

	if (debugDraw) {
		physicsWorld.DebugDraw();
	}

	for (auto [entity, body] : scene.registry.view<PhysicsBody>().each()) {
		if (body.body->IsAwake()) {
			scene.registry.emplace_or_replace<Transform2D>(entity, body.transform());
		}
	}
}

void PhysicsSystem::addPhysicsWorld(Scene& scene)
{
	if (scene.registry.ctx().contains<b2World>()) {
		ANKER_WARN("Scene already has PhysicsWorld attached");
		return;
	}

	const b2Vec2 gravity = 10.0f * Vec2::Down;

	auto& physicsWorld = scene.registry.ctx().emplace<b2World>(gravity);
	physicsWorld.SetDebugDraw(m_debugDraw.get());

	static constexpr auto destroyPhysicsBody = [](entt::registry& reg, EntityID entity) {
		auto& physicsWorld = reg.ctx().get<b2World>();
		physicsWorld.DestroyBody(reg.get<PhysicsBody>(entity).body);
	};
	scene.registry.on_destroy<PhysicsBody>().connect<destroyPhysicsBody>();
}

} // namespace Anker
