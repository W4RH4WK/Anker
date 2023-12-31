#include <anker/graphics/anker_render_device.hpp>

#include <imgui_impl_dx11.h>

#include <ddspp.h>

#include <anker/core/anker_data_loader.hpp>
#include <anker/platform/anker_platform.hpp>

namespace Anker {

const auto ShaderFileExtension = ".fxo";

static D3D11_SAMPLER_DESC convertSamplerDesc(const SamplerDesc& desc)
{
	auto filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	switch (desc.filterMode) {
	case FilterMode::Point: filter = D3D11_FILTER_MIN_MAG_MIP_POINT; break;
	case FilterMode::Linear: filter = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR; break;
	}

	return {
	    .Filter = filter,
	    .AddressU = D3D11_TEXTURE_ADDRESS_MODE(desc.addressModeU),
	    .AddressV = D3D11_TEXTURE_ADDRESS_MODE(desc.addressModeV),
	    .AddressW = D3D11_TEXTURE_ADDRESS_MODE(desc.addressModeW),
	    .ComparisonFunc = D3D11_COMPARISON_FUNC(desc.compareFunc),
	};
}

static Status createTextureFromDDS(Texture& texture, std::span<u8> ddsData, RenderDevice& device)
{
	ddspp::Descriptor ddsDesc;
	if (ddspp::decode_header(ddsData.data(), ddsDesc) != ddspp::Success) {
		ANKER_ERROR("{}: Invalid image format", texture.info.name);
		return FormatError;
	}
	const u8* ddsDataBody = ddsData.data() + ddsDesc.headerSize;

	texture.info.size = {ddsDesc.width, ddsDesc.height};
	texture.info.mipLevels = ddsDesc.numMips;
	texture.info.arraySize = ddsDesc.arraySize;
	texture.info.format = static_cast<TextureFormat>(ddsDesc.format);

	if (ddsDesc.type == ddspp::Cubemap) {
		texture.info.arraySize *= 6;
		texture.info.flags |= TextureFlag::Cubemap;
	}

	std::vector<TextureInit> inits;
	for (auto slice = 0u; slice < texture.info.arraySize; ++slice) {
		for (auto mip = 0u; mip < texture.info.mipLevels; ++mip) {
			inits.push_back({
			    .data = ddsDataBody + ddspp::get_offset(ddsDesc, mip, slice),
			    .rowPitch = ddspp::get_row_pitch(ddsDesc, mip),
			});
		}
	}

	return device.createTexture(texture, inits);
}

static Status createTextureFromPNGorJPG(Texture& texture, std::span<u8> imageData, RenderDevice& device)
{
	Image image(imageData);
	if (!image) {
		ANKER_ERROR("{}: Invalid image format", texture.info.name);
		return FormatError;
	}

	texture.info.size = {unsigned(image.width()), unsigned(image.height())};
	texture.info.format = TextureFormat::R8G8B8A8_UNORM;

	const std::array inits = {
	    TextureInit{
	        .data = image.pixels(),
	        .rowPitch = u32(image.rowPitch()),
	    },
	};

	return device.createTexture(texture, inits);
}

RenderDevice::RenderDevice()
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
	    .OutputWindow = Platform::nativeWindow(),
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

	if (not loadTexture(m_fallbackTexture, "fallback/fallback_texture")) {
		ANKER_ERROR("Fallback texture could not be loaded!");
	}
}

