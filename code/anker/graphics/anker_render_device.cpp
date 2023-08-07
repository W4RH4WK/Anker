#include "anker_render_device.hpp"

#include <ddspp.h>

#include <anker/core/anker_data_loader.hpp>
#include <anker/platform/anker_window_win32.hpp>

namespace Anker {

const auto ShaderFileExtension = ".fxo";

static D3D11_SAMPLER_DESC convertSamplerDesc(const SamplerDesc& desc)
{
	auto filter = D3D11_FILTER_ANISOTROPIC;
	switch (desc.filterMode) {
	case FilterMode::Point: filter = D3D11_FILTER_MIN_MAG_MIP_POINT; break;
	case FilterMode::Linear: filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; break;
	case FilterMode::Anisotropic: filter = D3D11_FILTER_ANISOTROPIC; break;
	}

	return {
	    .Filter = D3D11_FILTER_ANISOTROPIC,
	    .AddressU = D3D11_TEXTURE_ADDRESS_MODE(desc.addressModeU),
	    .AddressV = D3D11_TEXTURE_ADDRESS_MODE(desc.addressModeV),
	    .AddressW = D3D11_TEXTURE_ADDRESS_MODE(desc.addressModeW),
	    .MaxAnisotropy = 16,
	    .ComparisonFunc = D3D11_COMPARISON_FUNC(desc.compareFunc),
	};
}

static Status loadTextureDDS(const std::string& name, std::span<uint8_t> ddsData, RenderDevice& device,
                             Texture& outTexture)
{
	ddspp::Descriptor ddsDesc;
	if (ddspp::decode_header(ddsData.data(), ddsDesc) != ddspp::Success) {
		// ANKER_FATAL("Could not load DDS texture");
		return FormatError;
	}
	const uint8_t* ddsDataBody = ddsData.data() + ddsDesc.headerSize;

	TextureInfo info{
	    .name = name,
	    .size = {ddsDesc.width, ddsDesc.height},
	    .mipLevels = ddsDesc.numMips,
	    .arraySize = ddsDesc.arraySize,
	    .format = static_cast<TextureFormat>(ddsDesc.format),
	};

	if (ddsDesc.type == ddspp::Cubemap) {
		info.arraySize *= 6;
		info.flags |= TextureFlag::Cubemap;
	}

	std::vector<TextureInit> inits;
	for (auto slice = 0u; slice < info.arraySize; ++slice) {
		for (auto mip = 0u; mip < info.mipLevels; ++mip) {
			inits.push_back({
			    .data = ddsDataBody + ddspp::get_offset(ddsDesc, mip, slice),
			    .rowPitch = ddspp::get_row_pitch(ddsDesc, mip),
			});
		}
	}

	return device.createTexture(info, outTexture, inits);
}

static Status loadTexturePNGorJPG(const std::string& name, std::span<uint8_t> imageData, RenderDevice& device,
                                  Texture& outTexture)
{
	Image image(imageData);
	if (!image) {
		return FormatError;
	}

	std::array inits = {TextureInit{
	    .data = image.pixels(),
	    .rowPitch = uint32_t(image.rowPitch()),
	}};

	return device.createTexture(
	    {
	        .name = name,
	        .size = {image.width(), image.height()},
	        .format = TextureFormat::R8G8B8A8_UNORM_SRGB,
	    },
	    outTexture, inits);
}

RenderDevice::RenderDevice(Window& window, DataLoader& dataLoader) : m_dataLoader(dataLoader)
{
	const D3D_FEATURE_LEVEL levels[] = {
	    D3D_FEATURE_LEVEL_11_0,
	    D3D_FEATURE_LEVEL_11_1,
	};

	const auto deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT //
	                       | D3D11_CREATE_DEVICE_DEBUG;

	HRESULT hresult = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, deviceFlags, levels, ARRAYSIZE(levels),
	                                    D3D11_SDK_VERSION, &m_device, nullptr, &m_context);
	if (FAILED(hresult)) {
		ANKER_FATAL("D3D11CreateDevice failed: {}", win32ErrorMessage(hresult));
	}

	m_device.As(&m_dxgiDevice);

	hresult = m_dxgiDevice->GetAdapter(&m_dxgiAdapter);
	if (FAILED(hresult)) {
		ANKER_FATAL("IDXGIDevice::GetAdapter failed: {}", win32ErrorMessage(hresult));
	}

