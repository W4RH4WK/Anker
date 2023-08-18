#pragma once

namespace Anker {

class Scene;
using ScenePtr = std::shared_ptr<Scene>;

class Scene {
  public:
	static ScenePtr create();

	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;
	Scene(Scene&&) noexcept = delete;
	Scene& operator=(Scene&&) noexcept = delete;

	EntityHandle createEntity(std::string_view name = "");
	EntityHandle entityHandle(EntityID);
	EntityCHandle entityHandle(EntityID) const;

	entt::registry registry;

	EntityHandle activeCamera;

  private:
	Scene();
};

} // namespace Anker
