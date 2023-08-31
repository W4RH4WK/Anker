#pragma once

#include <anker/editor/anker_edit_widget_drawer.hpp>

namespace Anker {

struct Transform2D {
	explicit Transform2D(Vec2 position = Vec2::Zero, float rotation = 0, Vec2 scale = Vec2::One)
	    : position(position), rotation(rotation), scale(scale)
	{}

	// GLM conversion
	Transform2D(const Mat3& mat)
	{
		Mat2 rotationMat = Mat2(mat);
		position = glm::vec2(mat[2]);
		rotation = std::atan2(rotationMat[1][0], rotationMat[0][0]);
		scale = {length(rotationMat[0]), length(rotationMat[1])};
	}
	operator Mat3() const
	{
		Mat3 mat = Mat3Id;
		mat = glm::translate(mat, glm::vec2(position));
		mat = glm::rotate(mat, rotation);
		mat = glm::scale(mat, glm::vec2(scale));
		return mat;
	}

	// Box2D conversion
	Transform2D(b2Transform transform) : Transform2D(transform.p, transform.q.GetAngle()) {}
	operator b2Transform() const { return b2Transform(position, b2Rot(rotation)); }

	Vec2 position = Vec2::Zero;
	float rotation = 0;
	Vec2 scale = Vec2::One;

	uint32_t layer = 0;
};

inline Transform2D inverse(Transform2D transform)
{
	transform.position *= -1.0f;
	transform.rotation *= -1.0f;
	transform.scale = 1.0f / transform.scale;
	return transform;
}

inline Vec2 operator*(const Transform2D& transform, Vec2 v)
{
	v *= transform.scale;
	v.rotate(transform.rotation);
	v += transform.position;
	return v;
}

inline Vec2 operator*(Vec2 v, const Transform2D& transform)
{
	v += transform.position;
	v.rotate(transform.rotation);
	v *= transform.scale;
	return v;
}

} // namespace Anker

REFL_TYPE(Anker::Transform2D)
REFL_FIELD(position)
REFL_FIELD(rotation, Anker::attr::Radians())
REFL_FIELD(scale)
REFL_END
