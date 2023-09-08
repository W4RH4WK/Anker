#include <anker/graphics/anker_text_renderer.hpp>

#include <anker/core/anker_asset_cache.hpp>
#include <anker/graphics/anker_font.hpp>

namespace Anker {

TextRenderer::TextRenderer(RenderDevice& renderDevice, AssetCache& assetCache) : m_renderDevice(renderDevice)
{
	m_vertexShader = assetCache.loadVertexShader( //
	    "shaders/text.vs",                        //
	    std::array{
	        D3D11_INPUT_ELEMENT_DESC{
	            .SemanticName = "POSITION",
	            .Format = DXGI_FORMAT_R32G32_FLOAT,
	            .AlignedByteOffset = offsetof(Vertex, position),
	            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
	        },
	        D3D11_INPUT_ELEMENT_DESC{
	            .SemanticName = "TEXCOORD",
	            .SemanticIndex = 0,
	            .Format = DXGI_FORMAT_R32G32_FLOAT,
	            .AlignedByteOffset = offsetof(Vertex, uv),
	            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
	        },
	    });
	m_pixelShader = assetCache.loadPixelShader("shaders/text.ps");

	m_vertexBuffer.info = {
	    .name = "TextRenderer Vertex Buffer",
	    .size = 1024 * sizeof(Vertex),
	    .stride = sizeof(Vertex),
	    .bindFlags = GpuBindFlag::VertexBuffer,
	    .flags = GpuBufferFlag::CpuWriteable,
	};
	if (not m_renderDevice.createBuffer(m_vertexBuffer)) {
		ANKER_FATAL("Failed to create TextRenderer Vertex Buffer");
	}
}

void TextRenderer::draw(const Font& font, std::string_view text)
{
	ANKER_PROFILE_ZONE();

	std::vector<Vertex> vertices;

	Vec2 cursor = {0, 0};

	for (unsigned i = 0; i < text.size(); ++i) {
		auto& charData = font.charData(text[i]);

		if (i != 0) {
			cursor.x += font.kern(text[i - 1], text[i]);
		}

		vertices.insert(    //
		    vertices.end(), //
		    {
		        Vertex{
		            .position = cursor + charData.visRect.topLeftWorld(),
		            .uv = charData.texRect.topLeft(),
		        },
		        Vertex{
		            .position = cursor + charData.visRect.bottomLeftWorld(),
		            .uv = charData.texRect.bottomLeft(),
		        },
		        Vertex{
		            .position = cursor + charData.visRect.topRightWorld(),
		            .uv = charData.texRect.topRight(),
		        },
		        Vertex{
		            .position = cursor + charData.visRect.topRightWorld(),
		            .uv = charData.texRect.topRight(),
		        },
		        Vertex{
		            .position = cursor + charData.visRect.bottomLeftWorld(),
		            .uv = charData.texRect.bottomLeft(),
		        },
		        Vertex{
		            .position = cursor + charData.visRect.bottomRightWorld(),
		            .uv = charData.texRect.bottomRight(),
		        },
		    });

		cursor.x += charData.xAdvance;
	}

	for (auto& v : vertices) {
		v.position /= 16;
	}

	m_renderDevice.fillBuffer(m_vertexBuffer, vertices);

	m_renderDevice.bindVertexShader(*m_vertexShader);
	m_renderDevice.bindPixelShader(*m_pixelShader);

	m_renderDevice.bindTexturePS(0, font.texture());
	m_renderDevice.draw(m_vertexBuffer, uint32_t(vertices.size()));
	m_renderDevice.unbindTexturePS(0);
}

} // namespace Anker
