#pragma once

#include <anker/graphics/anker_gizmo_renderer.hpp>
#include <anker/graphics/anker_post_process_renderer.hpp>
#include <anker/graphics/anker_render_device.hpp>
#include <anker/graphics/anker_sprite_renderer.hpp>
#include <anker/graphics/anker_text_renderer.hpp>
#include <anker/graphics/anker_map_renderer.hpp>

namespace Anker {

class AssetCache;
class Scene;

class Renderer {
  public:
	Renderer(RenderDevice&, AssetCache&);
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer(Renderer&&) noexcept = delete;
	Renderer& operator=(Renderer&&) noexcept = delete;

	void draw(const Scene&);

	void onResize(Vec2i size);

	GizmoRenderer gizmoRenderer;

  private:
	RenderDevice& m_renderDevice;

	MapRenderer m_mapRenderer;
	SpriteRenderer m_spriteRenderer;
	PostProcessRenderer m_postProcessRenderer;

	TextRenderer m_textRenderer;

	GpuBuffer m_sceneConstantBuffer;
	Texture m_sceneRenderTarget;
};

} // namespace Anker
