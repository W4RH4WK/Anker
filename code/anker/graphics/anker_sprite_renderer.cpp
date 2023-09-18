#include <anker/graphics/anker_sprite_renderer.hpp>

#include <anker/core/anker_asset_cache.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_transform.hpp>
#include <anker/graphics/anker_parallax.hpp>
#include <anker/graphics/anker_sprite.hpp>
#include <anker/graphics/anker_vertex.hpp>

namespace Anker {

struct SpriteRendererConstantBuffer {
	Mat4 transform = Mat4Id; // Mat4 instead of Mat3 because of alignment
	Vec4 color = Vec4(1);
	Vec2 parallax = Vec2(1);
	Vec2 _pad;
};
static_assert(sizeof(SpriteRendererConstantBuffer) % 16 == 0, "Constant Buffer size must be 16-byte aligned");

SpriteRenderer::SpriteRenderer(RenderDevice& renderDevice, AssetCache& assetCache) : m_renderDevice(renderDevice)
{
	m_vertexShader = assetCache.loadVertexShader("shaders/sprite.vs", Vertex2D::ShaderInputs);
	m_pixelShader = assetCache.loadPixelShader("shaders/sprite.ps");

	m_constantBuffer.info = {
	    .name = "SpriteRenderer Constant Buffer",
	    .size = sizeof(SpriteRendererConstantBuffer),
	    .stride = sizeof(SpriteRendererConstantBuffer),
	    .bindFlags = GpuBindFlag::ConstantBuffer,
	    .flags = GpuBufferFlag::CpuWriteable,
	};
	if (not m_renderDevice.createBuffer(m_constantBuffer)) {
		ANKER_FATAL("Failed to create SpriteRenderer Constant Buffer");
	}

	m_vertexBuffer.info = {
	    .name = "SpriteRenderer Vertex Buffer",
	    .size = 4 * sizeof(Vertex2D),
	    .stride = sizeof(Vertex2D),
	    .bindFlags = GpuBindFlag::VertexBuffer,
	    .flags = GpuBufferFlag::CpuWriteable,
	};
	if (not m_renderDevice.createBuffer(m_vertexBuffer)) {
		ANKER_FATAL("Failed to create SpriteRenderer Vertex Buffer");
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

	m_renderDevice.bindVertexShader(*m_vertexShader);
	m_renderDevice.bindPixelShader(*m_pixelShader);

	for (auto [entity, transform, sprite] : scene.registry.view<Transform2D, Sprite>().each()) {
		if (sprite.layer != layerToRender || !sprite.texture) {
			continue;
		}

		{
			SpriteRendererConstantBuffer cb = {
			    .transform = Mat3(transform),
			    .color = sprite.color,
			};
			if (auto* parallax = scene.registry.try_get<Parallax>(entity)) {
				cb.parallax = parallax->factor;
			}
			m_renderDevice.fillBuffer(m_constantBuffer, std::array{cb});
			m_renderDevice.bindBufferVS(1, m_constantBuffer);
			m_renderDevice.bindBufferPS(1, m_constantBuffer);
		}

		auto textureSize = Vec2(sprite.texture->info.size);
		Rect2 spriteRect;
		spriteRect.size = Vec2(sprite.texture->info.size) * sprite.textureRect.size / sprite.pixelToMeter;
		spriteRect.offset = spriteRect.size * sprite.offset;

		m_renderDevice.fillBuffer( //
		    m_vertexBuffer,        //
		    std::array{
		        Vertex2D{
		            .position = spriteRect.topLeftWorld(),
		            .uv = sprite.textureRect.topLeft(),
		        },
		        Vertex2D{
		            .position = spriteRect.bottomLeftWorld(),
		            .uv = sprite.textureRect.bottomLeft(),
		        },
		        Vertex2D{
		            .position = spriteRect.topRightWorld(),
		            .uv = sprite.textureRect.topRight(),
		        },
		        Vertex2D{
		            .position = spriteRect.bottomRightWorld(),
		            .uv = sprite.textureRect.bottomRight(),
		        },
		    });

		m_renderDevice.bindTexturePS(0, *sprite.texture);
		m_renderDevice.draw(m_vertexBuffer, 4, Topology::TriangleStrip);
		m_renderDevice.unbindTexturePS(0);
	}
}

} // namespace Anker
