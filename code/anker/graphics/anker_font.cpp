#include <anker/graphics/anker_font.hpp>

#include <stb_rect_pack.h>
#include <stb_truetype.h>

namespace Anker {

Status Font::load(Font& font, std::span<const uint8_t> fontData, RenderDevice& renderDevice)
{
	ANKER_PROFILE_ZONE_T(font.name);

	stbtt_fontinfo info;
	if (!stbtt_InitFont(&info, fontData.data(), 0)) {
		ANKER_ERROR("stbtt_InitFont failed");
		return FontError;
	}

	std::vector<uint8_t> bitmap(512 * 512);

	// Render glyphs to bitmap and populate the font's charData.
	{
		stbtt_pack_context packContext;
		if (!stbtt_PackBegin(&packContext, bitmap.data(), 512, 512, 0, 8, nullptr)) {
			ANKER_ERROR("stbtt_PackBegin failed");
			return FontError;
		}

		std::vector<stbtt_packedchar> charData(font.charData.size());

		stbtt_pack_range range = {
		    .font_size = 32.0f,
		    .first_unicode_codepoint_in_range = CharStart,
		    .num_chars = int(charData.size()),
		    .chardata_for_range = charData.data(),
		};

		stbtt_PackSetOversampling(&packContext, 2, 2);

		stbtt_PackFontRanges(&packContext, fontData.data(), 0, &range, 1);

		stbtt_PackEnd(&packContext);

		std::ranges::transform(charData, font.charData.begin(), [](auto& c) {
			return Font::CharData{
			    .texCoords = Rect2u::fromPoints({c.x0, c.y0}, {c.x1, c.y1}),
			    .visualOffset = {c.xoff, c.yoff},
			    .xAdvance = c.xadvance,
			};
		});
	}

	font.texture.info = {
	    .name = font.name,
	    .size = {512, 512},
	    .format = TextureFormat::R8_UNORM,
	    .bindFlags = GpuBindFlag::Shader,
	};
	std::array textureInit = {TextureInit{.data = bitmap.data(), .rowPitch = 512}};

	ANKER_TRY(renderDevice.createTexture(font.texture, textureInit));

	// We copy over the kerning table so the can release the loaded font data.
	if (info.kern) {
		std::vector<stbtt_kerningentry> entries(stbtt_GetKerningTableLength(&info));
		stbtt_GetKerningTable(&info, entries.data(), int(entries.size()));

		for (auto& entry : entries) {
			bool inRange = CharStart <= entry.glyph1 && entry.glyph1 <= CharEnd //
			            && CharStart <= entry.glyph2 && entry.glyph2 <= CharEnd;
			if (!inRange) {
				continue;
			}

			auto index1 = entry.glyph1 - CharStart;
			auto index2 = entry.glyph2 - CharStart;
			font.kerningTable[index1 * CharCount + index2] = entry.advance;
		}
	}

	return OK;
}

} // namespace Anker
