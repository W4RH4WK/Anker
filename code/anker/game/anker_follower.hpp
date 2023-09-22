#pragma once

namespace Anker {

class Scene;

struct Follower {
	EntityID target = entt::null;
	Vec2 speed = Vec2(1);

	static void tick(float, Scene&);
};

} // namespace Anker

REFL_TYPE(Anker::Follower)
REFL_FIELD(target)
REFL_FIELD(speed)
REFL_END
