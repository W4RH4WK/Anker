#include "anker_scene.hpp"

namespace Anker {

ScenePtr Scene::create()
{
	return ScenePtr(new Scene);
}

EntityHandle Scene::createEntity(std::string_view)
{
	EntityHandle entity = {registry, registry.create()};
	// if (!name.empty()) {
	//	entity.emplace<EntityName>().name.assign(name.data(), name.size());
	// }
	return entity;
}

EntityHandle Scene::entityHandle(EntityID id)
{
	return {registry, id};
}

EntityCHandle Scene::entityHandle(EntityID id) const
{
	return {registry, id};
}

} // namespace Anker
