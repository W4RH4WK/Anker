#pragma once

namespace Anker {

struct PhysicsBody {
	b2Body* body;
};

class EditWidgetDrawer;
bool serialize(EditWidgetDrawer&, PhysicsBody&);

} // namespace Anker

REFL_TYPE(Anker::PhysicsBody)
REFL_END
