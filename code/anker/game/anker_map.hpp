#pragma once

#include <anker/core/anker_scene.hpp>

namespace Anker {

// When loading a map, we insert the map identifier into the Scene's context.
// This allows us to query which map was loaded later on.
struct MapIdentifier {
	std::string identifier;
};

// Load the map with the given identifier and add layers + objects to the given
// scene. Also sets MapIdentifier.
Status addMapToScene(Scene&, std::string_view identifier);

// Creates a new scene with map data added.
ScenePtr loadMap(std::string_view mapIdentifier);

} // namespace Anker
