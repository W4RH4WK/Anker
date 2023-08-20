#include <anker/graphics/anker_sprite_renderer.hpp>

#include <anker/core/anker_asset_cache.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_transform.hpp>
#include <anker/graphics/anker_sprite.hpp>

namespace Anker {

SpriteRenderer::SpriteRenderer(RenderDevice& renderDevice, AssetCache& assetCache) : m_renderDevice(renderDevice)
{
	m_vertexShader = assetCache.loadVertexShader( //
	    "shaders/basic_2d.vs",                    //
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
	        D3D11_INPUT_ELEMENT_DESC{
	            .SemanticName = "COLOR",
	            .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
	            .AlignedByteOffset = offsetof(Vertex, color),
	            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
	        },
	    });
	m_pixelShader = assetCache.loadPixelShader("shaders/basic_2d.ps");

	m_vertexBuffer.info = {
	    .name = "SpriteRenderer Vertex Buffer",
	    .size = 128 * 6 * sizeof(Vertex),
	    .stride = sizeof(Vertex),
	    .bindFlags = GpuBindFlag::VertexBuffer,
	    .flags = GpuBufferFlag::CpuWriteable,
	};
	if (not m_renderDevice.createBuffer(m_vertexBuffer)) {
		ANKER_FATAL("Failed to create SpriteRenderer Vertex Buffer");
	}
}

void SpriteRenderer::draw(const Scene& scene)
{
	ANKER_PROFILE_ZONE();

	m_renderDevice.bindVertexShader(*m_vertexShader);
	m_renderDevice.bindPixelShader(*m_pixelShader);

	struct SpriteRef {
		const Transform2D* transform = nullptr;
		const Sprite* sprite = nullptr;
	};

	// Collect all render-able sprites and organize them in a list from back to
	// front, grouped by texture.
	std::vector<SpriteRef> spritesToRender;
	{
		ANKER_PROFILE_ZONE_N("Collect Sprites to render");

		auto sprites = scene.registry.view<Transform2D, Sprite>();
		spritesToRender.reserve(sprites.size_hint());

		for (const auto& [_, transform, sprite] : sprites.each()) {
			if (sprite.texture) {
				spritesToRender.push_back({&transform, &sprite});
			}
		}

		auto byTexture = [](auto& a, auto& b) { return a.sprite->texture.get() < b.sprite->texture.get(); };
		auto byLayer = [](auto& a, auto& b) { return a.sprite->layer < b.sprite->layer; };
		std::ranges::sort(spritesToRender, byTexture);
		std::ranges::stable_sort(spritesToRender, byLayer);
	}

	// Iterate over the collected sprites and draw all sprites simultaneously
	// that are on the same layer and use the same texture.
	auto getTexture = [](auto& a) { return a.sprite->texture.get(); };
	auto getLayer = [](auto& a) { return a.sprite->layer; };
	for (auto& [layer, spritesInLayer] : spritesToRender | iter::groupby(getLayer)) {
		for (auto& [texture, sprites] : spritesInLayer | iter::groupby(getTexture)) {
			m_vertices.clear();
			for (auto& [transform, sprite] : sprites) {
				Vec2 spriteScale = Vec2(texture->info.size) / sprite->pixelToMeter;

				m_vertices.insert(    //
				    m_vertices.end(), //
				    {
				        Vertex{
				            .position = *transform * (Vec2(0.5f, 0.5f) * spriteScale),
				            .uv = {1, 0},
				            .color = sprite->color,
				        },
				        Vertex{
				            .position = *transform * (Vec2(-0.5f, 0.5f) * spriteScale),
				            .uv = {0, 0},
				            .color = sprite->color,
				        },
				        Vertex{
				            .position = *transform * (Vec2(-0.5f, -0.5f) * spriteScale),
				            .uv = {0, 1},
				            .color = sprite->color,
				        },
				        Vertex{
				            .position = *transform * (Vec2(0.5f, 0.5f) * spriteScale),
				            .uv = {1, 0},
				            .color = sprite->color,
				        },
				        Vertex{
				            .position = *transform * (Vec2(-0.5f, -0.5f) * spriteScale),
				            .uv = {0, 1},
				            .color = sprite->color,
				        },
				        Vertex{
				            .position = *transform * (Vec2(0.5f, -0.5f) * spriteScale),
				            .uv = {1, 1},
				            .color = sprite->color,
				        },
				    });
			}

			if (auto sizeInBytes = m_vertices.size() * sizeof(m_vertices[0]); m_vertexBuffer.info.size < sizeInBytes) {
				m_vertexBuffer.info.size = uint32_t(sizeInBytes);
				if (not m_renderDevice.createBuffer(m_vertexBuffer)) {
					ANKER_FATAL("Failed to create SpriteRenderer Vertex Buffer");
				}
			}

			m_renderDevice.fillBuffer(m_vertexBuffer, m_vertices);

			m_renderDevice.bindTexturePS(0, *texture);
			m_renderDevice.draw(m_vertexBuffer, uint32_t(m_vertices.size()));
			m_renderDevice.unbindTexturePS(0);
		}
	}
}

} // namespace Anker
