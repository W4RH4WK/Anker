#pragma once

#include <anker/core/anker_asset.hpp>
#include <anker/editor/anker_edit_widget_drawer.hpp>
#include <anker/graphics/anker_render_device.hpp>

namespace Anker {

class AssetCache;
class Scene;

struct MapLayer {
	std::string name;
	Vec4 color = Vec4(1);
	Vec2 parallax = Vec2(1);

	GpuBuffer vertexBuffer;
	AssetPtr<Texture> texture;
};

// Load the map with the given identifier and add layers + objects to the given
// scene.
Status loadMap(Scene&, std::string_view identifier, AssetCache&);

} // namespace Anker

REFL_TYPE(Anker::MapLayer)
REFL_FIELD(name)
REFL_FIELD(color, Anker::attr::Color())
REFL_FIELD(parallax)
REFL_FIELD(texture)
REFL_END
