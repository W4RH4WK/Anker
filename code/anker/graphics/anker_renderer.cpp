#include "anker_renderer.hpp"

#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_transform.hpp>

#include "anker_camera.hpp"

namespace Anker {

struct SceneConstantBuffer {
	Mat4 view = Mat4Id; // Mat4 instead of Mat3 because of alignment
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
	if (not m_renderDevice.createBuffer(m_sceneConstantBuffer)) {
		ANKER_FATAL("Failed to create Scene Constant Buffer");
	}

	m_renderDevice.enableAlphaBlending();
}

void Renderer::draw(const Scene& scene)
{
	ANKER_PROFILE_ZONE();

	if (!scene.activeCamera) {
		ANKER_WARN("No active camera");
		m_renderDevice.clearRenderTarget(m_renderDevice.backBuffer());
		return;
	}

	auto [cameraTransform, cameraParams] = scene.activeCamera.try_get<Transform2D, Camera>();
	if (!cameraTransform || !cameraParams) {
		ANKER_WARN("Invalid active camera");
		m_renderDevice.clearRenderTarget(m_renderDevice.backBuffer());
		return;
	}

	{
		Mat3 view = cameraTransform->mat3();
		view = scale(view, Vec2(cameraParams->distance));

		// Correct for varying aspect ratio. Generally we want to keep the
		// vertical size as is when the window gets wider.
		Vec2 backBufferSize = m_renderDevice.backBuffer().info.size;
		float aspectRatio = backBufferSize.x / backBufferSize.y;
		if (aspectRatio < 1.0f) {
			view = scale(view, {1.0f, 1.0f / aspectRatio});
		} else {
			view = scale(view, {aspectRatio, 1.0f});
		}

		view = inverse(view);

		auto* cb = m_renderDevice.mapBuffer<SceneConstantBuffer>(m_sceneConstantBuffer);
		*cb = SceneConstantBuffer{
		    .view = view,
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
