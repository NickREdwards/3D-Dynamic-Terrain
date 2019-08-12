#ifndef ENGINE_HELPERS_MATH_H
#define ENGINE_HELPERS_MATH_H

#include <DirectXMath.h>

using DirectX::XMVECTOR;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;
using DirectX::XMFLOAT4X4;
using DirectX::XMMATRIX;

namespace Engine 
{
namespace Helpers
{

static XMFLOAT3 XMVectorToXMFloat3(XMVECTOR& vec) {
	XMFLOAT3 returnVal;
	XMStoreFloat3(&returnVal, vec);
	return returnVal;
}

static XMFLOAT4X4 XMMatrixToXMFloat4x4(XMMATRIX& mat) {
	XMFLOAT4X4 returnVal;
	XMStoreFloat4x4(&returnVal, mat);
	return returnVal;
}

static XMFLOAT3 MultXMFloat3ByScalar(XMFLOAT3& val, float scalar) {
	auto vVal = XMLoadFloat3(&val);
	return XMVectorToXMFloat3(DirectX::XMVectorScale(vVal, scalar));
}

static XMFLOAT3 AddXMFloat3ToXMFloat3(XMFLOAT3& a, XMFLOAT3& b) {
	return XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
}

static XMFLOAT3 AddXMFloat3ToXMFloat3Neg(XMFLOAT3& a, XMFLOAT3& b) {
	return XMFLOAT3(a.x + -b.x, a.y + -b.y, a.z + -b.z);
}

static float DotXMFloat4(const XMFLOAT4 &a, const XMFLOAT4 &b) {
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

static float ConvertNoiseToHeight(float val) { 
	return (val * 0.5f) + 0.5f; 
}

static float Clamp(float val, float minimum, float maximum) {
	if (val < minimum) return minimum;
	if (val > maximum) return maximum;
	return val;
}

}
}

#endif