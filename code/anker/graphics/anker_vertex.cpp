#include <anker/graphics/anker_vertex.hpp>

namespace Anker {

const std::array<D3D11_INPUT_ELEMENT_DESC, 2> Vertex2D::ShaderInputs = {
    D3D11_INPUT_ELEMENT_DESC{
        .SemanticName = "POSITION",
        .Format = DXGI_FORMAT_R32G32_FLOAT,
        .AlignedByteOffset = offsetof(Vertex2D, position),
        .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
    },
    D3D11_INPUT_ELEMENT_DESC{
        .SemanticName = "TEXCOORD",
        .SemanticIndex = 0,
        .Format = DXGI_FORMAT_R32G32_FLOAT,
        .AlignedByteOffset = offsetof(Vertex2D, uv),
        .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
    },
};

std::array<Vertex2D, 6> Vertex2D::makeQuad(const Rect2& position, const Rect2& uv)
{
	return {
	    Vertex2D{.position = position.topLeftWorld(), .uv = uv.topLeft()},
	    Vertex2D{.position = position.bottomLeftWorld(), .uv = uv.bottomLeft()},
	    Vertex2D{.position = position.topRightWorld(), .uv = uv.topRight()},
	    Vertex2D{.position = position.topRightWorld(), .uv = uv.topRight()},
	    Vertex2D{.position = position.bottomLeftWorld(), .uv = uv.bottomLeft()},
	    Vertex2D{.position = position.bottomRightWorld(), .uv = uv.bottomRight()},
	};
}

} // namespace Anker
