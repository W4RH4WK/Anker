#include <anker/editor/anker_inspector_widget_drawer.hpp>

#include <anker/graphics/anker_render_device.hpp>

namespace Anker {

bool serialize(InspectorWidgetDrawer&, AssetPtr<VertexShader>& vertexShader)
{
	if (vertexShader) {
		ImGui::Text("VertexShader: %s", vertexShader->info.name.c_str());
	} else {
		ImGui::Text("No VertexShader");
	}
	return false;
}

bool serialize(InspectorWidgetDrawer&, AssetPtr<PixelShader>& pixelShader)
{
	if (pixelShader) {
		ImGui::Text("PixelShader: %s", pixelShader->info.name.c_str());
	} else {
		ImGui::Text("No PixelShader");
	}
	return false;
}

bool serialize(InspectorWidgetDrawer&, AssetPtr<Texture>& texture)
{
	if (texture) {
		ImGui::Text("Texture: %s", texture->info.name.c_str());
	} else {
		ImGui::Text("No Texture");
	}
	return false;
}

} // namespace Anker
