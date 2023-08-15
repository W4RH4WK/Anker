#pragma once

#include <anker/core/anker_asset.hpp>

#include "anker_render_device.hpp"

namespace Anker {

class AssetCache;

class ScreenRenderer {
  public:
	ScreenRenderer(RenderDevice&, AssetCache&);
	ScreenRenderer(const ScreenRenderer&) = delete;
	ScreenRenderer& operator=(const ScreenRenderer&) = delete;
	ScreenRenderer(ScreenRenderer&&) noexcept = delete;
	ScreenRenderer& operator=(ScreenRenderer&&) noexcept = delete;

	void draw();

  private:
	RenderDevice& m_renderDevice;

	AssetPtr<VertexShader> m_vertexShader;
};

} // namespace Anker
