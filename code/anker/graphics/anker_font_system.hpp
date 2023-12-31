#pragma once

#include <anker/graphics/anker_font.hpp>

namespace Anker {

class RenderDevice;

class FontSystem {
  public:
	FontSystem(RenderDevice&);
	FontSystem(const FontSystem&) = delete;
	FontSystem& operator=(const FontSystem&) = delete;
	FontSystem(FontSystem&&) noexcept = delete;
	FontSystem& operator=(FontSystem&&) noexcept = delete;

	Status loadFont(Font& font, std::string_view identifier);

	const Font& systemFont() const { return m_systemFont; }

  private:
	Status loadFontFromTTF(Font& font, std::span<const u8> fontData);

	RenderDevice& m_renderDevice;

	Font m_systemFont;
};

} // namespace Anker
