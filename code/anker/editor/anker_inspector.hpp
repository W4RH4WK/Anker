#pragma once

namespace Anker {

class Scene;

class Inspector {
  public:
	Inspector() = default;
	Inspector(const Inspector&) = delete;
	Inspector& operator=(const Inspector&) = delete;
	Inspector(Inspector&&) noexcept = delete;
	Inspector& operator=(Inspector&&) noexcept = delete;

	void tick(float, Scene&);

	void drawMenuBarEntry();

  private:
	void drawNameWidget(EntityHandle entity);
	void drawAddComponentButton(EntityHandle entity);
	void drawComponentEditor(EntityHandle);

	void drawSelectionGizmo(EntityCHandle);

	bool m_enabled = false;

	std::optional<EntityID> m_selectedEntity;
};

} // namespace Anker