	hresult = m_dxgiAdapter->GetParent(IID_PPV_ARGS(&m_dxgiFactory));
	if (FAILED(hresult)) {
		ANKER_FATAL("IDXGIObject::GetParent failed: {}", win32ErrorMessage(hresult));
	}

	DXGI_SWAP_CHAIN_DESC swapchainDesc{
	    // BGRA format is preferred as this format is common among display
	    // controllers.
	    .BufferDesc = {.Format = DXGI_FORMAT_B8G8R8A8_UNORM},
	    .SampleDesc = {.Count = 1},
	    .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
	    .BufferCount = 2,
	    .OutputWindow = window,
	    .Windowed = true,
	    .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
	    .Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING,
	};

	hresult = m_dxgiFactory->CreateSwapChain(m_device.Get(), &swapchainDesc, &m_dxgiSwapchain);
	if (FAILED(hresult)) {
		ANKER_FATAL("IDXGIFactory::CreateSwapChain failed: {}", win32ErrorMessage(hresult));
	}

	createMainRenderTarget();

	setRasterizer();

	if (not loadTexture("fallback/fallback_texture", m_fallbackTexture)) {
		ANKER_WARN("Fallback texture could not be loaded!");
	}
}

Status RenderDevice::createBuffer(const BufferInfo& info, Buffer& outBuffer, std::span<const uint8_t> init)
{
	outBuffer.info = info;
	outBuffer.info.size = std::max(info.size, uint32_t(init.size()));

	D3D11_BUFFER_DESC desc{
	    .ByteWidth = outBuffer.info.size,
	    .Usage = D3D11_USAGE_DEFAULT,
	};

	if (info.bindFlags & BindFlag::ConstantBuffer) {
		desc.BindFlags |= D3D11_BIND_CONSTANT_BUFFER;
	}
	if (info.bindFlags & BindFlag::VertexBuffer) {
		desc.BindFlags |= D3D11_BIND_VERTEX_BUFFER;
	}
	if (info.bindFlags & BindFlag::IndexBuffer) {
		desc.BindFlags |= D3D11_BIND_INDEX_BUFFER;
	}

	if (info.flags & BufferFlag::CpuWriteable) {
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
	}
	if (info.flags & BufferFlag::Structured) {
		desc.StructureByteStride = info.stride;
		desc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	}

	const D3D11_SUBRESOURCE_DATA dxInit{.pSysMem = init.data()};

	HRESULT hresult = m_device->CreateBuffer(&desc, init.empty() ? nullptr : &dxInit, &outBuffer.buffer);
	if (FAILED(hresult)) {
		ANKER_ERROR("{}: CreateBuffer failed: {}", info.name, win32ErrorMessage(hresult));
		return GraphicsError;
	}

	outBuffer.buffer->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(info.name.size()), info.name.data());
	return OK;
}

void RenderDevice::bindBufferVS(uint32_t slot, const Buffer& buffer)
{
	m_context->VSSetConstantBuffers(slot, 1, buffer.buffer.GetAddressOf());
}

void RenderDevice::bindBufferPS(uint32_t slot, const Buffer& buffer)
{
	m_context->PSSetConstantBuffers(slot, 1, buffer.buffer.GetAddressOf());
}

void* RenderDevice::mapBuffer(const Buffer& buffer)
{
	return mapResource(buffer.buffer.Get());
}

void RenderDevice::unmapBuffer(const Buffer& buffer)
{
	unmapResource(buffer.buffer.Get());
}

Status RenderDevice::loadVertexShader(std::string_view identifier,
                                      std::span<const D3D11_INPUT_ELEMENT_DESC> shaderInputs,
                                      VertexShader& outVertexShader)
{
	ANKER_PROFILE_ZONE_T(identifier);

	ByteBuffer binary;
	ANKER_TRY(m_dataLoader.load(std::string{identifier} + ShaderFileExtension, binary));

	return createVertexShader(identifier, binary, shaderInputs, outVertexShader);
}

Status RenderDevice::loadPixelShader(std::string_view identifier, PixelShader& outPixelShader)
{
	ANKER_PROFILE_ZONE_T(identifier);

	ByteBuffer binary;
	ANKER_TRY(m_dataLoader.load(std::string{identifier} + ShaderFileExtension, binary));

	return createPixelShader(identifier, binary, outPixelShader);
}

