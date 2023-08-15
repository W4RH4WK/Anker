#include "anker_post_process_renderer.hpp"

#include <anker/core/anker_asset_cache.hpp>

namespace Anker {

PostProcessRenderer::PostProcessRenderer(RenderDevice& renderDevice, AssetCache& assetCache)
    : ScreenRenderer(renderDevice, assetCache), m_renderDevice(renderDevice)
{
	m_constantBuffer.info = {
	    .name = "PostProcessing Constant Buffer",
	    .size = sizeof(PostProcessParams),
	    .bindFlags = GpuBindFlag::ConstantBuffer,
	    .flags = GpuBufferFlag::CpuWriteable,
	};
	if (not renderDevice.createBuffer(m_constantBuffer)) {
		ANKER_FATAL("Failed to create PostProcessing Constant Buffer");
	}

	m_pixelShader = assetCache.loadPixelShader("shaders/post_process.ps");
}

void PostProcessRenderer::draw(const PostProcessParams& params)
{
	m_renderDevice.bindPixelShader(*m_pixelShader);

	{
		auto* cb = m_renderDevice.mapBuffer<PostProcessParams>(m_constantBuffer);
		*cb = params;
		m_renderDevice.unmapBuffer(m_constantBuffer);
	}

	m_renderDevice.bindBufferPS(0, m_constantBuffer);

	ScreenRenderer::draw();
}

} // namespace Anker
