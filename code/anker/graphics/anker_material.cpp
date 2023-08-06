#include "anker_material.hpp"

namespace Anker {

void Material::bind(RenderDevice& device) const
{
	device.bindVertexShader(*vertexShader);
	device.bindPixelShader(*pixelShader);

	for (auto i = 0; i < textures.size(); ++i) {
		device.bindTexturePS(i, *textures[i].texture, textures[i].sampler);
	}
}

void Material::unbind(RenderDevice& device) const
{
	for (auto i = 0; i < textures.size(); ++i) {
		device.unbindTexturePS(i);
	}
}

} // namespace Anker