Status RenderDevice::createVertexShader(std::string_view identifier, std::span<const uint8_t> binary,
                                        std::span<const D3D11_INPUT_ELEMENT_DESC> shaderInputs,
                                        VertexShader& outVertexShader)
{
	ANKER_PROFILE_ZONE_T(identifier);

	HRESULT hresult = m_device->CreateVertexShader(binary.data(), binary.size(), nullptr, &outVertexShader.shader);
	if (FAILED(hresult)) {
		ANKER_ERROR("{}: CreateVertexShader failed: {}", identifier, win32ErrorMessage(hresult));
		return GraphicsError;
	}

	outVertexShader.inputLayoutDesc.assign(shaderInputs.begin(), shaderInputs.end());
	if (!shaderInputs.empty()) {
		hresult = m_device->CreateInputLayout(shaderInputs.data(), UINT(shaderInputs.size()), //
		                                      binary.data(), binary.size(),                   //
		                                      &outVertexShader.inputLayout);
		if (FAILED(hresult)) {
			ANKER_ERROR("{}: CreateInputLayout failed: {}", identifier, win32ErrorMessage(hresult));
			return GraphicsError;
		}
	} else {
		outVertexShader.inputLayout.Reset();
	}

	outVertexShader.shader->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(identifier.size()), identifier.data());

	return OK;
}

Status RenderDevice::createPixelShader(std::string_view identifier, std::span<const uint8_t> binary,
                                       PixelShader& outPixelShader)
{
	ANKER_PROFILE_ZONE_T(identifier);

	HRESULT hresult = m_device->CreatePixelShader(binary.data(), binary.size(), //
	                                              nullptr, &outPixelShader.shader);
	if (FAILED(hresult)) {
		ANKER_ERROR("{}: CreatePixelShader failed: {}", identifier, win32ErrorMessage(hresult));
		return GraphicsError;
	}

	outPixelShader.shader->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(identifier.size()), identifier.data());

	return OK;
}

void RenderDevice::bindVertexShader(const VertexShader& vertexShader)
{
	m_context->IASetInputLayout(vertexShader.inputLayout.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_context->VSSetShader(vertexShader.shader.Get(), nullptr, 0);
}

void RenderDevice::bindPixelShader(const PixelShader& pixelShader)
{
	m_context->PSSetShader(pixelShader.shader.Get(), nullptr, 0);
}

Status RenderDevice::loadTexture(std::string_view identifier_, Texture& outTexture)
{
	ANKER_PROFILE_ZONE_T(identifier_);

	auto identifier = std::string{identifier_};

	ByteBuffer textureData;

	if (m_dataLoader.load(identifier + ".dds", textureData)) {
		if (loadTextureDDS(identifier, textureData, *this, outTexture)) {
			return OK;
		}
	}
	if (m_dataLoader.load(identifier + ".png", textureData)) {
		if (loadTexturePNGorJPG(identifier, textureData, *this, outTexture)) {
			return OK;
		}
	}
	if (m_dataLoader.load(identifier + ".jpg", textureData)) {
		if (loadTexturePNGorJPG(identifier, textureData, *this, outTexture)) {
			return OK;
		}
	}

	ANKER_ERROR("{}: Missing, using fallback!", identifier);
	outTexture = fallbackTexture();
	return ReadError;
}

Status RenderDevice::createTexture(const TextureInfo& info, Texture& outTexture, std::span<const TextureInit> inits)
{
	outTexture = {.info = info};

	D3D11_TEXTURE2D_DESC desc{
	    .Width = info.size.x,
	    .Height = info.size.y,
	    .MipLevels = info.mipLevels,
	    .ArraySize = info.arraySize,
	    .Format = static_cast<DXGI_FORMAT>(info.format),
	    .SampleDesc = {.Count = 1},
	    .Usage = D3D11_USAGE_DEFAULT,
	};

	if (info.bindFlags & BindFlag::Shader) {
		desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}
	if (info.bindFlags & BindFlag::RenderTarget) {
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	}
	if (info.bindFlags & BindFlag::DepthStencil) {
		desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	}

	if (info.flags & TextureFlag::CpuWriteable) {
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
	}
	if (info.flags & TextureFlag::Cubemap) {
		desc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
	}

	std::vector<D3D11_SUBRESOURCE_DATA> dxInits;
	for (const auto& init : inits) {
		dxInits.push_back(D3D11_SUBRESOURCE_DATA{
		    .pSysMem = init.data,
		    .SysMemPitch = init.rowPitch,
		});
	}

	HRESULT hresult = m_device->CreateTexture2D(&desc, dxInits.empty() ? nullptr : dxInits.data(), &outTexture.texture);
	if (FAILED(hresult)) {
		ANKER_ERROR("{}: CreateTexture2D failed: {}", info.name, win32ErrorMessage(hresult));
		return GraphicsError;
	}

	if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE) {
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc{
		    .Format = desc.Format,
		    .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
		    .Texture2D = {.MipLevels = desc.MipLevels},
		};

		if (info.flags & TextureFlag::Cubemap) {
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			viewDesc.TextureCube = {.MipLevels = desc.MipLevels};
		}

		hresult = m_device->CreateShaderResourceView(outTexture.texture.Get(), &viewDesc, &outTexture.shaderView);
		if (FAILED(hresult)) {
			ANKER_ERROR("{}: CreateShaderResourceView failed: {}", info.name, win32ErrorMessage(hresult));
			return GraphicsError;
		}
	}

	if (desc.BindFlags & D3D11_BIND_RENDER_TARGET) {
		hresult = m_device->CreateRenderTargetView(outTexture.texture.Get(), nullptr, &outTexture.renderTargetView);
		if (FAILED(hresult)) {
			ANKER_ERROR("{}: CreateRenderTargetView failed: {}", info.name, win32ErrorMessage(hresult));
			return GraphicsError;
		}
	}

	if (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL) {
		const D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc{
		    .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
		};
		hresult = m_device->CreateDepthStencilView(outTexture.texture.Get(), &viewDesc, &outTexture.depthView);
		if (FAILED(hresult)) {
			ANKER_ERROR("{}: CreateDepthStencilView failed: {}", info.name, win32ErrorMessage(hresult));
			return GraphicsError;
		}
	}

	outTexture.texture->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(info.name.size()), info.name.data());
	return OK;
}

