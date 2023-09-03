#pragma once

#include <anker/editor/anker_editor_camera.hpp>
#include <anker/editor/anker_inspector.hpp>

namespace Anker {

class Scene;

// The EditorFramework serves as a framework for all editor-related systems. It
// is initialized after Engine and therefore uses the global Engine instance.
class EditorFramework {
  public:
	EditorFramework() = default;
	EditorFramework(const EditorFramework&) = delete;
	EditorFramework& operator=(const EditorFramework&) = delete;
	EditorFramework(EditorFramework&&) noexcept = delete;
	EditorFramework& operator=(EditorFramework&&) noexcept = delete;

	void tick(float, Scene&);

  private:
	Inspector m_inspector;
	EditorCameraSystem m_cameraSystem;
};

} // namespace Anker
