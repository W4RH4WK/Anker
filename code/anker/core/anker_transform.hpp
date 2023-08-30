#pragma once

#include <anker/editor/anker_edit_widget_drawer.hpp>

namespace Anker {

struct Transform2D {
	Vec2 position = Vec2(0);
	float rotation = 0;
	Vec2 scale = Vec2(1);

	uint32_t layer = 0;

	static Transform2D fromMat3(const Mat4& mat)
	{
		Mat2 rotationMat = Mat2(mat);
		return Transform2D{
		    .position = mat[2],
		    .rotation = std::atan2(rotationMat[1][0], rotationMat[0][0]),
		    .scale = {length(rotationMat[0]), length(rotationMat[1])},
		};
	}

	Mat3 mat3() const
	{
		Mat3 mat = Mat3Id;
		mat = glm::translate(mat, position);
		mat = glm::rotate(mat, rotation);
		mat = glm::scale(mat, scale);
		return mat;
	}
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
	v = glm::rotate(v, transform.rotation);
	v += transform.position;
	return v;
}

inline Vec2 operator*(Vec2 v, const Transform2D& transform)
{
	v += transform.position;
	v = glm::rotate(v, transform.rotation);
	v *= transform.scale;
	return v;
}

inline b2Transform toB2Transform(const Transform2D& t)
{
	return b2Transform(toB2Vec(t.position), b2Rot(t.rotation));
}

inline Transform2D toTransform(const b2Transform& transform)
{
	return Transform2D{
	    .position = toVec(transform.p),
	    .rotation = transform.q.GetAngle(),
	};
}

} // namespace Anker

REFL_TYPE(Anker::Transform2D)
REFL_FIELD(position)
REFL_FIELD(rotation, Anker::attr::Radians())
REFL_FIELD(scale)
REFL_END
