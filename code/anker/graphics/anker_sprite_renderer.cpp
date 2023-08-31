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
	        // Per Vertex
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

	        // Per Instance
	        D3D11_INPUT_ELEMENT_DESC{
	            .SemanticName = "TRANSFORM",
	            .SemanticIndex = 0,
	            .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
	            .InputSlot = 1,
	            .AlignedByteOffset = offsetof(InstanceData, transform) + 0 * sizeof(Vec4),
	            .InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA,
	            .InstanceDataStepRate = 1,
	        },
	        D3D11_INPUT_ELEMENT_DESC{
	            .SemanticName = "TRANSFORM",
	            .SemanticIndex = 1,
	            .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
	            .InputSlot = 1,
	            .AlignedByteOffset = offsetof(InstanceData, transform) + 1 * sizeof(Vec4),
	            .InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA,
	            .InstanceDataStepRate = 1,
	        },
	        D3D11_INPUT_ELEMENT_DESC{
	            .SemanticName = "TRANSFORM",
	            .SemanticIndex = 2,
	            .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
	            .InputSlot = 1,
	            .AlignedByteOffset = offsetof(InstanceData, transform) + 2 * sizeof(Vec4),
	            .InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA,
	            .InstanceDataStepRate = 1,
	        },
	        D3D11_INPUT_ELEMENT_DESC{
	            .SemanticName = "TRANSFORM",
	            .SemanticIndex = 3,
	            .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
	            .InputSlot = 1,
	            .AlignedByteOffset = offsetof(InstanceData, transform) + 3 * sizeof(Vec4),
	            .InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA,
	            .InstanceDataStepRate = 1,
	        },
	        D3D11_INPUT_ELEMENT_DESC{
	            .SemanticName = "INSTANCE_COLOR",
	            .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
	            .InputSlot = 1,
	            .AlignedByteOffset = offsetof(InstanceData, color),
	            .InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA,
	            .InstanceDataStepRate = 1,
	        },
	    });
	m_pixelShader = assetCache.loadPixelShader("shaders/basic_2d.ps");

	const std::array vertices{
	    Vertex{.position = {-0.5f, 0.5f}, .uv = {0, 0}},
	    Vertex{.position = {-0.5f, -0.5f}, .uv = {0, 1}},
	    Vertex{.position = {0.5f, 0.5f}, .uv = {1, 0}},
	    Vertex{.position = {0.5f, -0.5f}, .uv = {1, 1}},
	};
	m_vertexBuffer.info = {
	    .name = "SpriteRenderer Vertex Buffer",
	    .bindFlags = GpuBindFlag::VertexBuffer,
	};
	if (not m_renderDevice.createBuffer(m_vertexBuffer, vertices)) {
		ANKER_FATAL("Failed to create SpriteRenderer Vertex Buffer");
	}

	m_instanceBuffer.info = {
	    .name = "SpriteRenderer Instance Buffer",
	    .size = 128 * sizeof(InstanceData),
	    .stride = sizeof(InstanceData),
	    .bindFlags = GpuBindFlag::VertexBuffer,
	    .flags = GpuBufferFlag::CpuWriteable,
	};
	if (not m_renderDevice.createBuffer(m_instanceBuffer)) {
		ANKER_FATAL("Field to create SpriteRenderer Instance Buffer");
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
			m_instanceData.clear();

			for (auto& [transform, sprite] : sprites) {
				Vec2 spriteScale = Vec2(texture->info.size) / sprite->pixelToMeter;

				m_instanceData.push_back({
				    .transform = scale(Mat3(*transform), glm::vec2(spriteScale)),
				    .color = sprite->color,
				});
			}

			m_renderDevice.fillBuffer(m_instanceBuffer, m_instanceData);

			m_renderDevice.bindTexturePS(0, *texture);
			m_renderDevice.drawInstanced(m_vertexBuffer, 4,                                 //
			                             m_instanceBuffer, uint32_t(m_instanceData.size()), //
			                             Topology::TriangleStrip);
			m_renderDevice.unbindTexturePS(0);
		}
	}
}

} // namespace Anker
