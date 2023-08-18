#pragma once

#include <anker/editor/anker_edit_widget_drawer.hpp>

namespace Anker {

struct EntityName {
	std::string name;
};

} // namespace Anker

REFL_TYPE(Anker::EntityName, Anker::attr::Hidden())
REFL_FIELD(name)
REFL_END

namespace Anker {

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
