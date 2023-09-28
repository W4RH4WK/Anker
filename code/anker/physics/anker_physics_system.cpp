#include <anker/physics/anker_physics_system.hpp>

#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_scene_node.hpp>
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
			auto& node = scene.registry.get_or_emplace<SceneNode>(entity);
			node.setGlobalTransform(body.globalTransform());
		}
	}
}

static void createPhysicsBody(entt::registry& reg, EntityID entity)
{
	auto& physicsWorld = reg.ctx().get<PhysicsWorld>();

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.userData.entityID = entt::to_integral(entity);
	if (auto* node = reg.try_get<SceneNode>(entity)) {
		bodyDef.position = node->globalTransform().position;
		bodyDef.angle = node->globalTransform().rotation;
	}
	reg.get<PhysicsBody>(entity).body = physicsWorld.CreateBody(&bodyDef);
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

	const b2Vec2 gravity = 10.0f * Vec2::WorldDown;

	scene.physicsWorld.emplace(gravity);
	scene.physicsWorld->SetDebugDraw(&m_debugDraw);

	// Adding an alias for when we don't have access to the Scene object.
	scene.registry.ctx().emplace<PhysicsWorld&>(scene.physicsWorld);

	scene.registry.on_construct<PhysicsBody>().connect<createPhysicsBody>();
	scene.registry.on_destroy<PhysicsBody>().connect<destroyPhysicsBody>();
}

} // namespace Anker
