#pragma once

#include <anker/graphics/anker_render_device.hpp>

namespace Anker {

class RenderDevice;

struct Font {
	static Status load(Font&, std::span<const uint8_t>, RenderDevice&);

	Font() = default;
	Font(const Font&) = delete;
	Font& operator=(const Font&) = delete;
	Font(Font&&) noexcept = delete;
	Font& operator=(Font&&) noexcept = delete;

	// We only support the 96 printable ASCII characters for now, starting with
	// SPACE at char 32.
	static constexpr unsigned CharStart = 32;
	static constexpr unsigned CharEnd = 128;
	static constexpr unsigned CharCount = CharEnd - CharStart;

	struct CharData {
		Rect2u texCoords;
		Vec2 visualOffset;
		float xAdvance = 0;
	};

	std::array<CharData, CharCount> charData{};

	int kern(char a, char b) const
	{
		bool inRange = CharStart <= a && a <= CharEnd //
		            && CharStart <= b && b <= CharEnd;
		if (inRange) {
			return kerningTable[(a - CharStart) * CharCount + (b - CharStart)];
		} else {
			return 0;
		}
	}

	std::array<int, CharCount * CharCount> kerningTable{};

	Texture texture;

	std::string name;
};

} // namespace Anker
