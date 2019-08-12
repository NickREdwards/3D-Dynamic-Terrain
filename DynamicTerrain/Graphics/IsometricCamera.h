#ifndef ENGINE_GRAPHICS_ISOMETRICCAMERA_H
#define ENGINE_GRAPHICS_ISOMETRICCAMERA_H

#include <DirectXMath.h>

#include "..\Helpers\Math.h"
#include "..\Helpers\AABB.h"
#include "ICamera.h"

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4X4;
using DirectX::XMVECTOR;
using DirectX::XMMATRIX;
using DirectX::operator-;
using DirectX::operator*;
using DirectX::operator+=;
using DirectX::operator-=;
using namespace Engine::Helpers;

namespace Engine 
{
namespace Graphics 
{

const float CAMERA_MIN_ZOOM		= 500.0f;
const float CAMERA_MAX_ZOOM		= 1000.0f;
const float CAMERA_FOV			= DirectX::XM_PI / 3;
const float CAMERA_DRAW_DIST	= 10000.0f;

class _declspec(dllexport) IsometricCamera: public ICamera {
public:
	IsometricCamera(float viewportWidth, float viewportHeight)
		:	_viewportWidth(viewportWidth),
			_viewportHeight(viewportHeight),
			_viewportAspectRatio(viewportWidth / viewportHeight) {
		Reset();
	}

	~IsometricCamera() {}

	void Update() override {
		BuildViewMatrix();
		BuildViewFrustum();
	}

	void Reset(XMFLOAT3 position = XMFLOAT3(-1000.0f, 1000.0f, 1000.0f)) override {
		_position	= position;
		_target		= XMFLOAT3(0.0f, 0.0f, 0.0f);
		_forward	= XMFLOAT3(0.0f, 0.0f, 1.0f);
		_right		= XMFLOAT3(1.0f, 0.0f, 0.0f);

		_yaw	= 0.0f;
		_zoom	= 600.0f;

		assert(_viewportWidth > 0 && _viewportHeight > 0);
		_viewportAspectRatio = _viewportWidth / _viewportHeight;

		auto identityMatrix = DirectX::XMMatrixIdentity();
		XMStoreFloat4x4(&_viewMatrix, identityMatrix);
		XMStoreFloat4x4(&_projMatrix, identityMatrix);

		BuildViewMatrix();
		BuildProjMatrix();
	}

	inline bool IsSphereInViewFrustum(const XMFLOAT4& sphere) override {
		auto spherePos = XMLoadFloat3(&XMFLOAT3(sphere.x, sphere.y, sphere.z));
		auto sphereRadius = sphere.w;

		for (auto i = 0; i < 6; i++) {
			auto viewFrustum = XMLoadFloat4(&_viewFrustum[i]);

			XMFLOAT3 vfDotSP;
			XMStoreFloat3(&vfDotSP, DirectX::XMPlaneDotCoord(viewFrustum, spherePos));

			if (vfDotSP.x + sphereRadius < 0)
				return false;
		}
		return true;
	}

	inline bool IsAABBInViewFrustum(const AABB& aabb) override {
		auto radius = 200.0f;
		XMFLOAT4 boundingSphere = XMFLOAT4((aabb.Left + aabb.Right) / 2, 
										   (aabb.Bottom + aabb.Top) / 2,
										   (aabb.Front + aabb.Back) / 2, radius);

		// Try an early out
		if (IsSphereInViewFrustum(boundingSphere)) {
			return true;
		}

		return false;
	}

	inline void MoveLeft(float amount) override {
		auto deltaPos = MultXMFloat3ByScalar(_right, amount);
		_target = AddXMFloat3ToXMFloat3Neg(_target, deltaPos);
	}

	inline void MoveRight(float amount) override {
		auto deltaPos = MultXMFloat3ByScalar(_right, amount);
		_target = AddXMFloat3ToXMFloat3(_target, deltaPos);
	}

	inline void MoveForward(float amount) override {
		auto deltaPos = MultXMFloat3ByScalar(_forward, amount);
		_target = AddXMFloat3ToXMFloat3(_target, deltaPos);
	}

	inline void MoveBackward(float amount) override {
		auto deltaPos = MultXMFloat3ByScalar(_forward, amount);
		_target = AddXMFloat3ToXMFloat3Neg(_target, deltaPos);
	}
	
	inline void Move(const XMVECTOR& vec) override {
		auto posV = XMLoadFloat3(&_position);
		XMStoreFloat3(&_position, DirectX::XMVectorAdd(posV, vec));
	}

	inline void Yaw(float amount) override {
		if (_yaw >= DirectX::XM_2PI)
			_yaw = 0.0f;
		_yaw += amount;
	}

	inline void Pitch(float) override {}

	inline void Zoom(float amount) override {
		_zoom = Clamp(_zoom - amount, CAMERA_MIN_ZOOM, CAMERA_MAX_ZOOM);
	}

	inline void SetPosition(XMFLOAT3 position) override {
		_position = position;
	}

