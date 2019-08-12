#ifndef ENGINE_GEOMETRY_MESH_H
#define ENGINE_GEOMETRY_MESH_H

#include <D3D11.h>
#include <vector>

#include "Vertex.h"
#include "..\Core\MethodHelpers.h"

using std::vector;
using Engine::Core::RVAL;

namespace Engine 
{
namespace Geometry 
{

class MeshDrawLock;

class _declspec(dllexport) IMesh {
public:
	virtual ~IMesh() {};

	virtual RVAL RebuildBuffers(ID3D11Device* const d3dDevice, 
								vector<Vertex>& vertices, 
								vector<UINT>& indices) = 0;

	virtual ID3D11Buffer* const GetVertexBuffer() = 0;
	virtual ID3D11Buffer* const GetIndexBuffer() = 0;

	virtual UINT GetNumVertices() = 0;
	virtual UINT GetNumIndices() = 0;
	virtual UINT GetVertexStride() = 0;

	virtual void BeginDraw() = 0;
	virtual void EndDraw() = 0;
	virtual bool IsBeingDrawn() = 0;
};

class _declspec(dllexport) Mesh: public IMesh {
public:
	Mesh()
		:	_vertexBuffer(nullptr),
			_indexBuffer(nullptr),
			_numVertices(0),
			_numIndices(0),
			_beingDrawn(false) {}

	~Mesh() {
		if (_vertexBuffer) _vertexBuffer->Release();
		if (_indexBuffer) _indexBuffer->Release();
	}

	RVAL RebuildBuffers(ID3D11Device* const d3dDevice, 
						vector<Vertex>& vertices, 
						vector<UINT>& indices) override;

	inline ID3D11Buffer* const GetVertexBuffer() override { return _vertexBuffer; }
	inline ID3D11Buffer* const GetIndexBuffer() override  { return _indexBuffer;  }

	inline UINT GetNumVertices() override  { return _numVertices;   }
	inline UINT GetNumIndices() override   { return _numIndices;    }
	inline UINT GetVertexStride() override { return sizeof(Vertex); }

	inline void BeginDraw() override { _beingDrawn = true; }
	inline void EndDraw() override { _beingDrawn = false; }
	inline bool IsBeingDrawn() override { return _beingDrawn; }

private:
	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer* _indexBuffer;

	UINT _numVertices;
	UINT _numIndices;

	bool _beingDrawn;

	RVAL RebuildVertexBuffer(ID3D11Device* const d3dDevice, 
							 vector<Vertex>& vertices);

	RVAL RebuildIndexBuffer(ID3D11Device* const d3dDevice, 
							vector<UINT>& indices);

	D3D11_BUFFER_DESC GetVertexBufferDesc();
	D3D11_BUFFER_DESC GetIndexBufferDesc();
};

}
}

#endif