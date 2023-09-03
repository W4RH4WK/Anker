#include <anker/core/anker_scene.hpp>

#include <anker/core/anker_entity_name.hpp>
#include <anker/graphics/anker_camera.hpp>

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

constexpr auto ActiveCameraTag = "ActiveCamera"_hs;

EntityHandle Scene::activeCamera()
{
	if (auto* entity = registry.ctx().find<EntityID>(ActiveCameraTag)) {
		return entityHandle(*entity);
	} else {
		return entityHandle(entt::null);
	}
}

EntityCHandle Scene::activeCamera() const
{
	return const_cast<Scene*>(this)->activeCamera();
}

void Scene::setActiveCamera(EntityID entity)
{
	registry.ctx().insert_or_assign(ActiveCameraTag, entity);
}

} // namespace Anker
