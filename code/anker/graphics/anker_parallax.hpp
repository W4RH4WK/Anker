#pragma once

namespace Anker {

struct Parallax {
	// Parallax factor as given by Tiled. 1 is equal to no parallax, 0 is
	// one-to-one with the camera.
	Vec2 factor = Vec2(1);
};

} // namespace Anker

REFL_TYPE(Anker::Parallax)
REFL_FIELD(factor)
REFL_END
