#include <anker/core/anker_imgui_system.hpp>

#include <anker/graphics/anker_render_device.hpp>
#include <anker/platform/anker_platform.hpp>

namespace Anker {

ImguiSystem::ImguiSystem(RenderDevice& renderDevice) : m_renderDevice(renderDevice)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	Platform::imguiImplInit();
	renderDevice.imguiImplInit();
}

ImguiSystem::~ImguiSystem()
{
	m_renderDevice.imguiImplShutdown();
	Platform::imguiImplShutdown();
	ImGui::DestroyContext();
}

void ImguiSystem::newFrame()
{
	Platform::imguiImplNewFrame();
	m_renderDevice.imguiImplNewFrame();
	ImGui::NewFrame();
}

void ImguiSystem::draw()
{
	ImGui::Render();
	m_renderDevice.imguiImplRender();
}

} // namespace Anker
