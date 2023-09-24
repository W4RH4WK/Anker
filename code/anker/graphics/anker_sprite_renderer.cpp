#include <anker/graphics/anker_sprite_renderer.hpp>

#include <anker/core/anker_asset_cache.hpp>
#include <anker/core/anker_entity_name.hpp>
#include <anker/core/anker_scene.hpp>
#include <anker/core/anker_scene_node.hpp>
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
	    .size = 6 * sizeof(Vertex2D),
	    .stride = sizeof(Vertex2D),
	    .bindFlags = GpuBindFlag::VertexBuffer,
	    .flags = GpuBufferFlag::CpuWriteable,
	};
	if (not m_renderDevice.createBuffer(m_vertexBuffer)) {
		ANKER_FATAL("Failed to create SpriteRenderer Vertex Buffer");
	}
}

void SpriteRenderer::draw(const Scene&, const SceneNode* node)
{
	ANKER_PROFILE_ZONE();

	auto* sprite = node->entity().try_get<Sprite>();
	if (!sprite) {
		ANKER_ERROR("{}: Missing Sprite component!", entityDisplayName(node->entity()));
		return;
	}

	m_renderDevice.bindVertexShader(*m_vertexShader);
	m_renderDevice.bindPixelShader(*m_pixelShader);

	{
		SpriteRendererConstantBuffer cb = {
		    .transform = Mat3(node->globalTransform()),
		    .color = sprite->color,
		    .parallax = sprite->parallax,
		};
		m_renderDevice.fillBuffer(m_constantBuffer, std::array{cb});
		m_renderDevice.bindBufferVS(1, m_constantBuffer);
		m_renderDevice.bindBufferPS(1, m_constantBuffer);
	}

	Rect2 spriteRect;
	spriteRect.size = Vec2(sprite->texture->info.size) * sprite->textureRect.size / sprite->pixelToMeter;
	spriteRect.offset = spriteRect.size * sprite->offset;

	m_renderDevice.fillBuffer(m_vertexBuffer, Vertex2D::makeQuad(spriteRect, sprite->textureRect));

	m_renderDevice.bindTexturePS(0, *sprite->texture);
	m_renderDevice.draw(m_vertexBuffer);
	m_renderDevice.unbindTexturePS(0);
}

} // namespace Anker
