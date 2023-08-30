#include <anker/physics/anker_physics_body.hpp>

#include <anker/editor/anker_edit_widget_drawer.hpp>

namespace Anker {

bool serialize(EditWidgetDrawer& draw, PhysicsBody& body)
{
	bool changed = false;

	auto position = toVec(body.body->GetPosition());
	auto rotation = glm::degrees(body.body->GetTransform().q.GetAngle());

	changed |= draw.field("position", position);
	changed |= draw.field("rotation", rotation);

	if (changed) {
		body.body->SetTransform(toB2Vec(position), glm::radians(rotation));
		body.body->SetAwake(true);
	}

	return changed;
}

} // namespace Anker
