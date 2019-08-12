#ifndef ENGINE_GEOMETRY_VERTEX_H
#define ENGINE_GEOMETRY_VERTEX_H

#include <DirectXMath.h>

using DirectX::XMFLOAT3;

struct Vertex {
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	Vertex(XMFLOAT3 position, XMFLOAT3 normal)
		:	Position(position),
			Normal(normal) {}
};

#endif