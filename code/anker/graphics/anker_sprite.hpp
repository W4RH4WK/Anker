#pragma once

#include <anker/core/anker_asset.hpp>

namespace Anker {

struct Texture;

struct Sprite {
	AssetPtr<Texture> texture;
	float pixelToMeter = 32.0f;
};

} // namespace Anker

REFL_TYPE(Anker::Sprite)
REFL_FIELD(texture, Anker::attr::Inline())
REFL_FIELD(pixelToMeter)
REFL_END
