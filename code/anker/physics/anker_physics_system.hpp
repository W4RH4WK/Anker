#pragma once

namespace Anker {

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

	b2Draw* debugDrawInterface() { return m_debugDraw.get(); }

  private:
	std::unique_ptr<b2Draw> m_debugDraw;
};

} // namespace Anker
