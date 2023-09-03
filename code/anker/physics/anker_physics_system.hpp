#pragma once

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
	std::unique_ptr<b2Draw> m_debugDraw;
};

} // namespace Anker
