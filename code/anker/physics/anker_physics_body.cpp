#include <anker/physics/anker_physics_body.hpp>

#include <anker/editor/anker_edit_widget_drawer.hpp>

namespace Anker {

bool serialize(EditWidgetDrawer& draw, PhysicsBody& body)
{
	bool changed = false;

	Vec2 position = body.body->GetPosition();
	float rotation = body.body->GetTransform().q.GetAngle() / Degrees;

	changed |= draw.field("position", position);
	changed |= draw.field("rotation", rotation);

	if (changed) {
		body.body->SetTransform(position, rotation * Degrees);
		body.body->SetAwake(true);
	}

	return changed;
}

} // namespace Anker
