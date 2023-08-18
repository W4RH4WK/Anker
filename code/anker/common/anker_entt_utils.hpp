#pragma once

namespace Anker {

// EntityId is essentially just an index into the ECS registry. When you need to
// store an entity, store this id.
using EntityId = entt::entity;

// EntityHandle combines an EntityID with a reference to the corresponding ECS
// registry. This allows you to manage components more conveniently. Do not
// store EntityHandle (or EntityCHandle) because of the reference. EntityHandle
// is meant to be short-lived.
using EntityHandle = entt::handle;
using EntityCHandle = entt::const_handle;

} // namespace Anker
