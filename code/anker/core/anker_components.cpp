#include <anker/core/anker_components.hpp>

#include <anker/core/anker_entity_name.hpp>
#include <anker/core/anker_scene_node.hpp>
#include <anker/editor/anker_edit_widget_drawer.hpp>
#include <anker/editor/anker_editor_camera.hpp>
#include <anker/game/anker_follower.hpp>
#include <anker/game/anker_map.hpp>
#include <anker/game/anker_player_controller.hpp>
#include <anker/graphics/anker_camera.hpp>
#include <anker/graphics/anker_parallax.hpp>
#include <anker/graphics/anker_sprite.hpp>
#include <anker/physics/anker_physics_body.hpp>

namespace Anker {

template <typename Component>
concept HasTickFn = requires(Scene & s)
{
	{Component::tick(float(), s)};
};

template <typename Component>
constexpr ComponentInfo registerComponent(const char* name, ComponentFlags flags = ComponentFlag::None)
{
	ComponentInfo info{
	    .name = name,
	    .id = entt::type_hash<Component>(),
	    .flags = flags,

	    .addTo = [](EntityHandle entity) { entity.emplace<Component>(); },
	    .removeFrom = [](EntityHandle entity) { entity.remove<Component>(); },
	    .isPresentIn = [](EntityCHandle entity) { return entity.all_of<Component>(); },
	};

	if constexpr (HasTickFn<Component>) {
		info.tick = [](float dt, Scene& scene) { Component::tick(dt, scene); };
	}

	if constexpr (Serializable<EditWidgetDrawer, Component> && !std::is_empty_v<Component>) {
		info.drawEditWidget = [](EditWidgetDrawer& draw, EntityHandle entity) {
			if (auto* component = entity.try_get<Component>()) {
				draw(*component);
			}
		};
	}

	return info;
}

constexpr std::array Components = {
    registerComponent<EntityName>("Name", ComponentFlag::HideInInspector),
    registerComponent<SceneNode>("SceneNode"),
    registerComponent<Transform2D>("Transform2D"),
    registerComponent<Camera>("Camera"),
    registerComponent<Sprite>("Sprite"),
    registerComponent<PhysicsBody>("PhysicsBody"),
    registerComponent<PlayerController>("PlayerController"),
    registerComponent<Follower>("Follower"),
    registerComponent<EditorCamera>("EditorCamera"),
    registerComponent<MapLayer>("MapLayer"),
    registerComponent<Parallax>("Parallax"),
};

const std::unordered_map<entt::id_type, const ComponentInfo*> ComponentsById = []() {
	std::unordered_map<entt::id_type, const ComponentInfo*> componentsById;
	for (auto& info : Components) {
		componentsById[info.id] = &info;
	}
	return componentsById;
}();

const std::unordered_map<std::string_view, const ComponentInfo*> ComponentsByName = []() {
	std::unordered_map<std::string_view, const ComponentInfo*> componentsByName;
	for (auto& info : Components) {
		componentsByName[info.name] = &info;
	}
	return componentsByName;
}();

std::span<const ComponentInfo> components()
{
	return Components;
}

const ComponentInfo* componentById(entt::id_type id)
{
	if (auto it = ComponentsById.find(id); it != ComponentsById.end()) {
		return it->second;
	} else {
		return nullptr;
	}
}

const ComponentInfo* componentByName(std::string_view name)
{
	if (auto it = ComponentsByName.find(name); it != ComponentsByName.end()) {
		return it->second;
	} else {
		return nullptr;
	}
}

} // namespace Anker
