#pragma once

namespace Anker {

template <typename Archive, typename T>
concept Serializable = requires(Archive archive, T v)
{
	{archive(v)};
};

namespace internal {

// Serialization mechanisms allow for customization by providing a serialize
// function for a given Archive. This trait is used to check for such
// customizations.
template <typename Archive, typename T>
concept CustomSerialization = requires(Archive archive, T v)
{
	{serialize(archive, v)};
};

// While fundamental types are handled by the archive, class types can be
// serialized either by reflection or when a serialize function is provided.
template <typename Archive, typename T>
concept SerializableClass = std::is_class_v<T> && (CustomSerialization<Archive, T> || refl::is_reflectable<T>());

} // namespace internal
} // namespace Anker
