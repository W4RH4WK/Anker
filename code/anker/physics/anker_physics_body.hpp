#pragma once

#include <anker/core/anker_transform.hpp>

namespace Anker {

struct PhysicsBody {
	Transform2D transform() const { return body->GetTransform(); }
	void setTransform(const Transform2D& transform)
	{
		body->SetTransform(transform.position, transform.rotation);
		body->SetAwake(true);
	}

	b2Body* body = nullptr;
};

} // namespace Anker

REFL_TYPE(Anker::PhysicsBody)
REFL_FUNC(transform, property())
REFL_FUNC(setTransform, property())
REFL_END
