#pragma once

#include <anker/graphics/anker_render_device.hpp>

namespace Anker {

class Font {
  public:
	struct CharData {
		Rect2 texRect;      // Glyph location in texture
		Rect2 visRect;      // Glyph offset for rendering
		float xAdvance = 0; // Horizontal offset to the next character
	};

	const CharData& charData(char c) const
	{
		if (!inRange(c)) {
			c = ' ';
		}
		return m_charData[c - CharStart];
	}

	float kern(char a, char b) const
	{
		if (inRange(a) && inRange(b)) {
			return m_scale * m_kerningTable[(a - CharStart) * CharCount + (b - CharStart)];
		} else {
			return 0;
		}
	}

	float scale() const { return m_scale; }

	const Texture& texture() const { return m_texture; }

  private:
	// We only support the 96 printable ASCII characters for now, starting with
	// SPACE at char 32.
	static constexpr unsigned CharStart = 32;
	static constexpr unsigned CharEnd = 128;
	static constexpr unsigned CharCount = CharEnd - CharStart;
	static bool inRange(int c) { return CharStart <= c && c <= CharEnd; }

	std::array<CharData, CharCount> m_charData{};

	std::array<int, CharCount * CharCount> m_kerningTable{};

	float m_scale = 0;

	Texture m_texture;

	friend class FontSystem;
};

} // namespace Anker
