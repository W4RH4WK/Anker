#pragma once

#include <anker/core/anker_asset.hpp>

#include "anker_render_device.hpp"

namespace Anker {

// While textures can be shared across multiple materials, a different sampler
// may be needed, depending on the material.
struct MaterialTexture {
	AssetPtr<Texture> texture;
	SamplerDesc sampler;
};

struct Material {
	AssetPtr<VertexShader> vertexShader;
	AssetPtr<PixelShader> pixelShader;

	std::vector<MaterialTexture> textures;

	void bind(RenderDevice&) const;
	void unbind(RenderDevice&) const;
};

} // namespace Anker