void RenderDevice::bindTexturePS(uint32_t slot, const Texture& texture, const SamplerDesc& samplerDesc)
{
	auto* samplerState = samplerStateFromDesc(samplerDesc);
	m_context->PSSetSamplers(slot, 1, &samplerState);
	m_context->PSSetShaderResources(slot, 1, texture.shaderView.GetAddressOf());
}

void RenderDevice::unbindTexturePS(uint32_t slot)
{
	ID3D11ShaderResourceView* none = nullptr;
	m_context->PSSetShaderResources(slot, 1, &none);
}

std::byte* RenderDevice::mapTexture(const Texture& texture, uint32_t* outRowPitch)
{
	ANKER_CHECK(outRowPitch);
	return static_cast<std::byte*>(mapResource(texture.texture.Get(), outRowPitch));
}

void RenderDevice::unmapTexture(const Texture& texture)
{
	unmapResource(texture.texture.Get());
}

void RenderDevice::setRasterizer(const RasterizerDesc& desc)
{
	auto* r = rasterizerStateFromDesc(desc);
	m_context->RSSetState(r);
}

void RenderDevice::setRenderTarget(const Texture& target, const Texture* depth)
{
	m_context->OMSetRenderTargets(1, target.renderTargetView.GetAddressOf(), //
	                              depth ? depth->depthView.Get() : nullptr);
}

