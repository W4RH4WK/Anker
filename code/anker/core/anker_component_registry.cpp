#include <anker/core/anker_component_registry.hpp>

#include <anker/core/anker_entity_name.hpp>
#include <anker/core/anker_transform.hpp>
#include <anker/editor/anker_editor_camera.hpp>
#include <anker/physics/anker_physics_body.hpp>
#include <anker/graphics/anker_camera.hpp>
#include <anker/graphics/anker_sprite.hpp>

namespace Anker {

ComponentRegistry::ComponentRegistry()
{
	registerComponent<EntityName>("Name", -2);
	registerComponent<Transform2D>("Transform2D", -1);
	registerComponent<Camera>("Camera");
	registerComponent<Sprite>("Sprite");
	registerComponent<PhysicsBody>("PhysicsBody");
	registerComponent<EditorCamera>("EditorCamera");
}

const ComponentInfo* ComponentRegistry::componentByID(entt::id_type id) const
{
	if (auto it = m_lookupByID.find(id); it != m_lookupByID.end()) {
		return it->second;
	} else {
		return nullptr;
	}
}

const ComponentInfo* ComponentRegistry::componentByName(std::string_view name) const
{
	if (auto it = m_lookupByName.find({name.data(), name.size()}); it != m_lookupByName.end()) {
		return it->second;
	} else {
		return nullptr;
	}
}

void ComponentRegistry::addComponentInfo(const ComponentInfo& info)
{
	ANKER_CHECK(!componentByID(info.id));
	ANKER_CHECK(!componentByName(info.name));

	auto* insertedInfo = m_infos.emplace_back(std::make_unique<ComponentInfo>(info)).get();

	const auto bySortOrder = [](auto* a, auto* b) { return a->sortOrder < b->sortOrder; };
	m_sortedInfos.insert(std::upper_bound(m_sortedInfos.begin(), m_sortedInfos.end(), insertedInfo, bySortOrder),
	                     insertedInfo);

	m_lookupByID[insertedInfo->id] = insertedInfo;
	m_lookupByName[insertedInfo->name] = insertedInfo;

	ANKER_CHECK(m_infos.size() == m_sortedInfos.size());
	ANKER_CHECK(m_infos.size() == m_lookupByID.size());
	ANKER_CHECK(m_infos.size() == m_lookupByName.size());
}

} // namespace Anker
