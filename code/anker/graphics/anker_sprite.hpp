#pragma once

#include <anker/core/anker_asset.hpp>
#include <anker/editor/anker_edit_widget_drawer.hpp>

namespace Anker {

struct Texture;

struct Sprite {
	Vec4 color = Vec4(1);
	Vec2 parallax = Vec2(1);
	Vec2 offset;
	bool flipX = false;
	bool flipY = false;
	float pixelToMeter = 256.0f;
	AssetPtr<Texture> texture;
	Rect2 textureRect = Rect2::fromPoints({0, 0}, {1, 1});
};

} // namespace Anker

REFL_TYPE(Anker::Sprite)
REFL_FIELD(color, Anker::attr::Color())
REFL_FIELD(parallax)
REFL_FIELD(offset)
REFL_FIELD(flipX)
REFL_FIELD(flipY)
REFL_FIELD(pixelToMeter)
REFL_FIELD(texture, Anker::attr::Inline())
REFL_END