void RenderDevice::clearRenderTarget(const Texture& target, const Texture* depth, const Vec3& clearColor)
{
	m_context->ClearRenderTargetView(target.renderTargetView.Get(), value_ptr(clearColor));
	if (depth) {
		m_context->ClearDepthStencilView(depth->depthView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void RenderDevice::bindRenderTargetPS(uint32_t slot, const Texture& texture, const SamplerDesc& samplerDesc)
{
	auto* samplerState = samplerStateFromDesc(samplerDesc);
	m_context->PSSetSamplers(slot, 1, &samplerState);
	m_context->PSSetShaderResources(slot, 1, texture.shaderView.GetAddressOf());
}

void RenderDevice::draw(uint32_t vertexCount)
{
	m_context->Draw(vertexCount, 0);
}

void RenderDevice::draw(const Buffer& vertexBuffer, uint32_t vertexCount, Topology topology)
{
	UINT offset = 0;
	m_context->IASetVertexBuffers(0, 1, vertexBuffer.buffer.GetAddressOf(), &vertexBuffer.info.stride, &offset);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY(topology));
	m_context->Draw(vertexCount, 0);
}

void RenderDevice::draw(const Buffer& vertexBuffer, const Buffer& indexBuffer, uint32_t indexCount, Topology topology)
{
	UINT offset = 0;
	m_context->IASetVertexBuffers(0, 1, vertexBuffer.buffer.GetAddressOf(), &vertexBuffer.info.stride, &offset);
	m_context->IASetIndexBuffer(indexBuffer.buffer.Get(),                                                   //
	                            indexBuffer.info.stride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, //
	                            0);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY(topology));
	m_context->DrawIndexed(indexCount, 0, 0);
}

void RenderDevice::present()
{
	m_dxgiSwapchain->Present(1, 0);
	// m_dxgiSwapchain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
}

void RenderDevice::onResize(Vec2i)
{
	// Need to release reference to backBuffer before resizing swap chain.
	m_backBuffer.texture.Reset();
	m_backBuffer.renderTargetView.Reset();

	HRESULT hresult = m_dxgiSwapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
	if (FAILED(hresult)) {
		std::abort();
	}

	createMainRenderTarget();
}

void RenderDevice::createMainRenderTarget()
{
	// Grab back buffer from swap chain.
	HRESULT hresult = m_dxgiSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), &m_backBuffer.texture);
	if (FAILED(hresult)) {
		std::abort();
	}

	{
		D3D11_TEXTURE2D_DESC desc;
		m_backBuffer.texture->GetDesc(&desc);
		m_backBufferSize = {desc.Width, desc.Height};
	}

	// Setup back buffer view.
	hresult = m_device->CreateRenderTargetView(m_backBuffer.texture.Get(), nullptr, &m_backBuffer.renderTargetView);
	if (FAILED(hresult)) {
		std::abort();
	}

	// Set Viewport
	const D3D11_VIEWPORT viewportParams{
	    .Width = float(m_backBufferSize.x),
	    .Height = float(m_backBufferSize.y),
	    .MaxDepth = 1,
	};
	m_context->RSSetViewports(1, &viewportParams);
}

ID3D11SamplerState* RenderDevice::samplerStateFromDesc(const SamplerDesc& desc)
{
	if (auto it = m_samplerStates.find(desc); it != m_samplerStates.end()) {
		return it->second.Get();
	}

	ComPtr<ID3D11SamplerState> sampler;
	const auto d3d11Desc = convertSamplerDesc(desc);
	HRESULT hresult = m_device->CreateSamplerState(&d3d11Desc, &sampler);
	if (FAILED(hresult)) {
		std::abort();
	}

	m_samplerStates[desc] = sampler;
	return sampler.Get();
}

ID3D11RasterizerState* RenderDevice::rasterizerStateFromDesc(const RasterizerDesc& desc)
{
	if (auto it = m_rasterizerStates.find(desc); it != m_rasterizerStates.end()) {
		return it->second.Get();
	}

	ComPtr<ID3D11RasterizerState> rasterizer;
	const D3D11_RASTERIZER_DESC d3d11Desc{
	    .FillMode = desc.wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID,
	    .CullMode = D3D11_CULL_BACK,
	    .FrontCounterClockwise = true,
	    .DepthClipEnable = desc.depthClip,
	};
	HRESULT hresult = m_device->CreateRasterizerState(&d3d11Desc, &rasterizer);
	if (FAILED(hresult)) {
		std::abort();
	}

	m_rasterizerStates[desc] = rasterizer;
	return rasterizer.Get();
}

void* RenderDevice::mapResource(ID3D11Resource* resource, uint32_t* outRowPitch, uint32_t* outDepthPitch)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hresult = m_context->Map(resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hresult)) {
		std::abort();
	}
	if (outRowPitch) {
		*outRowPitch = mappedResource.RowPitch;
	}
	if (outDepthPitch) {
		*outDepthPitch = mappedResource.DepthPitch;
	}
	return mappedResource.pData;
}

void RenderDevice::unmapResource(ID3D11Resource* resource)
{
	m_context->Unmap(resource, 0);
}

} // namespace Anker
