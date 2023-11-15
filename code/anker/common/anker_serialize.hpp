#pragma once

namespace Anker {

template <typename Archive, typename T>
concept Serializable = requires(Archive archive, T v)
{
	{archive(v)};
};

namespace Internal {

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

} // namespace Internal

namespace Attr {

// Prevents the type, field, or property to show up in the editor.
struct Hidden : refl::attr::usage::type,  //
                refl::attr::usage::field, //
                refl::attr::usage::function {};

// Displays the value in degree and converts input back to radians.
struct Radians : refl::attr::usage::field, //
                 refl::attr::usage::function {};

// Field or property is not automatically surrounded with a tree widget.
struct Inline : refl::attr::usage::field, //
                refl::attr::usage::function {};

// Adds a color preview + picker.
struct Color : refl::attr::usage::field, //
               refl::attr::usage::function {};

// Uses a slider widget instead of the regular drag widget.
template <typename T>
struct Slider : refl::attr::usage::field, //
                refl::attr::usage::function {
	constexpr Slider(T min, T max) : min(min), max(max) {}
	T min;
	T max;
};

} // namespace Attr
} // namespace Anker
