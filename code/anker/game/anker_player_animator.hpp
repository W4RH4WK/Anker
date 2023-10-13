#pragma once

namespace Anker {

class Scene;

struct PlayerAnimator {
	static void tick(float, Scene&);
};

} // namespace Anker

REFL_TYPE(Anker::PlayerAnimator)
REFL_END
