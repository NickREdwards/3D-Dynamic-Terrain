#include "Object3D.h"

using namespace Engine::Geometry;

void Object3D::RebuildWorldMatrix() {
	auto translation = DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z);
	XMStoreFloat4x4(&_worldMatrix, translation);
}

void Object3D::RebuildAABB() {
	_BoundingBox.Left	= _position.x;
	_BoundingBox.Right	= _position.x + 1.0f;
	_BoundingBox.Bottom	= _position.y;
	_BoundingBox.Top	= _position.y + 1.0f;
	_BoundingBox.Front	= _position.z;
	_BoundingBox.Back	= _position.z + 1.0f;
}

void Object3D::SetPosition(XMFLOAT3 position) {
	_position = position;
	UpdatePosition();
}

void Object3D::UpdatePosition() {
	RebuildWorldMatrix();
	RebuildAABB();
}