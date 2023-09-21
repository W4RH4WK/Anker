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
	// We enforce node pointers to be const as we must not modify the scene
	// graph during traversal. See m_sceneGraphModification below.
	void drawSceneNodeRecursive(const SceneNode*);

	void drawNameWidget(EntityHandle entity);
	void drawAddComponentButton(EntityHandle entity);
	void drawComponentEditor(EntityHandle);

	void drawSelectionGizmo(EntityCHandle);

	bool m_enabled = false;

	EntityID m_selectedEntity = entt::null;

	// User interaction is processed while traversing the scene graph; however,
	// we must not modify the scene graph during traversal. We record the
	// desired operation and execute it after graph traversal. const_cast may be
	// used here if needed.
	std::function<void()> m_sceneGraphModification;
};

} // namespace Anker
