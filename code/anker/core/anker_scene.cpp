#include <anker/core/anker_scene.hpp>

#include <anker/core/anker_entity_name.hpp>

namespace Anker {

EntityHandle Scene::createEntity(std::string_view name)
{
	EntityHandle entity = {registry, registry.create()};
	if (!name.empty()) {
		entity.emplace<EntityName>().name = name;
	}
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
