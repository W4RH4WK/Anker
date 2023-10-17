#pragma once

namespace Anker {

inline b2Vec2 normalFromContact(b2Contact* contact, b2Body* body)
{
	b2WorldManifold worldManifest;
	contact->GetWorldManifold(&worldManifest);

	b2Vec2 normal = worldManifest.normal;
	if (contact->GetFixtureB()->GetBody() == body) {
		normal = -normal;
	}

	return normal;
}

} // namespace Anker
