#pragma once

#include <anker/core/anker_asset.hpp>

namespace Anker {

namespace attr {

// Prevents the type, field, or property to show up in the editor.
struct Hidden : refl::attr::usage::type,  //
                refl::attr::usage::field, //
                refl::attr::usage::function {};

// Value is automatically converted to degrees when displayed.
struct Radians : refl::attr::usage::field, //
                 refl::attr::usage::function {};

// Field, or property is not automatically surrounded with a tree widget.
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

} // namespace attr

// Draws an ImGui edit widget for the given object. Drawing of types can be
// customized by providing a serialize function. Reflection is taken into
// account.
class EditWidgetDrawer {
  public:
	bool field(const char* name, bool& value) { return ImGui::Checkbox(name, &value); }

	bool field(const char* name, int& value) { return ImGui::DragInt(name, &value); }

	template <typename ReflDescriptor>
	bool field(ReflDescriptor member, const char* name, int& value)
	{
		if constexpr (has_attribute<attr::Slider>(member)) {
			auto attr = get_attribute<attr::Slider>(member);
			return ImGui::SliderInt(name, &value, attr.min, attr.max);
		} else {
			return field(name, value);
		}
	}

	bool field(const char* name, float& value) { return ImGui::DragFloat(name, &value); }

	template <typename ReflDescriptor>
	bool field(ReflDescriptor member, const char* name, float& value)
	{
		float valueCopy = value;
		if constexpr (has_attribute<attr::Radians>(member)) {
			valueCopy = valueCopy / Degrees;
		}

		bool changed = false;
		if constexpr (has_attribute<attr::Slider>(member)) {
			auto attr = get_attribute<attr::Slider>(member);
			changed = ImGui::SliderFloat(name, &valueCopy, attr.min, attr.max);
		} else {
			changed = field(name, valueCopy);
		}

		if (changed) {
			if constexpr (has_attribute<attr::Radians>(member)) {
				valueCopy = valueCopy * Degrees;
			}
			value = valueCopy;
		}
		return changed;
	}

	bool field(const char* name, Vec2& value) { return ImGui::InputFloat2(name, &value.x); }

	bool field(const char* name, Vec3& value) { return ImGui::InputFloat3(name, &value.x); }

	bool field(const char* name, Vec4& value) { return ImGui::InputFloat4(name, &value.x); }

	template <typename ReflDescriptor>
	bool field(ReflDescriptor member, const char* name, Vec3& value)
	{
		if constexpr (has_attribute<attr::Color>(member)) {
			return ImGui::ColorEdit3(name, value_ptr(value));
		} else {
			return field(name, value);
		}
	}

	template <typename ReflDescriptor>
	bool field(ReflDescriptor member, const char* name, Vec4& value)
	{
		if constexpr (has_attribute<attr::Color>(member)) {
			return ImGui::ColorEdit4(name, value_ptr(value));
		} else {
			return field(name, value);
		}
	}

	bool field(const char* name, Quat& value) { return ImGui::InputQuat(name, &value); }

	bool field(const char* name, std::string& value)
	{
		return ImGui::InputText(name, &value, ImGuiInputTextFlags_EnterReturnsTrue);
	}

	bool field(const char* name, std::string_view& view)
	{
		m_tempString = view;
		bool changed = field(name, m_tempString);
		if (changed) {
			view = m_tempString;
		}
		return changed;
	}

	template <typename EnumType>
	bool field(const char* name, EnumType& value) requires std::is_enum_v<EnumType>
	{
		return ImGui::DragInt(name, reinterpret_cast<std::underlying_type_t<EnumType>*>(&value));
	}

	template <typename EnumType>
	bool field(const char* name, EnumType& value, EnumEntries<EnumType> entries) requires std::is_enum_v<EnumType>
	{
		return ImGui::InputEnum(name, &value, entries);
	}

	template <typename ReflDescriptor, typename EnumType>
	bool field(ReflDescriptor member, const char* name, EnumType& value) requires std::is_enum_v<EnumType>
	{
		if constexpr (has_attribute<EnumAttr<EnumType>>(member)) {
			return field(name, value, get_attribute<EnumAttr>(member).entries);
		} else {
			return field(name, value);
		}
	}

