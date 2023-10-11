#pragma once

#include <anker/core/anker_asset.hpp>
#include <anker/editor/anker_edit_widget_drawer.hpp>
#include <anker/graphics/anker_render_device.hpp>

namespace Anker {

struct TileLayer {
	Vec4 color = Vec4(1);
	Vec2 parallax = Vec2(1);
	std::vector<std::pair<GpuBuffer, AssetPtr<Texture>>> parts;
};

inline bool serialize(EditWidgetDrawer& draw, TileLayer& tileLayer)
{
	bool changed = false;

	changed = draw.fieldAsColor("color", tileLayer.color) || changed;
	changed = draw.field("parallax", tileLayer.parallax) || changed;

	ImGui::Text("Parts: %d", tileLayer.parts.size());
	for (auto [i, part] : iter::enumerate(tileLayer.parts)) {
		ImGui::Text("%2d: Vertices: %d\n    Texture:  %s", i, part.first.info.elementCount(), part.second->info.name.c_str());
	}

	return changed;
}

} // namespace Anker

REFL_TYPE(Anker::TileLayer)
REFL_FIELD(color, Anker::attr::Color())
REFL_FIELD(parallax)
REFL_END
