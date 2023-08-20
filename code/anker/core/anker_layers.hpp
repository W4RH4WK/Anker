#pragma once

namespace Anker {

enum class Layer {
#define ANKER_ENUM_Layer \
	ANKER_ENUM_E(Background, -1) \
	ANKER_ENUM_E(Default, 0) \
	ANKER_ENUM_E(Foreground, 1)
#define ANKER_ENUM_E(_entry, _value) _entry = _value,
	ANKER_ENUM_Layer
#undef ANKER_ENUM_E
};
constexpr std::array LayerEntries{
#define ANKER_ENUM_E(_entry, _value) std::pair{Layer::_entry, #_entry},
    ANKER_ENUM_Layer
#undef ANKER_ENUM_E
};
ANKER_ENUM_TO_FROM_STRING(Layer)

} // namespace Anker
