#include "Mesh.h"

using namespace Engine::Geometry;

RVAL Mesh::RebuildBuffers(ID3D11Device* const d3dDevice, vector<Vertex>& vertices, vector<UINT>& indices) {
	_numVertices = 0;
	_numIndices = indices.size();

	if (RebuildVertexBuffer(d3dDevice, vertices) != RVAL::R_OK)
		return RVAL::R_FAILED;

	_numVertices = vertices.size();
	_numIndices = 0;

	if (RebuildIndexBuffer(d3dDevice, indices) != RVAL::R_OK)
		return RVAL::R_FAILED;

	return RVAL::R_OK;
}

RVAL Mesh::RebuildVertexBuffer(ID3D11Device* const d3dDevice, vector<Vertex>& vertices) {
	_numVertices = vertices.size();

	auto bufferDesc = GetVertexBufferDesc();

	if (_numVertices < 1) return RVAL::R_OK;
	if (_numIndices < 1) return RVAL::R_FAILED;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &vertices[0];

	if (_vertexBuffer) _vertexBuffer->Release();
	_vertexBuffer = nullptr;

	if (FAILED(d3dDevice->CreateBuffer(&bufferDesc, &initData, &_vertexBuffer)))
		return RVAL::R_FAILED;

	return RVAL::R_OK;
}

RVAL Mesh::RebuildIndexBuffer(ID3D11Device* const d3dDevice, vector<UINT>& indices) {
	_numIndices = indices.size();

	auto bufferDesc = GetIndexBufferDesc();

	if (_numIndices < 1) return RVAL::R_OK;
	if (_numVertices < 1) return RVAL::R_FAILED;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &indices[0];

	if (_indexBuffer) _indexBuffer->Release();
	_indexBuffer = nullptr;

	if (FAILED(d3dDevice->CreateBuffer(&bufferDesc, &initData, &_indexBuffer)))
		return RVAL::R_FAILED;

	return RVAL::R_OK;
}

D3D11_BUFFER_DESC Mesh::GetVertexBufferDesc() {
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage				= D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth			= sizeof(Vertex) * _numVertices;
	bufferDesc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags		= 0;
	bufferDesc.MiscFlags			= 0;
	bufferDesc.StructureByteStride	= 0;

	return bufferDesc;
}

D3D11_BUFFER_DESC Mesh::GetIndexBufferDesc() {
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage				= D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth			= sizeof(UINT) * _numIndices;
	bufferDesc.BindFlags			= D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags		= 0;
	bufferDesc.MiscFlags			= 0;
	bufferDesc.StructureByteStride	= 0;

	return bufferDesc;
}