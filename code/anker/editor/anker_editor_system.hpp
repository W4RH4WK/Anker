#pragma once

#include <anker/editor/anker_editor_camera.hpp>
#include <anker/editor/anker_editor_inspector.hpp>

namespace Anker {

class Engine;
class Scene;

class EditorSystem {
  public:
	EditorSystem(Engine&);
	EditorSystem(const EditorSystem&) = delete;
	EditorSystem& operator=(const EditorSystem&) = delete;
	EditorSystem(EditorSystem&&) noexcept = delete;
	EditorSystem& operator=(EditorSystem&&) noexcept = delete;

	void tick(float, Scene&);

  private:
	Engine& m_engine;

	EditorInspector m_inspector;
	EditorCameraSystem m_cameraSystem;
};

} // namespace Anker
