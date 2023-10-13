#pragma once

namespace Anker {

class Scene;
class InspectorWidgetDrawer;

enum class ComponentFlag {
	None = 0,
	HideInInspector = 1 << 0,
};
ANKER_ENUM_FLAGS(ComponentFlag)

struct ComponentInfo {
	const char* name = nullptr;
	entt::id_type id = 0;
	ComponentFlags flags;

	void (*tick)(float, Scene&);

	void (*addTo)(EntityHandle);
	void (*removeFrom)(EntityHandle);
	bool (*isPresentIn)(EntityCHandle);
	void (*drawInspectorWidget)(InspectorWidgetDrawer&, EntityHandle);
};

std::span<const ComponentInfo> components();
const ComponentInfo* componentById(entt::id_type);
const ComponentInfo* componentByName(std::string_view);

} // namespace Anker
