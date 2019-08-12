#ifndef MOCKMESH_H
#define MOCKMESH_H

#include <gmock\gmock.h>

#include <Geometry\Mesh.h>

using namespace Engine::Geometry;

class MockIMesh: public IMesh {
public:
	MOCK_METHOD3(RebuildBuffers, RVAL(ID3D11Device* const, vector<Vertex>&, vector<UINT>&));
	MOCK_METHOD0(GetVertexBuffer, ID3D11Buffer* const());
	MOCK_METHOD0(GetIndexBuffer, ID3D11Buffer* const());
	MOCK_METHOD0(GetNumVertices, UINT());
	MOCK_METHOD0(GetNumIndices, UINT());
	MOCK_METHOD0(GetVertexStride, UINT());
	MOCK_METHOD0(BeginDraw, void());
	MOCK_METHOD0(EndDraw, void());
	MOCK_METHOD0(IsBeingDrawn, bool());
};

#endif