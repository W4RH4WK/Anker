#pragma once

#include <anker/core/anker_asset.hpp>

namespace Anker {

enum class GpuBindFlag {
	None = 0,
	ConstantBuffer = 1 << 0,
	VertexBuffer = 1 << 1,
	IndexBuffer = 1 << 2,
	Shader = 1 << 3,
	RenderTarget = 1 << 4,
	DepthStencil = 1 << 5,
};
ANKER_ENUM_FLAGS(GpuBindFlag)

////////////////////////////////////////////////////////////
// Shaders

enum class Topology {
	LineList = 2,
	TriangleList = 4,
	TriangleStrip = 5,
};

struct VertexShaderInfo {
	std::string name;
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputs;
};

struct VertexShader {
	VertexShaderInfo info;
	ComPtr<ID3D11VertexShader> shader;
	ComPtr<ID3D11InputLayout> inputLayout;
};

struct PixelShaderInfo {
	std::string name;
};

struct PixelShader {
	PixelShaderInfo info;
	ComPtr<ID3D11PixelShader> shader;
};

////////////////////////////////////////////////////////////
// GPU Buffers

enum class GpuBufferFlag {
	None = 0,
	CpuWriteable = 1 << 0,
	Structured = 1 << 1,
};
ANKER_ENUM_FLAGS(GpuBufferFlag)

struct GpuBufferInfo {
	std::string name;
	uint32_t size = 0;
	uint32_t stride = 1;
	GpuBindFlags bindFlags;
	GpuBufferFlags flags;
};

struct GpuBuffer {
	GpuBufferInfo info;
	ComPtr<ID3D11Buffer> buffer;
};

////////////////////////////////////////////////////////////
// Samplers

enum class FilterMode { Point, Linear, Anisotropic };
enum class TexAddressMode { Wrap = 1, Mirror = 2, Clamp = 3, Border = 4, MirrorOnce = 5 };

enum class CompareFunc {
	Never = 1,
	Less = 2,
	Equal = 3,
	LessEqual = 4,
	Greater = 5,
	NotEqual = 6,
	GreaterEqual = 7,
	Always = 8,
};

struct SamplerDesc {
	FilterMode filterMode = FilterMode::Anisotropic;
	TexAddressMode addressModeU = TexAddressMode::Border;
	TexAddressMode addressModeV = TexAddressMode::Border;
	TexAddressMode addressModeW = TexAddressMode::Border;
	CompareFunc compareFunc = CompareFunc::Never;

	friend auto operator<=>(const SamplerDesc&, const SamplerDesc&) = default;
};

////////////////////////////////////////////////////////////
// Textures

enum class TextureFormat {
	R16G16B16A16_UNORM = 11,
	R8G8B8A8_UNORM = 28,
	R8G8B8A8_UNORM_SRGB = 29,
	D32_FLOAT = 40,
	R8_UNORM = 61,
	BC7_UNORM = 98,
	BC7_UNORM_SRGB = 99,
};

enum class TextureFlag {
	None = 0,
	CpuWriteable = 1 << 0,
	Cubemap = 1 << 1,
};
ANKER_ENUM_FLAGS(TextureFlag)

struct TextureInfo {
	std::string name;
	Vec2u size = Vec2u::Zero;
	uint32_t mipLevels = 1;
	uint32_t arraySize = 1;
	TextureFormat format = TextureFormat::R8G8B8A8_UNORM;
	GpuBindFlags bindFlags = GpuBindFlag::Shader;
	TextureFlags flags;
};

struct TextureInit {
	const uint8_t* data;
	uint32_t rowPitch;
};

struct Texture {
	TextureInfo info;
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> shaderView;
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	ComPtr<ID3D11DepthStencilView> depthView;
};

////////////////////////////////////////////////////////////
// Rasterizer

struct RasterizerDesc {
	bool wireframe = false;
	bool depthClip = true;

	friend auto operator<=>(const RasterizerDesc&, const RasterizerDesc&) = default;
};

////////////////////////////////////////////////////////////
// Render Device

// RenderDevice builds a thin abstraction over the graphics API. It takes care
// of initialization (and finalization), holds the GPU device / context resource
// handles, and enables other engine parts (e.g. Renderer) to acquire and use
// GPU resources.
//
// We also manage the swap-chain and related buffers / views.
class RenderDevice {
  public:
	RenderDevice();

	RenderDevice(const RenderDevice&) = delete;
	RenderDevice& operator=(const RenderDevice&) = delete;
	RenderDevice(RenderDevice&&) noexcept = delete;
	RenderDevice& operator=(RenderDevice&&) noexcept = delete;

	////////////////////////////////////////////////////////////
	// Buffers

	// Creates a GPU buffer according to buffer.info .
	Status createBuffer(GpuBuffer& buffer, std::span<const uint8_t> init = {});

	Status createBuffer(GpuBuffer& buffer, Spannable auto const& init)
	{
		std::span initView = init;
		buffer.info.stride = sizeof(decltype(initView)::value_type);
		return createBuffer(buffer, std::span<const uint8_t>(asBytes(initView)));
	}

	void bindBufferVS(uint32_t slot, const GpuBuffer&);
	void bindBufferPS(uint32_t slot, const GpuBuffer&);

