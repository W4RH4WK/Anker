#pragma once

#include <anker/core/anker_asset.hpp>
#include <anker/graphics/anker_render_device.hpp>
#include <anker/graphics/anker_render_layers.hpp>

namespace Anker {

class AssetCache;
class Scene;

struct MapLayer {
	std::string name;
	RenderLayer layer = LayerDefault;

	uint32_t vertexCount = 0;
	GpuBuffer vertexBuffer;
	AssetPtr<Texture> texture;
};

// Load the map with the given identifier and add layers + objects to the given
// scene.
Status loadMap(Scene&, std::string_view identifier, AssetCache&);

} // namespace Anker

REFL_TYPE(Anker::MapLayer)
REFL_FIELD(name)
REFL_FIELD(layer)
REFL_FIELD(texture)
REFL_END
