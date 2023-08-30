#include <anker/physics/anker_physics_body.hpp>

#include <anker/editor/anker_edit_widget_drawer.hpp>

namespace Anker {

bool serialize(EditWidgetDrawer& draw, PhysicsBody& body)
{
	bool changed = false;

	auto position = as<Vec2>(body.body->GetPosition());
	auto rotation = body.body->GetTransform().q.GetAngle() / Degrees;

	changed |= draw.field("position", position);
	changed |= draw.field("rotation", rotation);

	if (changed) {
		body.body->SetTransform(as<b2Vec2>(position), rotation * Degrees);
		body.body->SetAwake(true);
	}

	return changed;
}

} // namespace Anker
