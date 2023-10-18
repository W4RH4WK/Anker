#pragma once

namespace Anker {

enum PhysicsLayers : u16 {
	Default = 1 << 0,
	Map = 1 << 1,
	MapPlatforms = 1 << 2,
	Player = 1 << 3,
};

} // namespace Anker
