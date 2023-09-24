#pragma once

namespace Anker {

// Generally, each renderer can use its own vertex format; however, for 2D
// rendering this is the common case.
struct Vertex2D {
	Vec2 position;
	Vec2 uv;

	static const std::array<D3D11_INPUT_ELEMENT_DESC, 2> ShaderInputs;

	static std::array<Vertex2D, 6> makeQuad(const Rect2& position, const Rect2& uv);
};

} // namespace Anker
