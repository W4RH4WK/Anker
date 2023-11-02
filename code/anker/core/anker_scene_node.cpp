#include <anker/core/anker_scene_node.hpp>

#include <anker/editor/anker_inspector_widget_drawer.hpp>

namespace Anker {

SceneNode::SceneNode(const Transform2D& localTransform)
{
	setLocalTransform(localTransform);
}

SceneNode::SceneNode(const Transform2D& localTransform, SceneNode* parent) : SceneNode(localTransform)
{
	setParent(parent);
}

SceneNode::~SceneNode()
{
	// On component destruction, we forward our children to our parent. Here we
	// need a copy of m_children as it will be mutated by setParent.
	for (auto* child : std::vector<SceneNode*>(m_children)) {
		child->setParent(m_parent);
	}

	clearParent();
}

void SceneNode::setParent(SceneNode* newParent)
{
	if (newParent == m_parent) {
		return;
	}

	invalidateCachedParentTransform();

	// Check if the new parent is one of our children. If so, we re-parent that
	// node to our current parent.
	for (SceneNode* node = newParent; node; node = node->m_parent) {
		if (node == this) {
			newParent->setParent(m_parent);
			break;
		}
	}

	// Remove from original parent's children.
	if (m_parent) {
		if (auto it = std::ranges::find(m_parent->m_children, this); it != m_parent->m_children.end()) {
			m_parent->m_children.erase(it);
		} else {
			ANKER_ERROR("Parent-child-invariant broken!");
		}
	}

	m_parent = newParent;

	// Add to new parent's children
	if (m_parent) {
		m_parent->m_children.push_back(this);
	}

#if ANKER_CHECK_SCENE_NODE_INVARIANT_ENABLED
	if (!validateParentChildLink()) {
		ANKER_ERROR("Broken SceneNode invariant on {}", name());
	}
#endif
}

bool SceneNode::validateParentChildLink() const
{
	// Check link with parent.
	if (m_parent) {
		if (std::ranges::find(m_parent->m_children, this) == m_parent->m_children.end()) {
			ANKER_ERROR("Node not linked with parent");
			return false;
		}
	}

	// Check links with children.
	for (auto* child : m_children) {
		if (child->m_parent != this) {
			ANKER_ERROR("Child not linked with node");
			return false;
		}
	}

	// Check for unwanted loop.
	for (auto* node = m_parent; node; node = node->m_parent) {
		if (node == this) {
			ANKER_ERROR("Nodes build a loop");
			return false;
		}
	}

	return true;
}

// Links an entity with its corresponding SceneNode. This function is used
// automatically by the registry using the provide callback mechanism.
void linkSceneNodeWithEntity(entt::registry& reg, entt::entity entity)
{
	reg.get<SceneNode>(entity).m_entity = {reg, entity};
}

void registerSceneNodeCallbacks(entt::registry& reg)
{
	reg.on_construct<SceneNode>().connect<&linkSceneNodeWithEntity>();
	reg.on_update<SceneNode>().connect<&linkSceneNodeWithEntity>();
}

void unregisterSceneNodeCallbacks(entt::registry& reg)
{
	reg.on_construct<SceneNode>().disconnect<&linkSceneNodeWithEntity>();
	reg.on_update<SceneNode>().disconnect<&linkSceneNodeWithEntity>();
}

bool serialize(InspectorWidgetDrawer& draw, SceneNode& node)
{
	auto transform = node.localTransform();
	if (draw(transform)) {
		node.setLocalTransform(transform);
		return true;
	}
	return false;
}

} // namespace Anker
