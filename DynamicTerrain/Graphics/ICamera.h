#ifndef ENGINE_GRAPHICS_ICAMERA_H
#define ENGINE_GRAPHICS_ICAMERA_H

#include <DirectXMath.h>

#include "..\Helpers\AABB.h"

using DirectX::XMFLOAT3;
using DirectX::XMVECTOR;
using DirectX::XMMATRIX;
using Engine::Helpers::AABB;

namespace Engine 
{
namespace Graphics 
{

class _declspec(dllexport) ICamera {
public:
	virtual ~ICamera() {}

	virtual void Update() = 0;
	virtual void Reset(XMFLOAT3 position = XMFLOAT3(0.0f, 1.0f, 0.0f)) = 0;

	virtual bool IsSphereInViewFrustum(const XMFLOAT4&) = 0;
	virtual bool IsAABBInViewFrustum(const AABB&) = 0;

	virtual void MoveLeft(float) = 0;
	virtual void MoveRight(float) = 0;
	virtual void MoveForward(float) = 0;
	virtual void MoveBackward(float) = 0;

	virtual void Move(const XMVECTOR&) = 0;

	virtual void Yaw(float) = 0;
	virtual void Pitch(float) = 0;
	virtual void Zoom(float) = 0;

	virtual void SetPosition(XMFLOAT3 position) = 0;
	virtual void SetTarget(XMFLOAT3 target) = 0;

	virtual const XMFLOAT3& GetPosition() = 0;
	virtual const XMFLOAT3& GetTarget() = 0;
	virtual const XMFLOAT3& GetForward() = 0;
	virtual const XMFLOAT3& GetRight() = 0;
	virtual const XMFLOAT3& GetUp() = 0;

	virtual const XMMATRIX& GetViewMatrix() = 0;
	virtual const XMMATRIX& GetProjMatrix() = 0;

private:
	virtual void BuildViewMatrix() = 0;
	virtual void BuildProjMatrix() = 0;
	virtual void BuildViewFrustum() = 0;
};

}
}

#endif