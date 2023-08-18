#pragma once

namespace Anker {

using ComponentID = entt::id_type;

struct ComponentInfo {
	std::string name;
	int sortOrder = 0;
	ComponentID id = 0;
	bool hideInEditor = false;

	std::function<void(EntityHandle)> addTo;
	std::function<void(EntityHandle)> removeFrom;
	std::function<bool(EntityCHandle)> isPresentIn;
	// std::function<void(JsonReader&, EntityHandle)> fromJson;
	// std::function<void(JsonWriter&, EntityCHandle)> toJson;
	std::function<void(EditWidgetDrawer&, EntityHandle)> drawEditWidget;
};

// The component registry holds descriptions for component types. These
// descriptions include serialization operations among other meta data and
// operators.
//
// A component that is not registered here can still be used with the ECS.
// However, certain features like serialization and editor support won't be
// present.
//
// Generally, a system should register its related components here on
// construction.
class ComponentRegistry {
  public:
	ComponentRegistry();
	ComponentRegistry(const ComponentRegistry&) = delete;
	ComponentRegistry& operator=(const ComponentRegistry&) = delete;
	ComponentRegistry(ComponentRegistry&&) noexcept = delete;
	ComponentRegistry& operator=(ComponentRegistry&&) noexcept = delete;

	template <typename Component>
	void registerComponent(std::string_view name, int sortOrder = 0)
	{
		ComponentInfo info{
		    .name = {name.data(), name.size()},
		    .sortOrder = sortOrder,
		    .id = entt::type_hash<Component>(),

		    .addTo = [](EntityHandle entity) { entity.emplace<Component>(); },
		    .removeFrom = [](EntityHandle entity) { entity.remove<Component>(); },
		    .isPresentIn = [](EntityCHandle entity) { return entity.try_get<Component>(); },
		};

#if 0
		if constexpr (Serializable<JsonReader, Component>) {
			info.fromJson = [](JsonReader& reader, EntityHandle entity) {
				if (auto* component = entity.try_get<Component>()) {
					reader(*component);
				}
			};
		}
		if constexpr (Serializable<JsonWriter, Component>) {
			info.toJson = [](JsonWriter& writer, EntityCHandle entity) {
				if (auto* component = entity.try_get<Component>()) {
					writer(*component);
				}
			};
		}

#endif
		if constexpr (refl::is_reflectable<Component>()) {
			info.hideInEditor = has_attribute<attr::Hidden>(refl::reflect<Component>());
		}
		if constexpr (Serializable<EditWidgetDrawer, Component>) {
			info.drawEditWidget = [](EditWidgetDrawer& draw, EntityHandle entity) {
				if (auto* component = entity.try_get<Component>()) {
					draw(*component);
				}
			};
		}

		addComponentInfo(info);
	}

	const auto& components() const { return m_sortedInfos; }
	const ComponentInfo* componentByID(entt::id_type) const;
	const ComponentInfo* componentByName(std::string_view) const;

  private:
	void addComponentInfo(const ComponentInfo&);

	std::vector<std::unique_ptr<ComponentInfo>> m_infos;
	std::vector<const ComponentInfo*> m_sortedInfos;
	std::unordered_map<entt::id_type, const ComponentInfo*> m_lookupByID;
	std::unordered_map<std::string_view, const ComponentInfo*> m_lookupByName;
};

} // namespace Anker
