#pragma once

#include <anker/game/anker_player_camera_params.hpp>

namespace Anker {

class Scene;
class SceneNode;
class PlayerController;

// This component is to be attached to a Camera and will drive that camera to
// follow the player.
struct PlayerCameraFollower {
	EntityID player = entt::null;

	bool snapOnce = true;

	float fallingOffsetDelayLeft = 0;

	PlayerCameraParams params;

	void tickFollow(float dt, SceneNode& cameraNode, const SceneNode& playerNode, const PlayerController&);

	static void tick(float, Scene&);
};

} // namespace Anker

REFL_TYPE(Anker::PlayerCameraFollower)
REFL_FIELD(player)
REFL_FIELD(snapOnce)
REFL_FIELD(params)
REFL_END
