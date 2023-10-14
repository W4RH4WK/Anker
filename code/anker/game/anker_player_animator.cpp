#include <anker/game/anker_player_animator.hpp>

#include <anker/core/anker_scene.hpp>
#include <anker/game/anker_player_controller.hpp>
#include <anker/graphics/anker_sprite.hpp>

namespace Anker {

void PlayerAnimator::tick(float, Scene& scene)
{
	for (auto [_, controller, sprite] : scene.registry.view<PlayerAnimator, PlayerController, Sprite>().each()) {
		sprite.flipX = controller.lookDirection().x < 0;
	}
}

} // namespace Anker
