#pragma once

#include <anker/graphics/anker_render_device.hpp>

namespace Anker {

struct Font {
	// We only support the 96 printable ASCII characters for now, starting with
	// SPACE at char 32.
	static constexpr unsigned CharStart = 32;
	static constexpr unsigned CharEnd = 128;
	static constexpr unsigned CharCount = CharEnd - CharStart;
	static bool inRange(char c) { return CharStart <= c && c <= CharEnd; }

	struct CharData {
		Rect2 texRect;      // Glyph location in texture
		Rect2 visRect;      // Glyph offset for rendering
		float xAdvance = 0; // Horizontal offset to the next character
	};

	const CharData& operator[](char c) const
	{
		if (!inRange(c)) {
			c = ' ';
		}
		return charData[c - CharStart];
	}

	std::array<CharData, CharCount> charData{};

	int kern(char a, char b) const
	{
		if (inRange(a) && inRange(b)) {
			return kerningTable[(a - CharStart) * CharCount + (b - CharStart)];
		} else {
			return 0;
		}
	}

	std::array<int, CharCount * CharCount> kerningTable{};

	Texture texture;
};

} // namespace Anker
