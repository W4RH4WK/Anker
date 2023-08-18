#include <anker/core/anker_scene.hpp>

#include <anker/core/anker_entity_name.hpp>
#include <anker/core/anker_transform.hpp>
#include <anker/graphics/anker_camera.hpp>

namespace Anker {

ScenePtr Scene::create()
{
	return ScenePtr(new Scene);
}

EntityHandle Scene::createEntity(std::string_view name)
{
	EntityHandle entity = {registry, registry.create()};
	if (!name.empty()) {
		entity.emplace<EntityName>().name = name;
	}
	return entity;
}

EntityHandle Scene::entityHandle(EntityId id)
{
	return {registry, id};
}

EntityCHandle Scene::entityHandle(EntityId id) const
{
	return {registry, id};
}

Scene::Scene()
{
	activeCamera = createEntity("Camera");
	activeCamera.emplace<Transform2D>(Transform2D{
	    .position = Vec2Right / 2.0f,
	    //.rotation = glm::radians(45.0f),
	});
	activeCamera.emplace<Camera>();
}

} // namespace Anker