Status RenderDevice::createBuffer(GpuBuffer& buffer, std::span<const u8> init)
{
	buffer.info.size = std::max(buffer.info.size, u32(init.size()));
	ANKER_CHECK(buffer.info.size != 0, InvalidArgumentError);

	buffer.buffer.Reset();

	D3D11_BUFFER_DESC desc{
	    .ByteWidth = buffer.info.size,
	    .Usage = D3D11_USAGE_DEFAULT,
	};

	if (buffer.info.bindFlags & GpuBindFlag::ConstantBuffer) {
		desc.BindFlags |= D3D11_BIND_CONSTANT_BUFFER;
	}
	if (buffer.info.bindFlags & GpuBindFlag::VertexBuffer) {
		desc.BindFlags |= D3D11_BIND_VERTEX_BUFFER;
	}
	if (buffer.info.bindFlags & GpuBindFlag::IndexBuffer) {
		desc.BindFlags |= D3D11_BIND_INDEX_BUFFER;
	}

	if (buffer.info.flags & GpuBufferFlag::CpuWriteable) {
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
	}
	if (buffer.info.flags & GpuBufferFlag::Structured) {
		desc.StructureByteStride = buffer.info.stride;
		desc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	}

	const D3D11_SUBRESOURCE_DATA dxInit{.pSysMem = init.data()};

	HRESULT hresult = m_device->CreateBuffer(&desc, init.empty() ? nullptr : &dxInit, &buffer.buffer);
	if (FAILED(hresult)) {
		ANKER_ERROR("{}: CreateBuffer failed: {}", buffer.info.name, win32ErrorMessage(hresult));
		return GraphicsError;
	}

	buffer.buffer->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(buffer.info.name.size()), buffer.info.name.data());
	return Ok;
}

void RenderDevice::bindBufferVS(u32 slot, const GpuBuffer& buffer)
{
	m_context->VSSetConstantBuffers(slot, 1, buffer.buffer.GetAddressOf());
}

void RenderDevice::bindBufferPS(u32 slot, const GpuBuffer& buffer)
{
	m_context->PSSetConstantBuffers(slot, 1, buffer.buffer.GetAddressOf());
}

void RenderDevice::unmapBuffer(GpuBuffer& buffer)
{
	unmapResource(buffer.buffer.Get());
}

Status RenderDevice::loadVertexShader(VertexShader& vertexShader, std::string_view identifier)
{
	ANKER_PROFILE_ZONE_T(identifier);

	vertexShader.info.name = identifier;
	vertexShader.shader.Reset();
	vertexShader.inputLayout.Reset();

	ByteBuffer binary;
	ANKER_TRY(g_assetDataLoader.load(binary, std::string{identifier} + ShaderFileExtension));

	HRESULT hresult = m_device->CreateVertexShader(binary.data(), binary.size(), nullptr, &vertexShader.shader);
	if (FAILED(hresult)) {
		ANKER_ERROR("{}: CreateVertexShader failed: {}", identifier, win32ErrorMessage(hresult));
		return GraphicsError;
	}

	if (!vertexShader.info.inputs.empty()) {
		hresult = m_device->CreateInputLayout(vertexShader.info.inputs.data(), UINT(vertexShader.info.inputs.size()),
		                                      binary.data(), binary.size(), &vertexShader.inputLayout);
		if (FAILED(hresult)) {
			ANKER_ERROR("{}: CreateInputLayout failed: {}", identifier, win32ErrorMessage(hresult));
			return GraphicsError;
		}
	}

	vertexShader.shader->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(identifier.size()), identifier.data());

	return Ok;
}

Status RenderDevice::loadPixelShader(PixelShader& pixelShader, std::string_view identifier)
{
	ANKER_PROFILE_ZONE_T(identifier);

	pixelShader.info.name = identifier;
	pixelShader.shader.Reset();

	ByteBuffer binary;
	ANKER_TRY(g_assetDataLoader.load(binary, std::string{identifier} + ShaderFileExtension));

	HRESULT hresult = m_device->CreatePixelShader(binary.data(), binary.size(), nullptr, &pixelShader.shader);
	if (FAILED(hresult)) {
		ANKER_ERROR("{}: CreatePixelShader failed: {}", identifier, win32ErrorMessage(hresult));
		return GraphicsError;
	}

	pixelShader.shader->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(identifier.size()), identifier.data());

	return Ok;
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

