#pragma once

namespace Anker {

class DataLoader;
class RenderDevice;
class Window;

enum class BindFlag {
	None = 0,
	ConstantBuffer = 1 << 0,
	VertexBuffer = 1 << 1,
	IndexBuffer = 1 << 2,
	Shader = 1 << 3,
	RenderTarget = 1 << 4,
	DepthStencil = 1 << 5,
};
ANKER_ENUM_FLAGS(BindFlag)

////////////////////////////////////////////////////////////
// Shaders

enum class Topology {
	LineList = 2,
	TriangleList = 4,
};

struct VertexShader {
	ComPtr<ID3D11VertexShader> shader;
	ComPtr<ID3D11InputLayout> inputLayout;
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
};

struct PixelShader {
	ComPtr<ID3D11PixelShader> shader;
};

////////////////////////////////////////////////////////////
// Buffers

enum class BufferFlag {
	None = 0,
	CpuWriteable = 1 << 0,
	Structured = 1 << 1,
};
ANKER_ENUM_FLAGS(BufferFlag)

struct BufferInfo {
	std::string name;
	uint32_t size = 0;
	uint32_t stride = 1;
	BindFlags bindFlags;
	BufferFlags flags;
};

struct Buffer {
	BufferInfo info;
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
	TexAddressMode addressModeU = TexAddressMode::Wrap;
	TexAddressMode addressModeV = TexAddressMode::Wrap;
	TexAddressMode addressModeW = TexAddressMode::Wrap;
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
	Vec2u size = {64, 64};
	uint32_t mipLevels = 1;
	uint32_t arraySize = 1;
	TextureFormat format = TextureFormat::R8G8B8A8_UNORM;
	BindFlags bindFlags = BindFlag::Shader;
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
	RenderDevice(Window&, DataLoader&);

	RenderDevice(const RenderDevice&) = delete;
	RenderDevice& operator=(const RenderDevice&) = delete;
	RenderDevice(RenderDevice&&) noexcept = delete;
	RenderDevice& operator=(RenderDevice&&) noexcept = delete;

	////////////////////////////////////////////////////////////
	// Buffer

	Status createBuffer(const BufferInfo&, Buffer& outBuffer, std::span<const uint8_t> = {});

	template <typename T>
	Status createBufferFor(BufferInfo info, Buffer& outBuffer, std::span<const T> init = {})
	{
		info.stride = sizeof(T);
		return createBuffer(info, outBuffer, asBytes(init));
	}

	void bindBufferVS(uint32_t slot, const Buffer&);
	void bindBufferPS(uint32_t slot, const Buffer&);

	void* mapBuffer(const Buffer&);
	void unmapBuffer(const Buffer&);

	////////////////////////////////////////////////////////////
	// Shaders

	Status loadVertexShader(std::string_view identifier, std::span<const D3D11_INPUT_ELEMENT_DESC>,
	                        VertexShader& outVertexShader);
	Status loadPixelShader(std::string_view identifier, PixelShader& outPixelShader);

	Status createVertexShader(std::string_view identifier, std::span<const uint8_t> binary,
	                          std::span<const D3D11_INPUT_ELEMENT_DESC>, VertexShader& outVertexShader);
	Status createPixelShader(std::string_view identifier, std::span<const uint8_t> binary, PixelShader& outPixelShader);

	void bindVertexShader(const VertexShader&);
	void bindPixelShader(const PixelShader&);

	////////////////////////////////////////////////////////////
	// Textures

	Status loadTexture(std::string_view identifier, Texture& outTexture);

	Status createTexture(const TextureInfo&, Texture& outTexture, std::span<const TextureInit> = {});

	void bindTexturePS(uint32_t slot, const Texture&, const SamplerDesc& = {});
	void unbindTexturePS(uint32_t slot);

	std::byte* mapTexture(const Texture&, uint32_t* outRowPitch);
	void unmapTexture(const Texture&);

	////////////////////////////////////////////////////////////
	// Rasterizer
	void setRasterizer(const RasterizerDesc& = {});

	////////////////////////////////////////////////////////////
	// Render Target

	void setRenderTarget(const Texture&, const Texture* depth = nullptr);
	void clearRenderTarget(const Texture&, const Texture* depth = nullptr, const Vec3& clearColor = Vec3(0));

	void bindRenderTargetPS(uint32_t slot, const Texture&, const SamplerDesc& = {});

	////////////////////////////////////////////////////////////

	void draw(uint32_t vertexCount);
	void draw(const Buffer& vertexBuffer, uint32_t vertexCount, Topology = Topology::TriangleList);
	void draw(const Buffer& vertexBuffer, const Buffer& indexBuffer, uint32_t indexCount,
	          Topology = Topology::TriangleList);

	void present();

	void onResize(Vec2i size);

	Vec2u backBufferSize() const { return m_backBufferSize; }

	Texture& backBuffer() { return m_backBuffer; }
	const Texture& fallbackTexture() const { return m_fallbackTexture; }

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

	DataLoader& m_dataLoader;

	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<IDXGIDevice3> m_dxgiDevice;
	ComPtr<IDXGIAdapter> m_dxgiAdapter;
	ComPtr<IDXGIFactory> m_dxgiFactory;
	ComPtr<IDXGISwapChain> m_dxgiSwapchain;

	Texture m_backBuffer;
	Vec2u m_backBufferSize;

	Texture m_fallbackTexture;
};

} // namespace Anker
