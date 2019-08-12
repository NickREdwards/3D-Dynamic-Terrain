#include <gtest\gtest.h>
#include <gmock\gmock.h>

#include "..\Mocks\MockIDynamicTerrainSystem.h"

#include <Geometry\CubicSurfaceExtractor.h>
#include <Geometry\DynamicTerrainChunk.h>
#include <Geometry\DynamicTerrainSettings.h>

using namespace Engine::Geometry;
using testing::_;
using testing::Return;
using testing::Test;

class TestCubicSurfaceExtractor: public Test {
protected:
	virtual void SetUp() {
		_MockTerrainSystem = new MockIDynamicTerrainSystem;
		_Chunk = new DynamicTerrainChunk(_MockTerrainSystem);

		float** heightField = new float*[TS.ChunkSizeX+1];
		for (auto i = 0; i < TS.ChunkSizeX+1; i++) {
			heightField[i] = new float[TS.ChunkSizeZ+1];
			for (auto j = 0; j < TS.ChunkSizeZ+1; j++) {
				heightField[i][j] = 1.0f;
			}
		}

		ON_CALL(*_MockTerrainSystem, GetHeightFieldForChunk(_, _))
			.WillByDefault(Return(heightField));

		_Chunk->Init();
	}

	virtual void TearDown() {
		delete _MockTerrainSystem;
		delete _Chunk;
	}

	MockIDynamicTerrainSystem* _MockTerrainSystem;
	DynamicTerrainChunk* _Chunk;
	CubicSurfaceExtractor _Extractor;
};

TEST_F(TestCubicSurfaceExtractor, Extract) {
	vector<Vertex> vertices;
	vector<UINT> indices;

	_Extractor.Extract(nullptr, _Chunk, vertices, indices);

	ASSERT_GT(vertices.size(), 0);
	ASSERT_GT(indices.size(), 0);
}