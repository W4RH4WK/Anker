#pragma once

#include <anker/editor/anker_edit_widget_drawer.hpp>

namespace Anker {

enum class ToneMapping {
#define ANKER_ENUM_ToneMapping \
	ANKER_ENUM_E(None) \
	ANKER_ENUM_E(Uncharted2) \
	ANKER_ENUM_E(ACES)
#define ANKER_ENUM_E(_entry) _entry,
	ANKER_ENUM_ToneMapping
#undef ANKER_ENUM_E
};
constexpr std::array ToneMappingEntries{
#define ANKER_ENUM_E(_entry) std::pair{ToneMapping::_entry, #_entry},
    ANKER_ENUM_ToneMapping
#undef ANKER_ENUM_E
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
	float gamma = 2.2f;
	ToneMapping toneMapping = ToneMapping::None;
	float pad0[1];
};
static_assert(sizeof(PostProcessParams) % 16 == 0, "Constant Buffer size must be 16-byte aligned");

} // namespace Anker

REFL_TYPE(Anker::PostProcessParams)
REFL_FIELD(exposure, Anker::attr::Slider(0.0f, 3.0f))
REFL_FIELD(temperature, Anker::attr::Slider(-2.0f, 2.0f))
REFL_FIELD(tint, Anker::attr::Slider(-2.0f, 2.0f))
REFL_FIELD(contrast, Anker::attr::Slider(0.0f, 2.0f))
REFL_FIELD(brightness, Anker::attr::Slider(-1.0f, 1.0f))
REFL_FIELD(colorFilter, Anker::attr::Color())
REFL_FIELD(saturation, Anker::attr::Slider(0.0f, 3.0f))
REFL_FIELD(gamma, Anker::attr::Slider(0.0f, 5.0f))
REFL_FIELD(toneMapping, Anker::EnumAttr(Anker::ToneMappingEntries))
REFL_END
