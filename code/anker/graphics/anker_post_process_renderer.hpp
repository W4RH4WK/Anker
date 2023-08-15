#pragma once

#include <anker/core/anker_asset.hpp>

#include "anker_post_process_params.hpp"
#include "anker_render_device.hpp"
#include "anker_screen_renderer.hpp"

namespace Anker {

class AssetCache;

class PostProcessRenderer : ScreenRenderer {
  public:
	PostProcessRenderer(RenderDevice&, AssetCache&);
	PostProcessRenderer(const PostProcessRenderer&) = delete;
	PostProcessRenderer& operator=(const PostProcessRenderer&) = delete;
	PostProcessRenderer(PostProcessRenderer&&) noexcept = delete;
	PostProcessRenderer& operator=(PostProcessRenderer&&) noexcept = delete;

	void draw(const PostProcessParams&);

  private:
	RenderDevice& m_renderDevice;

	AssetPtr<PixelShader> m_pixelShader;
	GpuBuffer m_constantBuffer;
};

} // namespace Anker
