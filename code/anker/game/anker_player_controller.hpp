#pragma once

namespace Anker {

class Scene;

struct PlayerController {
	static void tick(float, Scene&);
};

} // namespace Anker

REFL_TYPE(Anker::PlayerController)
REFL_END
