#pragma once

#include <anker/core/anker_asset.hpp>
#include <anker/core/anker_layers.hpp>
#include <anker/editor/anker_edit_widget_drawer.hpp>

namespace Anker {

struct Texture;

struct Sprite {
	AssetPtr<Texture> texture;
	float pixelToMeter = 32.0f;
	Layer layer = Layer(32);
	Vec4 color = Vec4(1);
};

} // namespace Anker

REFL_TYPE(Anker::Sprite)
REFL_FIELD(texture, Anker::attr::Inline())
REFL_FIELD(pixelToMeter)
REFL_FIELD(layer, Anker::EnumAttr(Anker::LayerEntries))
REFL_FIELD(color, Anker::attr::Color())
REFL_END