	inline void SetTarget(XMFLOAT3 target) override {
		_target = target;
	}

	inline const XMFLOAT3& GetPosition() override	{ return _position; }
	inline const XMFLOAT3& GetTarget() override		{ return _target; }
	inline const XMFLOAT3& GetForward() override	{ return _forward; }
	inline const XMFLOAT3& GetRight() override		{ return _right; }
	inline const XMFLOAT3& GetUp() override			{ return XMFLOAT3(0.0f, 1.0f, 0.0f); }

	inline const XMMATRIX& GetViewMatrix() override { return XMLoadFloat4x4(&_viewMatrix); };
	inline const XMMATRIX& GetProjMatrix() override { return XMLoadFloat4x4(&_projMatrix); };

private:
	XMFLOAT3 _position;
	XMFLOAT3 _target;
	XMFLOAT3 _forward;
	XMFLOAT3 _right;

	float _viewportWidth;
	float _viewportHeight;
	float _viewportAspectRatio;

	float _yaw;
	float _zoom;
	
	XMFLOAT4X4 _viewMatrix;
	XMFLOAT4X4 _projMatrix;

	XMFLOAT4 _viewFrustum[6];

	inline void BuildViewMatrix() override {
		auto targ = DirectX::XMLoadFloat3(&_target);
		auto up = DirectX::XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f));
		auto fwd = DirectX::XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 1.0f));
		auto rht = DirectX::XMLoadFloat3(&XMFLOAT3(1.0f, 0.0f, 0.0f));

		XMFLOAT3 pos(_zoom * cos(_yaw) + _target.x, 
					 _zoom, 
					 _zoom * sin(_yaw) + _target.z);
		_position = pos;
		auto position = XMLoadFloat3(&pos);

		XMMATRIX matView = DirectX::XMMatrixLookAtLH(position, targ, up);
		XMStoreFloat4x4(&_viewMatrix, matView);

		auto right = DirectX::XMVector3Cross(up, DirectX::XMVector3Normalize(targ - position));
		auto forward = DirectX::XMVector3Cross(right, up);

		auto newForward = DirectX::XMVector3Normalize(forward);
		auto newRight = DirectX::XMVector3Normalize(right);

		XMStoreFloat3(&_forward, newForward);
		XMStoreFloat3(&_right, newRight);
	}

	inline void BuildProjMatrix() override {
		XMStoreFloat4x4(&_projMatrix, DirectX::XMMatrixPerspectiveFovLH(CAMERA_FOV, _viewportAspectRatio, 1.0f, CAMERA_DRAW_DIST));
	}

	inline void BuildViewFrustum() override {
		auto matView = XMLoadFloat4x4(&_viewMatrix);
		auto matProj = XMLoadFloat4x4(&_projMatrix);
		auto matViewProj = XMMatrixMultiply(matView, matProj);

		XMFLOAT4X4 viewProj;
		XMStoreFloat4x4(&viewProj, matViewProj);

		// Left plane
		_viewFrustum[0].x = viewProj._14 + viewProj._11;
		_viewFrustum[0].y = viewProj._24 + viewProj._21;
		_viewFrustum[0].z = viewProj._34 + viewProj._31;
		_viewFrustum[0].w = viewProj._44 + viewProj._41;
 
		// Right plane
		_viewFrustum[1].x = viewProj._14 - viewProj._11;
		_viewFrustum[1].y = viewProj._24 - viewProj._21;
		_viewFrustum[1].z = viewProj._34 - viewProj._31;
		_viewFrustum[1].w = viewProj._44 - viewProj._41;
 
		// Top plane
		_viewFrustum[2].x = viewProj._14 - viewProj._12;
		_viewFrustum[2].y = viewProj._24 - viewProj._22;
		_viewFrustum[2].z = viewProj._34 - viewProj._32;
		_viewFrustum[2].w = viewProj._44 - viewProj._42;
 
		// Bottom plane
		_viewFrustum[3].x = viewProj._14 + viewProj._12;
		_viewFrustum[3].y = viewProj._24 + viewProj._22;
		_viewFrustum[3].z = viewProj._34 + viewProj._32;
		_viewFrustum[3].w = viewProj._44 + viewProj._42;
 
		// Near plane
		_viewFrustum[4].x = viewProj._13;
		_viewFrustum[4].y = viewProj._23;
		_viewFrustum[4].z = viewProj._33;
		_viewFrustum[4].w = viewProj._43;
 
		// Far plane
		_viewFrustum[5].x = viewProj._14 - viewProj._13;
		_viewFrustum[5].y = viewProj._24 - viewProj._23;
		_viewFrustum[5].z = viewProj._34 - viewProj._33;
		_viewFrustum[5].w = viewProj._44 - viewProj._43;
 
		// Normalize planes
		for (int i = 0; i < 6; i++) {
			auto vf = DirectX::XMPlaneNormalize(XMLoadFloat4(&_viewFrustum[i]));
			XMStoreFloat4(&_viewFrustum[i], vf);
		}
	}
};

}
}

#endif