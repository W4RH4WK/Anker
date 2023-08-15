#include <anker/graphics/anker_screen_renderer.hpp>

#include <anker/core/anker_asset_cache.hpp>

namespace Anker {

ScreenRenderer::ScreenRenderer(RenderDevice& renderDevice, AssetCache& assetCache) : m_renderDevice(renderDevice)
{
	m_vertexShader = assetCache.loadVertexShader("shaders/screen.vs", {});
}

void ScreenRenderer::draw()
{
	m_renderDevice.bindVertexShader(*m_vertexShader);
	m_renderDevice.draw(3);
}

} // namespace Anker
