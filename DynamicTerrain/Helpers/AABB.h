#ifndef ENGINE_HELPERS_AABB_H
#define ENGINE_HELPERS_AABB_H

#include <DirectXMath.h>

using DirectX::XMFLOAT3;

namespace Engine
{
namespace Helpers
{

struct AABB {
	float Left, Right;
	float Top, Bottom;
	float Front, Back;

	AABB()	:	Left(0.0f), Right(0.0f),
				Top(0.0f), Bottom(0.0f),
				Front(0.0f), Back(0.0f) {}

	AABB(float _left, float _right,
		 float _top, float _bottom,
		 float _front, float _back)	
		:	Left(_left), Right(_right),
			Top(_top), Bottom(_bottom),
			Front(_front), Back(_back) {}

	bool IntersectedBy(const XMFLOAT3& point, float radius) {
		float rSq = radius * radius;

		if (point.x < Left) rSq -= (point.x - Left) * (point.x - Left);
		else if (point.x > Right) rSq -= (point.x - Right) * (point.x - Right);

		if (point.y < Bottom) rSq -= (point.y - Bottom) * (point.y - Bottom);
		else if (point.y > Top) rSq -= (point.y - Top) * (point.y - Top);

		if (point.z < Front) rSq -= (point.z - Front) * (point.z - Front);
		else if (point.z > Back) rSq -= (point.z - Back) * (point.z - Back);

		return rSq > 0;
	}
};

}
}

#endif