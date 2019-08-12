#include <gtest\gtest.h>
#include <gmock\gmock.h>
#include <vector>

#include <Geometry\Mesh.h>
#include <Geometry\Vertex.h>

#include "..\Mocks\MockID3D11Device.h"

using namespace Engine::Geometry;
using testing::Return;
using testing::_;
using std::vector;
using Engine::Core::RVAL;

/*
	Vertices and Indices can be empty, but if 
*/
TEST(Mesh, RebuildBuffers) {
	MockID3D11Device mockD3DDevice;
	Mesh mesh;
	vector<Vertex> vertices;
	vector<UINT> indices;

	ON_CALL(mockD3DDevice, CreateBuffer(_, _, _))
		.WillByDefault(Return(E_FAIL));

	// Vertices and Indices are empty so CreateBuffer should not be called,
	// however the rebuild function still succeeds as the buffer can be empty
	EXPECT_CALL(mockD3DDevice, CreateBuffer(_, _, _))
		.Times(0);

	ASSERT_EQ(RVAL::R_OK, mesh.RebuildBuffers(&mockD3DDevice, vertices, indices));

	// Push a vertex onto the vector, this should still fail as indices == 0
	vertices.push_back(Vertex(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)));

	ASSERT_EQ(RVAL::R_FAILED, mesh.RebuildBuffers(&mockD3DDevice, vertices, indices));

	// We should fail if vertices == 0 but indices > 0. This involves 
	// defaulting CreateBuffer to S_OK
	vertices.clear();
	indices.push_back(0);

	ON_CALL(mockD3DDevice, CreateBuffer(_, _, _))
		.WillByDefault(Return(S_OK));

	ASSERT_EQ(RVAL::R_FAILED, mesh.RebuildBuffers(&mockD3DDevice, vertices, indices));

	// Final test should have vertices and indices populated, and 
	// return R_OK, with CreateBuffer being called twice
	vertices.push_back(Vertex(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)));

	EXPECT_CALL(mockD3DDevice, CreateBuffer(_, _, _))
		.Times(2);

	ASSERT_EQ(RVAL::R_OK, mesh.RebuildBuffers(&mockD3DDevice, vertices, indices));
}

TEST(Mesh, GetNumVertices) {
	MockID3D11Device mockD3DDevice;
	Mesh mesh;
	vector<Vertex> vertices;
	vector<UINT> indices;

	ON_CALL(mockD3DDevice, CreateBuffer(_, _, _))
		.WillByDefault(Return(S_OK));

	vertices.push_back(Vertex(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)));
	vertices.push_back(Vertex(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)));
	vertices.push_back(Vertex(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)));

	indices.push_back(0);

	if (mesh.RebuildBuffers(&mockD3DDevice, vertices, indices) == RVAL::R_OK) {
		ASSERT_EQ(3, mesh.GetNumVertices());
	}
}

TEST(Mesh, GetNumIndices) {
	MockID3D11Device mockD3DDevice;
	Mesh mesh;
	vector<Vertex> vertices;
	vector<UINT> indices;

	ON_CALL(mockD3DDevice, CreateBuffer(_, _, _))
		.WillByDefault(Return(S_OK));

	vertices.push_back(Vertex(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)));

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	if (mesh.RebuildBuffers(&mockD3DDevice, vertices, indices) == RVAL::R_OK) {
		ASSERT_EQ(3, mesh.GetNumIndices());
	}
}