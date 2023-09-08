#pragma once

#include <anker/core/anker_asset.hpp>
#include <anker/graphics/anker_font.hpp>

namespace Anker {

class DataLoader;
class RenderDevice;

class FontSystem {
  public:
	FontSystem(DataLoader&, RenderDevice&);
	FontSystem(const FontSystem&) = delete;
	FontSystem& operator=(const FontSystem&) = delete;
	FontSystem(FontSystem&&) noexcept = delete;
	FontSystem& operator=(FontSystem&&) noexcept = delete;

	Status loadFont(Font& font, std::string_view identifier);

	AssetPtr<Font> systemFont() const { return m_systemFont; }

  private:
	Status loadFontFromTTF(Font& font, std::span<const uint8_t> fontData);

	DataLoader& m_dataLoader;
	RenderDevice& m_renderDevice;

	AssetPtr<Font> m_systemFont;
};

} // namespace Anker
