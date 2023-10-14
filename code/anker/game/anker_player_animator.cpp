#include <anker/game/anker_player_animator.hpp>

#include <anker/core/anker_scene.hpp>
#include <anker/game/anker_player_controller.hpp>
#include <anker/graphics/anker_sprite.hpp>

namespace Anker {

void PlayerAnimator::tick(float, Scene& scene)
{
	for (auto [_, controller, sprite] : scene.registry.view<PlayerAnimator, PlayerController, Sprite>().each()) {
		Vec2 velocity = controller.velocity();
		if (velocity.x < 0) {
			sprite.flipX = true;
		} else if (velocity.x > 0) {
			sprite.flipX = false;
		}
	}
}

} // namespace Anker
