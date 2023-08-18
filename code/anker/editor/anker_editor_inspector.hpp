#pragma once

namespace Anker {

class Scene;

class EditorInspector {
  public:
	EditorInspector() = default;
	EditorInspector(const EditorInspector&) = delete;
	EditorInspector& operator=(const EditorInspector&) = delete;
	EditorInspector(EditorInspector&&) noexcept = delete;
	EditorInspector& operator=(EditorInspector&&) noexcept = delete;

	void tick(float, Scene&);

	void drawMenuBarEntry();

  private:
	bool m_enabled = true;

	std::optional<EntityId> m_selectedEntity;
};

} // namespace Anker
