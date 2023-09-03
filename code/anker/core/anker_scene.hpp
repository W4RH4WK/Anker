#pragma once

namespace Anker {

class Scene;
using ScenePtr = std::shared_ptr<Scene>;

class Scene {
  public:
	Scene() = default;
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;
	Scene(Scene&&) noexcept = delete;
	Scene& operator=(Scene&&) noexcept = delete;

	EntityHandle createEntity(std::string_view name = "");
	EntityHandle entityHandle(EntityID);
	EntityCHandle entityHandle(EntityID) const;

	EntityHandle activeCamera();
	EntityCHandle activeCamera() const;
	void setActiveCamera(EntityID);

	entt::registry registry;
};

} // namespace Anker
