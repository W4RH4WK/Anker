#include <anker/editor/anker_edit_widget_drawer.hpp>

#include <anker/graphics/anker_render_device.hpp>

namespace Anker {

bool serialize(EditWidgetDrawer&, AssetPtr<VertexShader>& vertexShader)
{
	if (vertexShader) {
		ImGui::Text("VertexShader: %s", vertexShader->info.name.c_str());
	} else {
		ImGui::Text("No VertexShader");
	}
	return false;
}

bool serialize(EditWidgetDrawer&, AssetPtr<PixelShader>& pixelShader)
{
	if (pixelShader) {
		ImGui::Text("PixelShader: %s", pixelShader->info.name.c_str());
	} else {
		ImGui::Text("No PixelShader");
	}
	return false;
}

bool serialize(EditWidgetDrawer&, AssetPtr<Texture>& texture)
{
	if (texture) {
		ImGui::Text("Texture: %s", texture->info.name.c_str());
	} else {
		ImGui::Text("No Texture");
	}
	return false;
}

} // namespace Anker
