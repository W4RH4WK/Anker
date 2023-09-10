#include <anker/game/anker_parallax.hpp>

#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_transform.hpp>

namespace Anker {

void ParallaxSystem::tick(float, Scene& scene)
{
	auto* cameraTransform = scene.activeCamera().try_get<Transform2D>();
	if (!cameraTransform) {
		return;
	}

	for (auto [_, transform, parallax] : scene.registry.view<Transform2D, Parallax>().each()) {
		if (!parallax.offset) {
			parallax.offset = transform.position;
		}

		transform.position = (Vec2(1) - parallax.factor) * cameraTransform->position //
		                   + *parallax.offset;
	}
}

} // namespace Anker