	template <typename... Ts>
	bool field(const char* name, std::variant<Ts...>& value)
	{
		bool changed = false;

		const char* currentTypeName = std::visit([]<typename T>(T&) { return typeName<T>(); }, value);

		if (ImGui::BeginCombo(name, currentTypeName)) {
			forEachType<Ts...>([&]<typename T>(T&) {
				bool isSelected = std::holds_alternative<T>(value);
				if (ImGui::Selectable(typeName<T>(), isSelected)) {
					value = T{};
					changed = true;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			});
			ImGui::EndCombo();
		}

		ImGui::Indent();
		{
			auto alternativeLabel = std::string{"##"} + name;
			auto drawAlternative = overload{
			    // Regular fields are drawn as normal.
			    [&](auto& alternative) { return field(alternativeLabel.c_str(), alternative); },
			    // Objects are drawn in-line.
			    [&]<typename T>(T& alternative) requires internal::SerializableClass<EditWidgetDrawer, T> {
				    return field(alternativeLabel.c_str(), alternative, true);
			    },
			};
			changed |= std::visit(drawAlternative, value);
		}
		ImGui::Unindent();

		return changed;
	}

	template <typename T>
	bool field(const char* name, T& object, bool drawInline = false) //
	    requires internal::SerializableClass<EditWidgetDrawer, T>
	{
		if (drawInline) {
			return (*this)(object);
		}

		else {
			bool changed = false;
			if (ImGui::TreeNodeEx(name, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth)) {
				changed = (*this)(object);
				ImGui::TreePop();
			}
			return changed;
		}
	}

	template <typename ReflDescriptor, typename T>
	bool field(ReflDescriptor member, const char* name, T& object) //
	    requires internal::SerializableClass<EditWidgetDrawer, T>
	{
		return field(name, object, has_attribute<attr::Inline>(member));
	}

	template <typename T>
	bool operator()(T& object) requires internal::SerializableClass<EditWidgetDrawer, T>
	{
		if constexpr (internal::CustomSerialization<EditWidgetDrawer, T>) {
			return serialize(*this, object);
		}

		else if constexpr (refl::is_reflectable<T>()) {
			bool changed = false;

			// Only consider members without the Hidden attribute.
			auto members = filter(refl::reflect<T>().members, [](auto member) { //
				return !has_attribute<attr::Hidden>(member);
			});

			// Fields can be accessed directly via reference.
			auto fields = filter(members, [](auto member) { return is_field(member); });
			for_each(fields, [&](auto member) { //
				changed |= field(member, member.name.c_str(), member(object));
			});

			// For fields accessible via getter/setter (i.e. properties), we
			// acquire a copy of the field via its getter and invoke the setter
			// when the copy was modified by the edit widget.
			auto properties = filter(members, [](auto member) { //
				return is_property(member) && is_writable(member) && has_reader(member);
			});
			for_each(properties, [&](auto member) {
				auto valueCopy = std::invoke(get_reader(member).pointer, object);
				if (field(member, get_display_name(member), valueCopy)) {
					changed = true;
					std::invoke(get_writer(member), object, valueCopy);
				}
			});

			return changed;
		}
	}

  private:
	// Fallback to silently accept all types are not drawable.
	template <typename T>
	bool field(const char*, T&)
	{
		return false;
	}

	// Fallback for fields that do not take advantage of reflection attributes.
	template <typename ReflDescriptor, typename T>
	bool field(ReflDescriptor, const char* label, T& object)
	{
		return field(label, object);
	}

	// Temporary string to support getter/setter using string_view.
	std::string m_tempString;
};

////////////////////////////////////////////////////////////
// AssetPtr support

struct VertexShader;
struct PixelShader;
struct Texture;

bool serialize(EditWidgetDrawer&, AssetPtr<VertexShader>&);
bool serialize(EditWidgetDrawer&, AssetPtr<PixelShader>&);
bool serialize(EditWidgetDrawer&, AssetPtr<Texture>&);

} // namespace Anker
