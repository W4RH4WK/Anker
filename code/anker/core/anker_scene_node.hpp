#pragma once

#include <anker/core/anker_transform.hpp>

namespace Anker {

// SceneNode represents a node in the scene graph. Apart from a local Transform,
// it contains a parent pointer and pointer to its children.
//
// Invariants:
// - Parent and child references are kept consistent.
// - Combined parent transforms are cached. This cache is invalided
//   automatically.
class SceneNode {
  public:
	SceneNode() = default;
	explicit SceneNode(const Transform2D& localTransform);
	explicit SceneNode(const Transform2D& localTransform, SceneNode* parent);
	SceneNode(const SceneNode&) = delete;
	SceneNode& operator=(const SceneNode&) = delete;
	SceneNode(SceneNode&&) noexcept = delete;
	SceneNode& operator=(SceneNode&&) noexcept = delete;
	~SceneNode();

	EntityHandle entity() const { return m_entity; }

	const Transform2D& localTransform() const { return m_localTransform; }
	void setLocalTransform(const Transform2D& transform)
	{
		invalidateCachedParentTransformInChildren();
		m_localTransform = transform;
	}

	Transform2D parentTransform() const
	{
		if (!m_cachedParentTransform) {
			m_cachedParentTransform = m_parent ? m_parent->localTransform() : Transform2D{};
		}
		return *m_cachedParentTransform;
	}

	Transform2D globalTransform() const { return parentTransform() * m_localTransform; }
	void setGlobalTransform(const Transform2D& globalTransform)
	{
		setLocalTransform(inverse(parentTransform()) * globalTransform);
	}

	SceneNode* parent() const { return m_parent; }
	void clearParent() { setParent(nullptr); }
	void setParent(SceneNode*);

	bool hasParent() const { return m_parent; }

	std::span<SceneNode* const> children() { return m_children; }
	std::span<const SceneNode* const> children() const { return m_children; }

	void addChild(SceneNode* child) { child->setParent(this); }
	void removeChild(SceneNode* child)
	{
		if (child->parent() == this) {
			child->clearParent();
		}
	}

	bool validateParentChildLink() const;

  private:
	void invalidateCachedParentTransform()
	{
		m_cachedParentTransform.reset();
		invalidateCachedParentTransformInChildren();
	}

	void invalidateCachedParentTransformInChildren()
	{
		for (auto* child : m_children) {
			child->invalidateCachedParentTransform();
		}
	}

	Transform2D m_localTransform;
	mutable std::optional<Transform2D> m_cachedParentTransform;

	EntityHandle m_entity;

	SceneNode* m_parent = nullptr;
	std::vector<SceneNode*> m_children;

	friend void linkSceneNodeWithEntity(entt::registry&, entt::entity);
};

void registerSceneNodeCallbacks(entt::registry&);
void unregisterSceneNodeCallbacks(entt::registry&);

bool serialize(EditWidgetDrawer&, SceneNode&);

} // namespace Anker

// Ensure components are not relocated in memory. This allows us to use raw
// pointers for them.
template <>
struct entt::component_traits<Anker::SceneNode> {
	using type = Anker::SceneNode;
	static constexpr bool in_place_delete = true;
	static constexpr std::size_t page_size = entt::internal::page_size<Anker::SceneNode>::value;
};
