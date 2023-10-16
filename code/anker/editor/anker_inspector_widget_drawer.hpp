#pragma once

#include <anker/core/anker_asset.hpp>

namespace Anker {

// Draws an ImGui edit widget for the given object. Drawing of types can be
// customized by providing a serialize function. Reflection is taken into
// account.
class InspectorWidgetDrawer {
  public:
	bool field(const char* name, bool& value) { return ImGui::Checkbox(name, &value); }

	bool field(const char* name, int& value) { return ImGui::DragInt(name, &value); }

	template <typename ReflDescriptor>
	bool field(ReflDescriptor member, const char* name, int& value)
	{
		if constexpr (has_attribute<Attr::Slider>(member)) {
			auto attr = get_attribute<Attr::Slider>(member);
			return ImGui::SliderInt(name, &value, attr.min, attr.max);
		} else {
			return field(name, value);
		}
	}

	bool field(const char* name, float& value) { return ImGui::DragFloat(name, &value); }

	bool fieldAsSlider(const char* name, float& value, float min, float max)
	{
		return ImGui::SliderFloat(name, &value, min, max);
	}

	bool fieldAsDegree(const char* name, float& value)
	{
		float degrees = value * Rad2Deg;
		if (ImGui::DragFloat(name, &degrees)) {
			value = degrees * Deg2Rad;
			return true;
		} else {
			return false;
		}
	}

	template <typename ReflDescriptor>
	bool field(ReflDescriptor member, const char* name, float& value)
	{
		if constexpr (has_attribute<Attr::Radians>(member)) {
			return fieldAsDegree(name, value);
		} else if constexpr (has_attribute<Attr::Slider>(member)) {
			auto attr = get_attribute<Attr::Slider>(member);
			return fieldAsSlider(name, value, attr.min, attr.max);
		} else {
			return field(name, value);
		}
	}

	bool field(const char* name, Vec2& value) { return ImGui::InputFloat2(name, &value.x); }

	bool field(const char* name, Vec3& value) { return ImGui::InputFloat3(name, &value.x); }

	bool fieldAsColor(const char* name, Vec3& value) { return ImGui::ColorEdit3(name, &value.x); }

	template <typename ReflDescriptor>
	bool field(ReflDescriptor member, const char* name, Vec3& value)
	{
		if constexpr (has_attribute<Attr::Color>(member)) {
			return fieldAsColor(name, value);
		} else {
			return field(name, value);
		}
	}

	bool field(const char* name, Vec4& value) { return ImGui::InputFloat4(name, &value.x); }

	bool fieldAsColor(const char* name, Vec4& value) { return ImGui::ColorEdit4(name, &value.x); }

	template <typename ReflDescriptor>
	bool field(ReflDescriptor member, const char* name, Vec4& value)
	{
		if constexpr (has_attribute<Attr::Color>(member)) {
			return fieldAsColor(name, value);
		} else {
			return field(name, value);
		}
	}

	bool field(const char* name, Quat& value) { return ImGui::InputQuat(name, &value); }

	bool field(const char* name, std::string& value)
	{
		return ImGui::InputText(name, &value, ImGuiInputTextFlags_EnterReturnsTrue);
	}

	bool field(const char* name, EntityID& entity)
	{
		u32 value = entt::to_integral(entity);
		if (ImGui::InputScalar(name, ImGuiDataType_U32, &value)) {
			entity = EntityID(value);
			return true;
		} else {
			return false;
		}
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
		if constexpr (has_attribute<Attr::Enum<EnumType>>(member)) {
			return field(name, value, get_attribute<Attr::Enum>(member).entries);
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
			    [&]<typename T>(T& alternative) requires Internal::SerializableClass<InspectorWidgetDrawer, T> {
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
	    requires Internal::SerializableClass<InspectorWidgetDrawer, T>
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
	    requires Internal::SerializableClass<InspectorWidgetDrawer, T>
	{
		return field(name, object, has_attribute<Attr::Inline>(member));
	}

	template <typename T>
	bool operator()(T& object) requires Internal::SerializableClass<InspectorWidgetDrawer, T>
	{
		if constexpr (Internal::CustomSerialization<InspectorWidgetDrawer, T>) {
			return serialize(*this, object);
		}

		else if constexpr (refl::is_reflectable<T>()) {
			bool changed = false;

			// Only consider members without the Hidden attribute.
			auto members = filter(refl::reflect<T>().members, [](auto member) { //
				return !has_attribute<Attr::Hidden>(member);
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
};

////////////////////////////////////////////////////////////
// AssetPtr support

struct VertexShader;
struct PixelShader;
struct Texture;

bool serialize(InspectorWidgetDrawer&, AssetPtr<VertexShader>&);
bool serialize(InspectorWidgetDrawer&, AssetPtr<PixelShader>&);
bool serialize(InspectorWidgetDrawer&, AssetPtr<Texture>&);

} // namespace Anker
