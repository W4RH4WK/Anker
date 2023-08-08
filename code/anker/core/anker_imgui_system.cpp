#include "anker_imgui_system.hpp"

#include <anker/graphics/anker_render_device.hpp>
#include <anker/platform/anker_window_win32.hpp>

namespace Anker {

ImguiSystem::ImguiSystem(Window& window, RenderDevice& renderDevice)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOther(window, true);
	ImGui_ImplDX11_Init(renderDevice.device(), renderDevice.context());
}

ImguiSystem::~ImguiSystem()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImguiSystem::newFrame()
{
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();
}

void ImguiSystem::draw()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

} // namespace Anker
