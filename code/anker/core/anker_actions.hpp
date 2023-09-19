#pragma once

namespace Anker {

struct Actions {
	Vec2 playerMove;

	bool editorCameraActivate = false;
	Vec2 editorCameraPan;
	float editorCameraZoom = 0;
};

} // namespace Anker
