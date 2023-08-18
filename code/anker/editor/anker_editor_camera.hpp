#pragma once

namespace Anker {

struct EditorCamera {
	float speed = 0.01f;
};

class Scene;

class EditorCameraSystem {
  public:
	EditorCameraSystem() = default;
	EditorCameraSystem(const EditorCameraSystem&) = delete;
	EditorCameraSystem& operator=(const EditorCameraSystem&) = delete;
	EditorCameraSystem(EditorCameraSystem&&) noexcept = delete;
	EditorCameraSystem& operator=(EditorCameraSystem&&) noexcept = delete;

	void tick(float, Scene&);

	void drawMenuBarEntry(Scene&);
};

} // namespace Anker
