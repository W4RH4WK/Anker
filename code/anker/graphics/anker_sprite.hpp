#pragma once

#include <anker/core/anker_asset.hpp>
#include <anker/editor/anker_edit_widget_drawer.hpp>
#include <anker/graphics/anker_render_layers.hpp>

namespace Anker {

struct Texture;

struct Sprite {
	RenderLayer layer = LayerDefault;
	Vec4 color = Vec4(1);
	Vec2 offset;
	float pixelToMeter = 32.0f;
	AssetPtr<Texture> texture;
	Rect2 textureRect = Rect2::fromPoints({0, 0}, {1, 1});
};

} // namespace Anker

REFL_TYPE(Anker::Sprite)
REFL_FIELD(layer)
REFL_FIELD(color, Anker::attr::Color())
REFL_FIELD(offset)
REFL_FIELD(pixelToMeter)
REFL_FIELD(texture, Anker::attr::Inline())
REFL_END
