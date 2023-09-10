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
	            .SemanticName = "TEXTURE_RECT",
	            .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
	            .InputSlot = 1,
	            .AlignedByteOffset = offsetof(InstanceData, textureRect),
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

	m_vertexBuffer.info = {
	    .name = "SpriteRenderer Vertex Buffer",
	    .bindFlags = GpuBindFlag::VertexBuffer,
	};
	const std::array vertices = {
	    Vertex{.position = {0, 1}, .uv = {0, 0}},
	    Vertex{.position = {0, 0}, .uv = {0, 1}},
	    Vertex{.position = {1, 1}, .uv = {1, 0}},
	    Vertex{.position = {1, 0}, .uv = {1, 1}},
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

void SpriteRenderer::collectRenderLayers(const Scene& scene, std::insert_iterator<std::set<RenderLayer>> inserter)
{
	for (auto [_, sprite] : scene.registry.view<Sprite>().each()) {
		inserter = sprite.layer;
	}
}

void SpriteRenderer::draw(const Scene& scene, RenderLayer layerToRender)
{
	ANKER_PROFILE_ZONE();

	struct SpriteRef {
		const Transform2D* transform = nullptr;
		const Sprite* sprite = nullptr;
	};

	// Collect all render-able sprites, on the target layer, and organize them
	// in a list grouped by texture.
	std::vector<SpriteRef> spritesToRender;
	{
		auto sprites = scene.registry.view<Transform2D, Sprite>();

		for (auto [_, transform, sprite] : sprites.each()) {
			if (sprite.layer == layerToRender && sprite.texture) {
				spritesToRender.push_back({&transform, &sprite});
			}
		}

		auto byTexture = [](auto& a, auto& b) { return a.sprite->texture.get() < b.sprite->texture.get(); };
		std::ranges::sort(spritesToRender, byTexture);
	}

	m_renderDevice.bindVertexShader(*m_vertexShader);
	m_renderDevice.bindPixelShader(*m_pixelShader);

	// Iterate over the collected sprites and draw all sprites simultaneously
	// that use the same texture.
	auto getTexture = [](auto& a) { return a.sprite->texture.get(); };
	for (auto& [texture, sprites] : spritesToRender | iter::groupby(getTexture)) {
		m_instanceData.clear();

		for (auto [transform, sprite] : sprites) {
			Mat3 spriteTransform = Mat3(*transform);
			spriteTransform = scale(spriteTransform, glm::vec2(Vec2(texture->info.size) * sprite->textureRect.size / sprite->pixelToMeter));
			spriteTransform = translate(spriteTransform, glm::vec2(sprite->offset));

			m_instanceData.push_back({
			    .transform = spriteTransform,
			    .textureRect = Vec4(sprite->textureRect),
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

} // namespace Anker
