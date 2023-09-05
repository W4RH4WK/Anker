#pragma once

#include <anker/physics/anker_physics_debug_draw.hpp>

namespace Anker {

using PhysicsWorld = b2World;

class Scene;
class GizmoRenderer;

class PhysicsSystem {
  public:
	PhysicsSystem(GizmoRenderer&);
	PhysicsSystem(const PhysicsSystem&) = delete;
	PhysicsSystem& operator=(const PhysicsSystem&) = delete;
	PhysicsSystem(PhysicsSystem&&) noexcept = delete;
	PhysicsSystem& operator=(PhysicsSystem&&) noexcept = delete;

	void tick(float, Scene&);

	void addPhysicsWorld(Scene&);

	bool debugDraw = false;

  private:
	PhysicsDebugDraw m_debugDraw;
};

} // namespace Anker
