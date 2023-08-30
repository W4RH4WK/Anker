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
			m_gizmos.addLine(toVec(vertices[i - 1]), toVec(vertices[i]), toVec(color));
		}
		m_gizmos.addLine(toVec(vertices[vertexCount - 1]), toVec(vertices[0]), toVec(color));
	}

	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override
	{
		DrawPolygon(vertices, vertexCount, color);
	}

	virtual void DrawCircle(const b2Vec2&, float, const b2Color&) override {}

	virtual void DrawSolidCircle(const b2Vec2&, float, const b2Vec2&, const b2Color&) override {}

	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override
	{
		m_gizmos.addLine(toVec(p1), toVec(p2), toVec(color));
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
	scene.physicsWorld->Step(dt, 6, 2);
	scene.physicsWorld->DebugDraw();

	for (auto [_, transform, body] : scene.registry.view<Transform2D, PhysicsBody>().each()) {
		transform = toTransform(body.body->GetTransform());
	}
}

void PhysicsSystem::addPhysicsWorld(Scene& scene)
{
	if (scene.physicsWorld) {
		ANKER_WARN("Scene already has PhysicsWorld attached");
		return;
	}

	const b2Vec2 gravity = {0, -10.0f};

	scene.physicsWorld = std::make_unique<b2World>(gravity);
	scene.physicsWorld->SetDebugDraw(m_debugDraw.get());

	static constexpr auto destroyPhysicsBody = [](Scene& scene, entt::registry& reg, EntityID entity) {
		if (scene.physicsWorld) {
			scene.physicsWorld->DestroyBody(reg.get<PhysicsBody>(entity).body);
		}
	};
	scene.registry.on_destroy<PhysicsBody>().connect<destroyPhysicsBody>(scene);
}

////////////////////////////////////////////////////////////
// Box2D Hooks

void* b2Alloc(int32_t size)
{
	return operator new(size);
}

void b2Free(void* mem)
{
	operator delete(mem);
}

void b2Log(const char* format, va_list args)
{
	std::array<char, 1024> buffer;
	vsnprintf(buffer.data(), buffer.size(), format, args);
	ANKER_INFO("Box2d: {}", buffer.data());
}

} // namespace Anker
