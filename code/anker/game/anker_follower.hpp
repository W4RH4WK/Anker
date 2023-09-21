#pragma once

namespace Anker {

struct Follower {
	EntityID target = entt::null;
	float speed = 1.0f;
};

} // namespace Anker

REFL_TYPE(Anker::Follower)
REFL_FIELD(target)
REFL_FIELD(speed)
REFL_END

namespace Anker {

class Scene;

class FollowerSystem {
  public:
	FollowerSystem() = default;
	FollowerSystem(const FollowerSystem&) = delete;
	FollowerSystem& operator=(const FollowerSystem&) = delete;
	FollowerSystem(FollowerSystem&&) noexcept = delete;
	FollowerSystem& operator=(FollowerSystem&&) noexcept = delete;

	void tick(float, Scene&);
};

} // namespace Anker
