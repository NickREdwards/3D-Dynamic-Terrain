#include "WaterPlane.h"

using namespace Engine::Geometry;

RVAL WaterPlane::InitMesh(ID3D11Device* const d3dDevice) {
	vector<Vertex> vertices;
	vector<UINT> indices;

	float s = 4000.0f * 0.5f;

	vertices.push_back(Vertex(XMFLOAT3(-s, 0.0f, -s), XMFLOAT3(0.0f, 1.0f, 0.0f)));
	vertices.push_back(Vertex(XMFLOAT3(-s, 0.0f, +s), XMFLOAT3(0.0f, 1.0f, 0.0f)));
	vertices.push_back(Vertex(XMFLOAT3(+s, 0.0f, +s), XMFLOAT3(0.0f, 1.0f, 0.0f)));
	vertices.push_back(Vertex(XMFLOAT3(+s, 0.0f, -s), XMFLOAT3(0.0f, 1.0f, 0.0f)));

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);

	return GetMesh()->RebuildBuffers(d3dDevice, vertices, indices);
}