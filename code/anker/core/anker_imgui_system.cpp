#include <anker/core/anker_imgui_system.hpp>

#include <anker/graphics/anker_render_device.hpp>
#include <anker/platform/anker_platform_win32.hpp>

namespace Anker {

ImguiSystem::ImguiSystem(RenderDevice& renderDevice)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	g_platform->imguiImplInit();
	ImGui_ImplDX11_Init(renderDevice.device(), renderDevice.context());
}

ImguiSystem::~ImguiSystem()
{
	ImGui_ImplDX11_Shutdown();
	g_platform->imguiImplShutdown();
	ImGui::DestroyContext();
}

void ImguiSystem::newFrame()
{

	g_platform->imguiImplNewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();
}

void ImguiSystem::draw()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

} // namespace Anker
