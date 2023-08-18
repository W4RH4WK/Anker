#pragma once

namespace Anker {

struct EditorCamera {
	float speed = 0.01f;
};

class Scene;
class InputSystem;

class EditorCameraSystem {
  public:
	EditorCameraSystem(InputSystem&);
	EditorCameraSystem(const EditorCameraSystem&) = delete;
	EditorCameraSystem& operator=(const EditorCameraSystem&) = delete;
	EditorCameraSystem(EditorCameraSystem&&) noexcept = delete;
	EditorCameraSystem& operator=(EditorCameraSystem&&) noexcept = delete;

	void tick(float, Scene&);

	void drawMenuBarEntry(Scene&);

  private:
	InputSystem& m_inputSystem;
};

} // namespace Anker