Status RenderDevice::loadTexture(Texture& texture, std::string_view identifier)
{
	ANKER_PROFILE_ZONE_T(identifier);

	texture.info.name = identifier;
	texture.info.size = m_fallbackTexture.info.size;
	texture.texture.Reset();
	texture.shaderView.Reset();
	texture.renderTargetView.Reset();
	texture.depthView.Reset();

	const auto loaders = {
	    std::pair(".dds", createTextureFromDDS),
	    std::pair(".png", createTextureFromPNGorJPG),
	    std::pair(".jpg", createTextureFromPNGorJPG),
	};
	for (auto [ext, loader] : loaders) {
		auto filepath = std::string(identifier) + ext;
		if (g_assetDataLoader.exists(filepath)) {
			ByteBuffer textureData;
			ANKER_TRY(g_assetDataLoader.load(textureData, filepath));
			return loader(texture, textureData, *this);
		}
	}

	ANKER_ERROR("{}: Missing!", identifier);
	return ReadError;
}

Status RenderDevice::createTexture(Texture& texture, std::span<const TextureInit> inits)
{
	ANKER_CHECK(texture.info.size != Vec2u(0), InvalidArgumentError);

	D3D11_TEXTURE2D_DESC desc{
	    .Width = texture.info.size.x,
	    .Height = texture.info.size.y,
	    .MipLevels = texture.info.mipLevels,
	    .ArraySize = texture.info.arraySize,
	    .Format = static_cast<DXGI_FORMAT>(texture.info.format),
	    .SampleDesc = {.Count = 1},
	    .Usage = D3D11_USAGE_DEFAULT,
	};

	if (texture.info.bindFlags & GpuBindFlag::Shader) {
		desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}
	if (texture.info.bindFlags & GpuBindFlag::RenderTarget) {
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	}
	if (texture.info.bindFlags & GpuBindFlag::DepthStencil) {
		desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	}

	if (texture.info.flags & TextureFlag::CpuWriteable) {
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
	}
	if (texture.info.flags & TextureFlag::Cubemap) {
		desc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
	}

	std::vector<D3D11_SUBRESOURCE_DATA> dxInits;
	for (const auto& init : inits) {
		dxInits.push_back(D3D11_SUBRESOURCE_DATA{
		    .pSysMem = init.data,
		    .SysMemPitch = init.rowPitch,
		});
	}

	HRESULT hresult = m_device->CreateTexture2D(&desc, dxInits.empty() ? nullptr : dxInits.data(), &texture.texture);
	if (FAILED(hresult)) {
		ANKER_ERROR("{}: CreateTexture2D failed: {}", texture.info.name, win32ErrorMessage(hresult));
		return GraphicsError;
	}

	if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE) {
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc{
		    .Format = desc.Format,
		    .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
		    .Texture2D = {.MipLevels = UINT(-1)},
		};

		if (texture.info.flags & TextureFlag::Cubemap) {
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			viewDesc.TextureCube = {.MipLevels = desc.MipLevels};
		}

		hresult = m_device->CreateShaderResourceView(texture.texture.Get(), &viewDesc, &texture.shaderView);
		if (FAILED(hresult)) {
			ANKER_ERROR("{}: CreateShaderResourceView failed: {}", texture.info.name, win32ErrorMessage(hresult));
			return GraphicsError;
		}
	}

	if (desc.BindFlags & D3D11_BIND_RENDER_TARGET) {
		hresult = m_device->CreateRenderTargetView(texture.texture.Get(), nullptr, &texture.renderTargetView);
		if (FAILED(hresult)) {
			ANKER_ERROR("{}: CreateRenderTargetView failed: {}", texture.info.name, win32ErrorMessage(hresult));
			return GraphicsError;
		}
	}

	if (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL) {
		const D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc{
		    .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
		};
		hresult = m_device->CreateDepthStencilView(texture.texture.Get(), &viewDesc, &texture.depthView);
		if (FAILED(hresult)) {
			ANKER_ERROR("{}: CreateDepthStencilView failed: {}", texture.info.name, win32ErrorMessage(hresult));
			return GraphicsError;
		}
	}

	texture.texture->SetPrivateData(WKPDID_D3DDebugObjectName, //
	                                UINT(texture.info.name.size()), texture.info.name.data());
	return Ok;
}

