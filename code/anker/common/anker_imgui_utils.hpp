#pragma once

#include "anker_enum_utils.hpp"
#include "anker_math.hpp"
#include "anker_type_utils.hpp"

namespace ImGui {

inline bool ToggleButton(const char* label, bool* v)
{
	if (!*v) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_Border]);
	}
	bool clicked = ImGui::Button(label);
	if (!*v) {
		ImGui::PopStyleColor();
	}
	if (clicked) {
		*v = !*v;
	}
	return clicked;
}

// A wrapper for DragFloat that displays the value in degree.
inline bool DragFloatDegree(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f,
                            const char* format = "%.3f", ImGuiSliderFlags flags = 0)
{
	float v_degrees = *v * Anker::Rad2Deg;
	if (ImGui::DragFloat(label, &v_degrees, v_speed, v_min, v_max, format, flags)) {
		*v = v_degrees * Anker::Deg2Rad;
		return true;
	}
	return false;
}

inline bool InputQuat(const char* label, glm::quat* v)
{
	glm::vec3 angles = degrees(eulerAngles(*v));
	if (ImGui::InputFloat3(label, &angles.x)) {
		*v = glm::quat(radians(angles));
		return true;
	}
	return false;
}

template <typename EnumType>
bool InputEnum(const char* label, EnumType* value, Anker::EnumEntries<EnumType> entries) //
    requires std::is_enum_v<EnumType>
{
	bool changed = false;
	if (ImGui::BeginCombo(label, to_string(*value))) {
		for (auto& [entry, entryString] : entries) {
			bool isSelected = *value == entry;
			if (ImGui::Selectable(entryString, isSelected)) {
				*value = entry;
				changed = true;
			}

			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	return changed;
}

} // namespace ImGui