	template <typename T = uint8_t>
	T* mapBuffer(GpuBuffer& buffer)
	{
		return static_cast<T*>(mapResource(buffer.buffer.Get()));
	}
	void unmapBuffer(GpuBuffer&);

	void fillBuffer(GpuBuffer& buffer, Spannable auto const& data)
	{
		std::span dataView = data;
		ANKER_ASSERT(buffer.info.stride == sizeof(decltype(dataView)::value_type));

		// Automatically grow buffer as needed.
		if (buffer.info.size < dataView.size_bytes()) {
			buffer.info.size = uint32_t(dataView.size_bytes());
			if (not createBuffer(buffer)) {
				ANKER_FATAL("Failed to grow {}", buffer.info.name);
			}
		}

		auto* dst = mapBuffer(buffer);
		std::ranges::copy(asBytes(dataView), dst);
		unmapBuffer(buffer);
	}

	////////////////////////////////////////////////////////////
	// Shaders

	Status loadVertexShader(VertexShader&, std::string_view identifier);
	Status loadPixelShader(PixelShader&, std::string_view identifier);

	void bindVertexShader(const VertexShader&);
	void bindPixelShader(const PixelShader&);

	////////////////////////////////////////////////////////////
	// Textures

	Status loadTexture(Texture&, std::string_view identifier);

	// Creates a texture according to texture.info .
	Status createTexture(Texture&, std::span<const TextureInit> = {});

	void bindTexturePS(uint32_t slot, const Texture&, const SamplerDesc& = {});
	void unbindTexturePS(uint32_t slot);

	template <typename T = uint8_t>
	T* mapTexture(const Texture& texture, uint32_t* outRowPitch)
	{
		ANKER_ASSERT(outRowPitch);
		return static_cast<T*>(mapResource(texture.texture.Get(), outRowPitch));
	}
	void unmapTexture(const Texture&);

	////////////////////////////////////////////////////////////
	// Rasterizer

	void setRasterizer(const RasterizerDesc& = {});

	////////////////////////////////////////////////////////////
	// Blending

	void enableAlphaBlending();

	////////////////////////////////////////////////////////////
	// Render Target

	void setRenderTarget(const Texture&, const Texture* depth = nullptr);
	void clearRenderTarget(const Texture&, const Texture* depth = nullptr, const Vec3& clearColor = Vec3(0));

	void bindRenderTargetPS(uint32_t slot, const Texture&, const SamplerDesc& = {});

	////////////////////////////////////////////////////////////

	void draw(uint32_t vertexCount);
	void draw(const GpuBuffer& vertexBuffer, uint32_t vertexCount, Topology = Topology::TriangleList);
	void draw(const GpuBuffer& vertexBuffer, const GpuBuffer& indexBuffer, uint32_t indexCount,
	          Topology = Topology::TriangleList);

	void drawInstanced(uint32_t vertexCount, uint32_t instanceCount);
	void drawInstanced(const GpuBuffer& vertexBuffer, uint32_t vertexCount,         //
	                   const GpuBuffer& instanceDataBuffer, uint32_t instanceCount, //
	                   Topology = Topology::TriangleList);
	void drawInstanced(const GpuBuffer& vertexBuffer,                               //
	                   const GpuBuffer& indexBuffer, uint32_t indexCount,           //
	                   const GpuBuffer& instanceDataBuffer, uint32_t instanceCount, //
	                   Topology = Topology::TriangleList);

	////////////////////////////////////////////////////////////
	// ImGui

	void imguiImplInit();
	void imguiImplShutdown();
	void imguiImplNewFrame();
	void imguiImplRender();

	////////////////////////////////////////////////////////////

	void present();

	void onResize(Vec2i size);

	const Texture& backBuffer() const { return m_backBuffer; }

	AssetPtr<Texture> fallbackTexture() const { return m_fallbackTexture; }

	// Avoid directly accessing these if possible:
	ID3D11Device* device() { return m_device.Get(); }
	ID3D11DeviceContext* context() { return m_context.Get(); }

  private:
	void createMainRenderTarget();

	ID3D11SamplerState* samplerStateFromDesc(const SamplerDesc&);
	std::map<SamplerDesc, ComPtr<ID3D11SamplerState>> m_samplerStates;

	ID3D11RasterizerState* rasterizerStateFromDesc(const RasterizerDesc&);
	std::map<RasterizerDesc, ComPtr<ID3D11RasterizerState>> m_rasterizerStates;

	void* mapResource(ID3D11Resource*, uint32_t* outRowPitch = nullptr, uint32_t* outDepthPitch = nullptr);
	void unmapResource(ID3D11Resource*);

	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<IDXGIDevice3> m_dxgiDevice;
	ComPtr<IDXGIAdapter> m_dxgiAdapter;
	ComPtr<IDXGIFactory> m_dxgiFactory;
	ComPtr<IDXGISwapChain> m_dxgiSwapchain;

	ComPtr<ID3D11BlendState> m_alphaBlendState;

	Texture m_backBuffer;

	AssetPtr<Texture> m_fallbackTexture;
};

} // namespace Anker
