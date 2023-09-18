#pragma once

#include <anker/core/anker_asset.hpp>
#include <anker/graphics/anker_render_device.hpp>

namespace Anker {

class Font;
class AssetCache;

class TextRenderer {
  public:
	TextRenderer(RenderDevice&, AssetCache&);
	TextRenderer(const TextRenderer&) = delete;
	TextRenderer& operator=(const TextRenderer&) = delete;
	TextRenderer(TextRenderer&&) noexcept = delete;
	TextRenderer& operator=(TextRenderer&&) noexcept = delete;

	void draw(const Font&, std::string_view text);

  private:
	RenderDevice& m_renderDevice;

	AssetPtr<VertexShader> m_vertexShader;
	AssetPtr<PixelShader> m_pixelShader;
	GpuBuffer m_vertexBuffer;
};

} // namespace Anker
