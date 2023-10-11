#pragma once

#include <anker/core/anker_entity_name.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/physics/anker_physics_body.hpp>

namespace Anker {

class PhysicsContactListener : public b2ContactListener {
  public:
	PhysicsContactListener(Scene& scene) : m_scene(scene) {}
	PhysicsContactListener(const PhysicsContactListener&) = delete;
	PhysicsContactListener& operator=(const PhysicsContactListener&) = delete;
	PhysicsContactListener(PhysicsContactListener&&) noexcept = delete;
	PhysicsContactListener& operator=(PhysicsContactListener&&) noexcept = delete;

	void BeginContact(b2Contact* contact) override
	{
		auto entityA = m_scene.entityHandle(EntityID(contact->GetFixtureA()->GetBody()->GetUserData().entityID));
		if (auto* body = entityA.try_get<PhysicsBody>()) {
			body->touchingContacts.push_back(contact);
		} else {
			ANKER_ERROR("Invalid PhysicsBody reference: {}", entityImGuiLabel(entityA));
		}

		auto entityB = m_scene.entityHandle(EntityID(contact->GetFixtureB()->GetBody()->GetUserData().entityID));
		if (auto* body = entityB.try_get<PhysicsBody>()) {
			body->touchingContacts.push_back(contact);
		} else {
			ANKER_ERROR("Invalid PhysicsBody reference: {}", entityImGuiLabel(entityA));
		}
	}

	void EndContact(b2Contact* contact) override
	{
		// EndContact is also triggered when a body gets destroyed. In this case
		// the PhysicsBody component may have been removed already.

		auto entityA = EntityID(contact->GetFixtureA()->GetBody()->GetUserData().entityID);
		if (auto* body = m_scene.registry.try_get<PhysicsBody>(entityA)) {
			std::erase(body->touchingContacts, contact);
		}

		auto entityB = EntityID(contact->GetFixtureB()->GetBody()->GetUserData().entityID);
		if (auto* body = m_scene.registry.try_get<PhysicsBody>(entityB)) {
			std::erase(body->touchingContacts, contact);
		}
	}

  private:
	Scene& m_scene;
};

} // namespace Anker
