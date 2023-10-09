#pragma once

#include <anker/core/anker_asset.hpp>
#include <anker/editor/anker_edit_widget_drawer.hpp>
#include <anker/graphics/anker_render_device.hpp>

namespace Anker {

struct TileLayer {
	std::string name;
	Vec4 color = Vec4(1);
	Vec2 parallax = Vec2(1);

	GpuBuffer vertexBuffer;
	AssetPtr<Texture> texture;
};

} // namespace Anker

REFL_TYPE(Anker::TileLayer)
REFL_FIELD(name)
REFL_FIELD(color, Anker::attr::Color())
REFL_FIELD(parallax)
REFL_FIELD(texture)
REFL_END
