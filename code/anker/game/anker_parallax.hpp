#pragma once

namespace Anker {

struct Parallax {
	// Parallax factor as given by Tiled. 1 is equal to no parallax, 0 is
	// one-to-one with the camera.
	Vec2 factor = Vec2(1);

	// Initialized from Transform2D on first tick.
	std::optional<Vec2> offset;
};

} // namespace Anker

REFL_TYPE(Anker::Parallax)
REFL_FIELD(factor)
REFL_END

namespace Anker {

class Scene;

class ParallaxSystem {
  public:
	ParallaxSystem() = default;
	ParallaxSystem(const ParallaxSystem&) = delete;
	ParallaxSystem& operator=(const ParallaxSystem&) = delete;
	ParallaxSystem(ParallaxSystem&&) noexcept = delete;
	ParallaxSystem& operator=(ParallaxSystem&&) noexcept = delete;

	void tick(float, Scene&);
};

} // namespace Anker
