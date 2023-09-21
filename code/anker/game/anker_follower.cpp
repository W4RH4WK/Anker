#include <anker/game/anker_follower.hpp>

#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_scene_node.hpp>

namespace Anker {

void FollowerSystem::tick(float dt, Scene& scene)
{
	for (auto [_, node, follower] : scene.registry.view<SceneNode, Follower>().each()) {
		auto* targetNode = scene.entityHandle(follower.target).try_get<SceneNode>();
		if (!targetNode) {
			continue;
		}

		Vec2 targetPosition = targetNode->globalTransform().position;

		Transform2D transform = node.globalTransform();
		transform.position.x = interpolate(transform.position.x, targetPosition.x, dt, follower.speed);
		transform.position.y = interpolate(transform.position.y, targetPosition.y, dt, follower.speed);
		node.setGlobalTransform(transform);
	}
}

} // namespace Anker
