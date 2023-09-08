#include <anker/ui/anker_ui_system.hpp>

#include <stb_rect_pack.h>
#include <stb_truetype.h>

#include <anker/core/anker_data_loader.hpp>
#include <anker/graphics/anker_render_device.hpp>

namespace Anker {

UISystem::UISystem(DataLoader& dataLoader, RenderDevice& renderDevice)
    : m_dataLoader(dataLoader), m_renderDevice(renderDevice)
{
	if (not loadFont(m_systemFont, "fonts/Roboto")) {
		ANKER_WARN("Failed to load system font");
	}
}

static Status loadFontFromTTF(Font& font, std::span<const uint8_t> fontData, RenderDevice& renderDevice)
{
	stbtt_fontinfo info;
	if (!stbtt_InitFont(&info, fontData.data(), 0)) {
		ANKER_ERROR("stbtt_InitFont failed");
		return FontError;
	}

	const Vec2u texSize = font.texture.info.size;

	std::vector<uint8_t> bitmap(texSize.x * texSize.y);

	// Render glyphs to bitmap and populate the font's charData.
	{
		stbtt_pack_context packContext;
		if (!stbtt_PackBegin(&packContext, bitmap.data(), texSize.x, texSize.y, 0, 8, nullptr)) {
			ANKER_ERROR("stbtt_PackBegin failed");
			return FontError;
		}

		std::vector<stbtt_packedchar> charData(font.charData.size());

		stbtt_pack_range range = {
		    .font_size = 32.0f,
		    .first_unicode_codepoint_in_range = font.CharStart,
		    .num_chars = int(charData.size()),
		    .chardata_for_range = charData.data(),
		};

		stbtt_PackSetOversampling(&packContext, 2, 2);

		stbtt_PackFontRanges(&packContext, fontData.data(), 0, &range, 1);

		stbtt_PackEnd(&packContext);

		std::ranges::transform(charData, font.charData.begin(), [&](auto& c) {
			return Font::CharData{
			    .texRect = Rect2::fromPoints(Vec2(c.x0, c.y0) / Vec2(texSize), Vec2(c.x1, c.y1) / Vec2(texSize)),
			    .visRect = Rect2::fromPoints({c.xoff, -c.yoff}, {c.xoff2, -c.yoff2}),
			    .xAdvance = c.xadvance,
			};
		});
	}

	font.texture.info.format = TextureFormat::R8_UNORM;
	font.texture.info.bindFlags = GpuBindFlag::Shader;
	std::array texInit = {TextureInit{.data = bitmap.data(), .rowPitch = texSize.x}};
	ANKER_TRY(renderDevice.createTexture(font.texture, texInit));

	// We copy over the kerning table so the can release the loaded font data.
	if (info.kern) {
		std::vector<stbtt_kerningentry> entries(stbtt_GetKerningTableLength(&info));
		stbtt_GetKerningTable(&info, entries.data(), int(entries.size()));

		for (auto& entry : entries) {
			bool inRange = font.CharStart <= entry.glyph1 && entry.glyph1 <= font.CharEnd //
			            && font.CharStart <= entry.glyph2 && entry.glyph2 <= font.CharEnd;
			if (inRange) {
				auto index1 = entry.glyph1 - font.CharStart;
				auto index2 = entry.glyph2 - font.CharStart;
				font.kerningTable[index1 * font.CharCount + index2] = entry.advance;
			}
		}
	}

	return OK;
}

Status UISystem::loadFont(Font& font, std::string_view identifier)
{
	ANKER_PROFILE_ZONE_T(identifier);

	font.texture.info.name = identifier;
	font.texture.info.size = {512, 512};

	ByteBuffer fontData;
	if (m_dataLoader.load(std::string{identifier} + ".ttf", fontData)) {
		if (loadFontFromTTF(font, fontData, m_renderDevice)) {
			return OK;
		}
	}

	ANKER_ERROR("{}: Missing, using fallback!", identifier);
	font = systemFont();
	return ReadError;
}

} // namespace Anker
