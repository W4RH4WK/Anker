#pragma once

#include <anker/graphics/anker_gizmo_renderer.hpp>
#include <anker/graphics/anker_post_process_renderer.hpp>
#include <anker/graphics/anker_render_device.hpp>
#include <anker/graphics/anker_sprite_renderer.hpp>
#include <anker/graphics/anker_text_renderer.hpp>
#include <anker/graphics/anker_tile_layer_renderer.hpp>

namespace Anker {

class AssetCache;
class Scene;
class SceneNode;

class RenderSystem {
  public:
	RenderSystem(RenderDevice&, AssetCache&);
	RenderSystem(const RenderSystem&) = delete;
	RenderSystem& operator=(const RenderSystem&) = delete;
	RenderSystem(RenderSystem&&) noexcept = delete;
	RenderSystem& operator=(RenderSystem&&) noexcept = delete;

	void draw(const Scene&);

	void onResize(Vec2i size);

	GizmoRenderer gizmoRenderer;

  private:
	void drawSceneNodeRecursive(const Scene&, const SceneNode*);

	RenderDevice& m_renderDevice;

	TileLayerRenderer m_tileLayerRenderer;
	SpriteRenderer m_spriteRenderer;
	PostProcessRenderer m_postProcessRenderer;

	TextRenderer m_textRenderer;

	GpuBuffer m_sceneConstantBuffer;
	Texture m_sceneRenderTarget;
};

} // namespace Anker
