#include <anker/physics/anker_physics_system.hpp>

#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_transform.hpp>
#include <anker/physics/anker_physics_body.hpp>

namespace Anker {

PhysicsSystem::PhysicsSystem(GizmoRenderer& gizmoRenderer) : m_debugDraw(gizmoRenderer) {}

void PhysicsSystem::tick(float dt, Scene& scene)
{
	auto& physicsWorld = scene.registry.ctx().get<b2World>();
	physicsWorld.Step(dt, 6, 2);

	if (debugDraw) {
		physicsWorld.DebugDraw();
	}

	for (auto [entity, body] : scene.registry.view<PhysicsBody>().each()) {
		if (body.body && body.body->IsAwake()) {
			scene.registry.emplace_or_replace<Transform2D>(entity, body.transform());
		}
	}
}

static void destroyPhysicsBody(entt::registry& reg, EntityID entity)
{
	auto& physicsWorld = reg.ctx().get<PhysicsWorld>();
	if (auto* body = reg.get<PhysicsBody>(entity).body) {
		physicsWorld.DestroyBody(body);
	}
}

void PhysicsSystem::addPhysicsWorld(Scene& scene)
{
	if (scene.physicsWorld) {
		ANKER_WARN("Scene already has PhysicsWorld attached");
		return;
	}

	const b2Vec2 gravity = 10.0f * Vec2::Down;

	scene.physicsWorld.emplace(gravity);
	scene.physicsWorld->SetDebugDraw(&m_debugDraw);

	// Adding an alias for when we don't have access to the Scene object.
	scene.registry.ctx().emplace<PhysicsWorld&>(scene.physicsWorld);

	scene.registry.on_destroy<PhysicsBody>().connect<destroyPhysicsBody>();
}

} // namespace Anker
