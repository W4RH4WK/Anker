#pragma once

namespace Anker {

// Generally, each renderer can use its own vertex format; however, for 2D
// rendering this is the common case.
struct Vertex2D {
	Vec2 position;
	Vec2 uv;

	static const std::array<D3D11_INPUT_ELEMENT_DESC, 2> ShaderInputs;
};

inline const std::array<D3D11_INPUT_ELEMENT_DESC, 2> Vertex2D::ShaderInputs = {
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

} // namespace Anker
