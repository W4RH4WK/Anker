#pragma once

namespace Anker {

using RenderLayer = int32_t;

constexpr RenderLayer LayerDefault = 0;
constexpr RenderLayer LayerPlayer = 1;
constexpr RenderLayer LayerMapBackgroundEnd = -100;
constexpr RenderLayer LayerMapForegroundStart = 100;

} // namespace Anker
