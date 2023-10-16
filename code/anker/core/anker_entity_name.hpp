#pragma once

namespace Anker {

struct EntityName {
	std::string name;
};

} // namespace Anker

REFL_TYPE(Anker::EntityName, Anker::Attr::Hidden())
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

inline std::string entityImGuiLabel(EntityHandle entity)
{
	return fmt::format("{0} ({1})##entity{1}", entityDisplayName(entity), entt::to_integral(entity.entity()));
}

} // namespace Anker
