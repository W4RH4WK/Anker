#pragma once

#include <anker/editor/anker_editor_camera.hpp>
#include <anker/editor/anker_inspector.hpp>

namespace Anker {

class Scene;

// The EditorSystem serves as a framework for all editor-related systems. It is
// initialized after Engine and therefore uses the global Engine instance.
class EditorSystem {
  public:
	EditorSystem() = default;
	EditorSystem(const EditorSystem&) = delete;
	EditorSystem& operator=(const EditorSystem&) = delete;
	EditorSystem(EditorSystem&&) noexcept = delete;
	EditorSystem& operator=(EditorSystem&&) noexcept = delete;

	void tick(float, Scene&);

  private:
	Inspector m_inspector;
	EditorCameraSystem m_cameraSystem;
};

} // namespace Anker
