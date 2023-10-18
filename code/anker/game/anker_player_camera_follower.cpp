#include <anker/game/anker_player_camera_follower.hpp>

#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_scene_node.hpp>
#include <anker/game/anker_player_controller.hpp>
#include <anker/graphics/anker_camera.hpp>

namespace Anker {

void PlayerCameraFollower::tickFollow(float dt, SceneNode& cameraNode, const SceneNode& playerNode,
                                      const PlayerController& playerController)
{
	Transform2D cameraTransform = cameraNode.globalTransform();

	Vec2 targetPosition = playerNode.globalTransform().position;
	targetPosition += params.baseOffset;
	targetPosition += params.lookOffset * playerController.lookDirection();

	Vec2 speed = params.baseSpeed;

	if (playerController.isFalling()) {
		if (fallingOffsetDelayLeft <= 0) {
			targetPosition.y += params.fallingOffset * playerController.velocity().y;
		}
		fallingOffsetDelayLeft -= dt;
	} else {
		fallingOffsetDelayLeft = params.fallingOffsetDelay;
	}

	if (playerController.isDashing() && !playerController.isDashingBackwards()) {
		speed.x = params.dashingSpeed;
	}

	if (snapOnce) {
		cameraTransform.position = targetPosition;
		snapOnce = false;
	} else {
		cameraTransform.position.moveTowards(targetPosition, speed, dt);
	}
	cameraNode.setGlobalTransform(cameraTransform);

	if (auto* camera = cameraNode.entity().try_get<Camera>()) {
		camera->distance = params.distance;
	}
}

void PlayerCameraFollower::tick(float dt, Scene& scene)
{
	for (auto [_, cameraNode, follower] : scene.registry.view<SceneNode, PlayerCameraFollower>().each()) {
		EntityHandle player = scene.entityHandle(follower.player);
		auto* playerNode = player.try_get<SceneNode>();
		auto* playerController = player.try_get<PlayerController>();
		if (!player || !playerNode || !playerController) {
			continue;
		}

		follower.tickFollow(dt, cameraNode, *playerNode, *playerController);
	}
}

} // namespace Anker
