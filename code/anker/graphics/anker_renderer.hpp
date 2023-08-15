#pragma once

#include "anker_post_process_renderer.hpp"
#include "anker_render_device.hpp"
#include "anker_sprite_renderer.hpp"

namespace Anker {

class AssetCache;
class Scene;

class Renderer {
  public:
	explicit Renderer(RenderDevice&, AssetCache&);
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer(Renderer&&) noexcept = delete;
	Renderer& operator=(Renderer&&) noexcept = delete;

	void draw(const Scene&);

	void onResize(Vec2i size);

  private:
	RenderDevice& m_renderDevice;

	SpriteRenderer m_spriteRenderer;
	PostProcessRenderer m_postProcessRenderer;

	GpuBuffer m_sceneConstantBuffer;
	Texture m_sceneRenderTarget;
};

} // namespace Anker
