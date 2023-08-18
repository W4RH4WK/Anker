#pragma once

namespace Anker {

struct EntityName {
	std::string name;
};

inline std::string entityDisplayName(EntityHandle entity)
{
	if (auto* name = entity.try_get<EntityName>()) {
		return name->name.c_str();
	} else {
		return fmt::format("entity {}", entt::to_integral(entity.entity()));
	}
}

inline std::string entityLabel(EntityHandle entity)
{
	return fmt::format("{}##entity{}", entityDisplayName(entity), entt::to_integral(entity.entity()));
}

} // namespace Anker