void RenderDevice::bindTexturePS(u32 slot, const Texture& texture, const SamplerDesc& samplerDesc)
{
	auto* samplerState = samplerStateFromDesc(samplerDesc);
	m_context->PSSetSamplers(slot, 1, &samplerState);

	if (texture.shaderView) {
		m_context->PSSetShaderResources(slot, 1, texture.shaderView.GetAddressOf());
	} else {
		m_context->PSSetShaderResources(slot, 1, m_fallbackTexture.shaderView.GetAddressOf());
	}
}

void RenderDevice::unbindTexturePS(u32 slot)
{
	ID3D11ShaderResourceView* none = nullptr;
	m_context->PSSetShaderResources(slot, 1, &none);
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
	m_context->ClearRenderTargetView(target.renderTargetView.Get(), &clearColor.x);
	if (depth) {
		m_context->ClearDepthStencilView(depth->depthView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void RenderDevice::enableAlphaBlending()
{
	if (!m_alphaBlendState) {
		D3D11_BLEND_DESC blendStateDesc{};
		blendStateDesc.RenderTarget[0] = {
		    .BlendEnable = true,
		    .SrcBlend = D3D11_BLEND_SRC_ALPHA,
		    .DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
		    .BlendOp = D3D11_BLEND_OP_ADD,
		    .SrcBlendAlpha = D3D11_BLEND_ONE,
		    .DestBlendAlpha = D3D11_BLEND_ZERO,
		    .BlendOpAlpha = D3D11_BLEND_OP_ADD,
		    .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
		};
		m_device->CreateBlendState(&blendStateDesc, &m_alphaBlendState);
	}

	m_context->OMSetBlendState(m_alphaBlendState.Get(), 0, 0xffffffff);
}

void RenderDevice::bindRenderTargetPS(u32 slot, const Texture& texture, const SamplerDesc& samplerDesc)
{
	auto* samplerState = samplerStateFromDesc(samplerDesc);
	m_context->PSSetSamplers(slot, 1, &samplerState);
	m_context->PSSetShaderResources(slot, 1, texture.shaderView.GetAddressOf());
}

void RenderDevice::draw(u32 vertexCount, Topology topology)
{
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY(topology));
	m_context->Draw(vertexCount, 0);
}

void RenderDevice::draw(const GpuBuffer& vertexBuffer, Topology topology)
{
	draw(vertexBuffer, vertexBuffer.info.elementCount(), topology);
}

void RenderDevice::draw(const GpuBuffer& vertexBuffer, u32 vertexCount, Topology topology)
{
	UINT offset = 0;
	m_context->IASetVertexBuffers(0, 1, vertexBuffer.buffer.GetAddressOf(), &vertexBuffer.info.stride, &offset);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY(topology));
	m_context->Draw(vertexCount, 0);
}

void RenderDevice::draw(const GpuBuffer& vertexBuffer, const GpuBuffer& indexBuffer, u32 indexCount, Topology topology)
{
	UINT offset = 0;
	m_context->IASetVertexBuffers(0, 1, vertexBuffer.buffer.GetAddressOf(), &vertexBuffer.info.stride, &offset);
	m_context->IASetIndexBuffer(indexBuffer.buffer.Get(),                                                   //
	                            indexBuffer.info.stride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, //
	                            0);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY(topology));
	m_context->DrawIndexed(indexCount, 0, 0);
}

void RenderDevice::drawInstanced(u32 vertexCount, u32 instanceCount)
{
	m_context->DrawInstanced(vertexCount, instanceCount, 0, 0);
}

void RenderDevice::drawInstanced(const GpuBuffer& vertexBuffer, u32 vertexCount,     //
                                 const GpuBuffer& instanceBuffer, u32 instanceCount, //
                                 Topology topology)
{
	std::array buffers{vertexBuffer.buffer.Get(), instanceBuffer.buffer.Get()};
	std::array strides{vertexBuffer.info.stride, instanceBuffer.info.stride};
	std::array offsets{0u, 0u};
	m_context->IASetVertexBuffers(0, 2, buffers.data(), strides.data(), offsets.data());

	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY(topology));
	m_context->DrawInstanced(vertexCount, instanceCount, 0, 0);
}

