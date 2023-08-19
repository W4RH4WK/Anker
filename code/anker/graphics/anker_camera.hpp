#pragma once

#include <anker/graphics/anker_post_process_params.hpp>

namespace Anker {

struct Camera {
	float distance = 10.0f;
	PostProcessParams postProcessParams;
};

} // namespace Anker

REFL_TYPE(Anker::Camera)
REFL_FIELD(distance)
REFL_FIELD(postProcessParams)
REFL_END
