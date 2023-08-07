#pragma once

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

  private:
	RenderDevice& m_renderDevice;

	SpriteRenderer m_spriteRenderer;

	GpuBuffer m_sceneConstantBuffer;
};

} // namespace Anker
