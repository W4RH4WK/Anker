#include <anker/graphics/anker_render_system.hpp>

#include <anker/core/anker_entity_name.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_scene_node.hpp>
#include <anker/game/anker_map.hpp>
#include <anker/graphics/anker_camera.hpp>
#include <anker/graphics/anker_sprite.hpp>

#include <anker/core/anker_engine.hpp>

namespace Anker {

struct SceneConstantBuffer {
	Mat4 view = Mat4Id; // Mat4 instead of Mat3 because of alignment
	Vec2 cameraPosition;
	Vec2 _pad;
};
static_assert(sizeof(SceneConstantBuffer) % 16 == 0, "Constant Buffer size must be 16-byte aligned");

RenderSystem::RenderSystem(RenderDevice& renderDevice, AssetCache& assetCache)
    : gizmoRenderer(renderDevice, assetCache),
      m_renderDevice(renderDevice),
      m_mapRenderer(renderDevice, assetCache),
      m_spriteRenderer(renderDevice, assetCache),
      m_postProcessRenderer(renderDevice, assetCache),
      m_textRenderer(renderDevice, assetCache)
{
	m_sceneConstantBuffer.info = {
	    .name = "Scene Constant Buffer",
	    .size = sizeof(SceneConstantBuffer),
	    .stride = sizeof(SceneConstantBuffer),
	    .bindFlags = GpuBindFlag::ConstantBuffer,
	    .flags = GpuBufferFlag::CpuWriteable,
	};
	if (not m_renderDevice.createBuffer(m_sceneConstantBuffer)) {
		ANKER_FATAL("Failed to create Scene Constant Buffer");
	}

	m_sceneRenderTarget.info = {
	    .name = "Scene Render Target",
	    .size = m_renderDevice.backBuffer().info.size,
	    .format = TextureFormat::R16G16B16A16_UNORM,
	    .bindFlags = GpuBindFlag::Shader | GpuBindFlag::RenderTarget,
	};
	if (not m_renderDevice.createTexture(m_sceneRenderTarget)) {
		ANKER_FATAL("Failed to create Scene Render Target");
	}

	m_renderDevice.enableAlphaBlending();
}

void RenderSystem::draw(const Scene& scene)
{
	ANKER_PROFILE_ZONE();

	auto camera = scene.activeCamera();
	if (!camera) {
		ANKER_WARN("No active camera");
		m_renderDevice.setRenderTarget(m_renderDevice.backBuffer());
		m_renderDevice.clearRenderTarget(m_renderDevice.backBuffer());
		return;
	}

	auto [cameraNode, cameraParams] = camera.try_get<SceneNode, Camera>();
	if (!cameraNode || !cameraParams) {
		ANKER_WARN("Invalid active camera");
		m_renderDevice.setRenderTarget(m_renderDevice.backBuffer());
		m_renderDevice.clearRenderTarget(m_renderDevice.backBuffer());
		return;
	}

	{
		SceneConstantBuffer sceneCb;

		Mat3 view = Mat3(cameraNode->globalTransform());
		view = scale(view, glm::vec2(cameraParams->distance));

		// Correct for varying aspect ratio. Generally we want to keep the
		// vertical size as is when the window gets wider.
		auto aspectRatio = m_renderDevice.backBuffer().info.size.ratio();
		if (aspectRatio < 1.0f) {
			view = scale(view, {1.0f, 1.0f / aspectRatio});
		} else {
			view = scale(view, {aspectRatio, 1.0f});
		}

		sceneCb.view = inverse(view);
		sceneCb.cameraPosition = cameraNode->globalTransform().position;

		m_renderDevice.fillBuffer(m_sceneConstantBuffer, std::array{sceneCb});
		m_renderDevice.bindBufferVS(0, m_sceneConstantBuffer);
		m_renderDevice.bindBufferPS(0, m_sceneConstantBuffer);
	}

	////////////////////////////////////////////////////////////
	// Scene Rendering

	m_renderDevice.setRasterizer({.depthClip = false});

	Vec3 clearColor = {0.1f, 0.17f, 0.24f};
	m_renderDevice.clearRenderTarget(m_sceneRenderTarget, nullptr, clearColor);
	m_renderDevice.setRenderTarget(m_sceneRenderTarget);

	for (auto [_, node] : scene.registry.view<SceneNode>().each()) {
		if (!node.hasParent()) {
			drawSceneNodeRecursive(scene, &node);
		}
	}

	// m_textRenderer.draw(*g_engine->fontSystem.systemFont(), "The quick brown fox jumps over the lazy dog.");

	////////////////////////////////////////////////////////////
	// Post Processing

	m_renderDevice.setRenderTarget(m_renderDevice.backBuffer());
	m_renderDevice.clearRenderTarget(m_renderDevice.backBuffer());

	m_renderDevice.bindTexturePS(0, m_sceneRenderTarget);
	m_postProcessRenderer.draw(cameraParams->postProcessParams);
	m_renderDevice.unbindTexturePS(0);

	gizmoRenderer.draw();
}

void RenderSystem::onResize(Vec2i)
{
	m_sceneRenderTarget.info.size = m_renderDevice.backBuffer().info.size;
	if (not m_renderDevice.createTexture(m_sceneRenderTarget)) {
		ANKER_FATAL("Failed to create Scene Render Target");
	}
}

void RenderSystem::drawSceneNodeRecursive(const Scene& scene, const SceneNode* node)
{
	if (node->entity().all_of<Sprite>()) {
		m_spriteRenderer.draw(scene, node);
	}
	if (node->entity().all_of<MapLayer>()) {
		m_mapRenderer.draw(scene, node);
	}

	for (auto* child : node->children()) {
		drawSceneNodeRecursive(scene, child);
	}
}

} // namespace Anker
