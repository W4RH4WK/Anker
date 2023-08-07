#pragma once

#include <anker/core/anker_asset.hpp>

namespace Anker {

struct Texture;

struct Sprite {
	AssetPtr<Texture> texture;
};

} // namespace Anker
