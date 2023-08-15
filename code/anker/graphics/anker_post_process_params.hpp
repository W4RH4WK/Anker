#pragma once

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
