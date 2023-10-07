#pragma once

#include <anker/core/anker_asset.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/editor/anker_edit_widget_drawer.hpp>
#include <anker/graphics/anker_render_device.hpp>

namespace Anker {

// When loading a map, we insert the map identifier into the Scene's context.
// This allows us to query which map was loaded later on.
struct MapIdentifier {
	std::string identifier;
};

// TODO: Rename TileLayer
struct MapLayer {
	std::string name;
	Vec4 color = Vec4(1);
	Vec2 parallax = Vec2(1);

	GpuBuffer vertexBuffer;
	AssetPtr<Texture> texture;
};

// Load the map with the given identifier and add layers + objects to the given
// scene. Also sets MapIdentifier.
Status addMapToScene(Scene&, std::string_view identifier);

// Creates a new scene with map data added.
ScenePtr loadMap(std::string_view mapIdentifier);

} // namespace Anker

REFL_TYPE(Anker::MapLayer)
REFL_FIELD(name)
REFL_FIELD(color, Anker::attr::Color())
REFL_FIELD(parallax)
REFL_FIELD(texture)
REFL_END