void RenderDevice::drawInstanced(const GpuBuffer& vertexBuffer,                      //
                                 const GpuBuffer& indexBuffer, u32 indexCount,       //
                                 const GpuBuffer& instanceBuffer, u32 instanceCount, //
                                 Topology topology)
{
	std::array buffers{vertexBuffer.buffer.Get(), instanceBuffer.buffer.Get()};
	std::array strides{vertexBuffer.info.stride, instanceBuffer.info.stride};
	std::array offsets{0u, 0u};
	m_context->IASetVertexBuffers(0, 2, buffers.data(), strides.data(), offsets.data());

	m_context->IASetIndexBuffer(indexBuffer.buffer.Get(),                                                   //
	                            indexBuffer.info.stride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, //
	                            0);

	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY(topology));
	m_context->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0);
}

void RenderDevice::imguiImplInit()
{
	ImGui_ImplDX11_Init(m_device.Get(), m_context.Get());
}

void RenderDevice::imguiImplShutdown()
{
	ImGui_ImplDX11_Shutdown();
}

void RenderDevice::imguiImplNewFrame()
{
	ImGui_ImplDX11_NewFrame();
}

void RenderDevice::imguiImplRender()
{
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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
		ANKER_FATAL("IDXGISwapChain::ResizeBuffers failed: {}", win32ErrorMessage(hresult));
	}

	createMainRenderTarget();
}

void RenderDevice::createMainRenderTarget()
{
	// Grab back buffer from swap chain
	HRESULT hresult = m_dxgiSwapchain->GetBuffer(0, IID_ID3D11Texture2D, &m_backBuffer.texture);
	if (FAILED(hresult)) {
		ANKER_FATAL("IDXGISwapChain::GetBuffer failed: {}", win32ErrorMessage(hresult));
	}

	// Update TextureInfo
	{
		D3D11_TEXTURE2D_DESC desc;
		m_backBuffer.texture->GetDesc(&desc);

		m_backBuffer.info = TextureInfo{
		    .name = "Back Buffer",
		    .size = {desc.Width, desc.Height},
		    .mipLevels = desc.MipLevels,
		    .arraySize = desc.ArraySize,
		    .format = TextureFormat(desc.Format),
		};
	}

	// Setup back buffer view
	hresult = m_device->CreateRenderTargetView(m_backBuffer.texture.Get(), nullptr, &m_backBuffer.renderTargetView);
	if (FAILED(hresult)) {
		ANKER_FATAL("ID3D11Device::CreateRenderTargetView failed: {}", win32ErrorMessage(hresult));
	}

	// Set Viewport
	const D3D11_VIEWPORT viewportParams{
	    .Width = float(m_backBuffer.info.size.x),
	    .Height = float(m_backBuffer.info.size.y),
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
		ANKER_FATAL("ID3D11Device::CreateSamplerState failed: {}", win32ErrorMessage(hresult));
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
	    .CullMode = D3D11_CULL_NONE,
	    .DepthClipEnable = desc.depthClip,
	};
	HRESULT hresult = m_device->CreateRasterizerState(&d3d11Desc, &rasterizer);
	if (FAILED(hresult)) {
		ANKER_FATAL("ID3D11Device::CreateRasterizerState failed: {}", win32ErrorMessage(hresult));
	}

	m_rasterizerStates[desc] = rasterizer;
	return rasterizer.Get();
}

void* RenderDevice::mapResource(ID3D11Resource* resource, u32* outRowPitch, u32* outDepthPitch)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hresult = m_context->Map(resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hresult)) {
		ANKER_FATAL("ID3D11DeviceContext::Map failed: {}", win32ErrorMessage(hresult));
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
