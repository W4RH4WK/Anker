#include "anker_renderer.hpp"

namespace Anker {

struct SceneConstantBuffer {
	float aspectRatio = 16.0f / 9.0f;
	float pad[3]{};
};
static_assert(sizeof(SceneConstantBuffer) % 16 == 0, "Constant Buffer size must be 16-byte aligned");

Renderer::Renderer(RenderDevice& renderDevice, AssetCache& assetCache)
    : m_renderDevice(renderDevice), m_spriteRenderer(renderDevice, assetCache)
{
	m_sceneConstantBuffer.info = {
	    .name = "Scene Constant Buffer",
	    .size = sizeof(SceneConstantBuffer),
	    .bindFlags = GpuBindFlag::ConstantBuffer,
	    .flags = GpuBufferFlag::CpuWriteable,
	};
	m_renderDevice.createBuffer(m_sceneConstantBuffer);

	m_renderDevice.enableAlphaBlending();
}

void Renderer::draw(const Scene& scene)
{
	ANKER_PROFILE_ZONE();

	{
		Vec2 backBufferSize = m_renderDevice.backBufferSize();
		auto* cb = m_renderDevice.mapBuffer<SceneConstantBuffer>(m_sceneConstantBuffer);
		*cb = SceneConstantBuffer{
			.aspectRatio = backBufferSize.x / backBufferSize.y,
		};
		m_renderDevice.unmapBuffer(m_sceneConstantBuffer);
		m_renderDevice.bindBufferVS(0, m_sceneConstantBuffer);
		m_renderDevice.bindBufferPS(0, m_sceneConstantBuffer);
	}

	m_renderDevice.clearRenderTarget(m_renderDevice.backBuffer());
	m_renderDevice.setRenderTarget(m_renderDevice.backBuffer());

	m_spriteRenderer.draw(scene);
}

} // namespace Anker
