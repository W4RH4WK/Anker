#pragma once

namespace Anker {

class Scene;
class SceneNode;

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
	void drawSceneNodeRecursive(SceneNode*);
	void drawNameWidget(EntityHandle entity);
	void drawAddComponentButton(EntityHandle entity);
	void drawComponentEditor(EntityHandle);

	void drawSelectionGizmo(EntityCHandle);

	bool m_enabled = false;

	std::optional<EntityID> m_selectedEntity;

	// The Inspector can be used to re-parent SceneNodes. However, this must not
	// be done during SceneNode traversal. We record a list of wanted
	// re-parenting operations and execute them after traversing all SceneNodes.
	std::vector<std::pair<SceneNode*, SceneNode*>> m_reparentList;
};

} // namespace Anker
