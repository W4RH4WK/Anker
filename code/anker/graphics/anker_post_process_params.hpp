#pragma once

#include <anker/editor/anker_inspector_widget_drawer.hpp>

namespace Anker {

enum class ToneMapping { None, Uncharted2, ACES };
constexpr std::array ToneMappingEntries{
    std::pair(ToneMapping::None, "None"),
    std::pair(ToneMapping::Uncharted2, "Uncharted2"),
    std::pair(ToneMapping::ACES, "ACES"),
};
ANKER_ENUM_TO_FROM_STRING(ToneMapping)

////////////////////////////////////////////////////////////

struct PostProcessParams {
	float exposure = 1.0f;
	float temperature = 0.0f;
	float tint = 0.0f;
	float contrast = 1.0f;
	float brightness = 0.0f;
	Vec3 colorFilter = Vec3(1);
	float saturation = 1.0f;
	float gamma = 1.0f;
	ToneMapping toneMapping = ToneMapping::None;
	float pad0[1];
};
static_assert(sizeof(PostProcessParams) % 16 == 0, "Constant Buffer size must be 16-byte aligned");

} // namespace Anker

REFL_TYPE(Anker::PostProcessParams)
REFL_FIELD(exposure, Anker::Attr::Slider(0.0f, 3.0f))
REFL_FIELD(temperature, Anker::Attr::Slider(-2.0f, 2.0f))
REFL_FIELD(tint, Anker::Attr::Slider(-2.0f, 2.0f))
REFL_FIELD(contrast, Anker::Attr::Slider(0.0f, 2.0f))
REFL_FIELD(brightness, Anker::Attr::Slider(-1.0f, 1.0f))
REFL_FIELD(colorFilter, Anker::Attr::Color())
REFL_FIELD(saturation, Anker::Attr::Slider(0.0f, 3.0f))
REFL_FIELD(gamma, Anker::Attr::Slider(0.0f, 5.0f))
REFL_FIELD(toneMapping, Anker::Attr::Enum(Anker::ToneMappingEntries))
REFL_END
