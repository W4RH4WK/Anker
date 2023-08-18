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
	void drawNameWidget(EntityHandle entity);
	void drawAddComponentButton(EntityHandle entity);
	void drawComponentEditor(EntityHandle);

	bool m_enabled = true;

	std::optional<EntityID> m_selectedEntity;
};

} // namespace Anker
