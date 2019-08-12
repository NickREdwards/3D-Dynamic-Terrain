#ifndef ENGINE_GEOMETRY_OBJECT3D_H
#define ENGINE_GEOMETRY_OBJECT3D_H

#include <DirectXMath.h>

#include "Mesh.h"
#include "..\Helpers\AABB.h"

using DirectX::XMFLOAT3;
using DirectX::XMVECTOR;
using DirectX::XMFLOAT4X4;
using Engine::Helpers::AABB;

namespace Engine 
{
namespace Geometry 
{

class _declspec(dllexport) IObject3D {
public:
	virtual ~IObject3D() {}

	virtual RVAL InitMesh(ID3D11Device* const) = 0;

	virtual void RebuildWorldMatrix() = 0;
	virtual void RebuildAABB() = 0;

	virtual void SetPosition(XMFLOAT3) = 0;
	virtual void SetWorldMatrix(XMFLOAT4X4) = 0;

	virtual XMFLOAT4X4& GetWorldMatrix() = 0;
	virtual XMFLOAT3& GetPosition() = 0;
	virtual AABB& GetAABB() = 0;
	virtual IMesh* GetMesh() = 0;
};

class _declspec(dllexport) Object3D: public IObject3D {
public:
	Object3D()
		:	_position(XMFLOAT3(0.0f, 0.0f, 0.0f)),
			_mesh(nullptr) {
		_mesh = new Mesh;
		RebuildAABB();
	}

	virtual ~Object3D() {
		delete _mesh;
	}

	virtual RVAL InitMesh(ID3D11Device* const d3dDevice) override;

	virtual void RebuildWorldMatrix() override;
	virtual void RebuildAABB() override;

	virtual void SetPosition(XMFLOAT3 position) override;
	virtual void SetWorldMatrix(XMFLOAT4X4 matrix) override;

	virtual XMFLOAT4X4& GetWorldMatrix() override;
	virtual XMFLOAT3& GetPosition() override;
	virtual AABB& GetAABB() override;
	virtual IMesh* GetMesh() override;
	
protected:
	AABB _BoundingBox;

private:
	XMFLOAT3 _position;
	XMFLOAT4X4 _worldMatrix;

	IMesh* _mesh;

	virtual void UpdatePosition();
};

inline RVAL Object3D::InitMesh(ID3D11Device* const) { return RVAL::R_FAILED; }

inline void Object3D::SetWorldMatrix(XMFLOAT4X4 matrix) { _worldMatrix = matrix; }

inline XMFLOAT4X4& Object3D::GetWorldMatrix() { return _worldMatrix; }
inline XMFLOAT3& Object3D::GetPosition() { return _position; }
inline AABB& Object3D::GetAABB() { return _BoundingBox; }
inline IMesh* Object3D::GetMesh() { return _mesh; }

}
}

#endif