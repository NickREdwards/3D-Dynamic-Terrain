#include <gtest\gtest.h>
#include <gmock\gmock.h>

#include "..\Mocks\MockIDynamicTerrainSystem.h"

#include <Geometry\DynamicTerrainChunk.h>
#include <Geometry\DynamicTerrainChunkUpdater.h>

using namespace Engine::Geometry;
using testing::Test;
using testing::_;
using testing::Return;

class TestDynamicTerrainChunkUpdater: public Test {
protected:
	virtual void SetUp() {
		_MockTerrainSystem = new MockIDynamicTerrainSystem;
		_Chunk = new DynamicTerrainChunk(_MockTerrainSystem);
		_Updater = new DynamicTerrainChunkUpdater(_Chunk);

		float** heightField = new float*[TS.ChunkSizeX+1];
		for (auto i = 0; i < TS.ChunkSizeX+1; i++) {
			heightField[i] = new float[TS.ChunkSizeZ+1];
			for (auto j = 0; j < TS.ChunkSizeZ+1; j++) {
				heightField[i][j] = 0.95f;
			}
		}

		ON_CALL(*_MockTerrainSystem, GetHeightFieldForChunk(_, _))
			.WillByDefault(Return(heightField));

		// Initialisation will free up the heightField memory
		_Chunk->Init();
	}

	virtual void TearDown() {
		delete _Updater;
		delete _Chunk;
	}

	MockIDynamicTerrainSystem* _MockTerrainSystem;
	DynamicTerrainChunk* _Chunk;
	DynamicTerrainChunkUpdater* _Updater;
};

TEST_F(TestDynamicTerrainChunkUpdater, TestIsVoxelActiveAt) {
	ASSERT_FALSE(_Updater->IsVoxelActiveAt(_Chunk, XMINT3(0, TS.ChunkSizeY - 1, 0)));
	ASSERT_TRUE(_Updater->IsVoxelActiveAt(_Chunk, XMINT3(0, TS.ChunkSizeY - 2, 0)));
}

TEST_F(TestDynamicTerrainChunkUpdater, TestIsVoxelOutOfBounds) {
	ASSERT_TRUE(_Updater->IsVoxelOutOfBounds(_Chunk, XMINT3(TS.ChunkSizeX, 1, 0)));
	ASSERT_TRUE(_Updater->IsVoxelOutOfBounds(_Chunk, XMINT3(-1, 1, 0)));
	ASSERT_TRUE(_Updater->IsVoxelOutOfBounds(_Chunk, XMINT3(0, TS.ChunkSizeY, 0)));
	ASSERT_TRUE(_Updater->IsVoxelOutOfBounds(_Chunk, XMINT3(0, 0, 0))); // Y < 1 is out of bounds
	ASSERT_TRUE(_Updater->IsVoxelOutOfBounds(_Chunk, XMINT3(0, 1, TS.ChunkSizeZ)));
	ASSERT_TRUE(_Updater->IsVoxelOutOfBounds(_Chunk, XMINT3(0, 1, -1)));

	ASSERT_FALSE(_Updater->IsVoxelOutOfBounds(_Chunk, XMINT3(1, 1, 0)));
	ASSERT_FALSE(_Updater->IsVoxelOutOfBounds(_Chunk, XMINT3(0, 2, 0)));
	ASSERT_FALSE(_Updater->IsVoxelOutOfBounds(_Chunk, XMINT3(0, 1, 1)));
}

TEST_F(TestDynamicTerrainChunkUpdater, TestIsVoxelAtEdge) {
	ASSERT_TRUE(_Updater->IsVoxelAtEdge(_Chunk, XMINT3(0, 0, 0)));
	ASSERT_TRUE(_Updater->IsVoxelAtEdge(_Chunk, XMINT3(0, 1, 1)));
	ASSERT_TRUE(_Updater->IsVoxelAtEdge(_Chunk, XMINT3(1, 0, 1)));
	ASSERT_TRUE(_Updater->IsVoxelAtEdge(_Chunk, XMINT3(1, 1, 0)));
	ASSERT_FALSE(_Updater->IsVoxelAtEdge(_Chunk, XMINT3(1, 1, 1)));

	ASSERT_TRUE(_Updater->IsVoxelAtEdge(_Chunk, XMINT3(TS.ChunkSizeX-1, TS.ChunkSizeY-1, TS.ChunkSizeZ-1)));
	ASSERT_TRUE(_Updater->IsVoxelAtEdge(_Chunk, XMINT3(TS.ChunkSizeX-1, 1, 1)));
	ASSERT_TRUE(_Updater->IsVoxelAtEdge(_Chunk, XMINT3(1, TS.ChunkSizeY-1, 1)));
	ASSERT_TRUE(_Updater->IsVoxelAtEdge(_Chunk, XMINT3(1, 1, TS.ChunkSizeZ-1)));
}

TEST_F(TestDynamicTerrainChunkUpdater, TestActivateDeactivate) {
	ASSERT_FALSE(_Updater->IsVoxelActiveAt(_Chunk, XMINT3(0, TS.ChunkSizeY - 1, 0)));
	_Updater->ActivateVoxel(_Chunk, XMINT3(0, TS.ChunkSizeY - 1, 0));
	ASSERT_TRUE(_Updater->IsVoxelActiveAt(_Chunk, XMINT3(0, TS.ChunkSizeY - 1, 0)));
	_Updater->DeactivateVoxel(_Chunk, XMINT3(0, TS.ChunkSizeY - 1, 0));
	ASSERT_FALSE(_Updater->IsVoxelActiveAt(_Chunk, XMINT3(0, TS.ChunkSizeY - 1, 0)));
}

TEST_F(TestDynamicTerrainChunkUpdater, TestDropVoxel) {
	ASSERT_TRUE(_Updater->IsVoxelActiveAt(_Chunk, XMINT3(0, TS.ChunkSizeY - 2, 0)));

	_Updater->DeactivateVoxel(_Chunk, XMINT3(0, TS.ChunkSizeY - 3, 0));
	ASSERT_FALSE(_Updater->IsVoxelActiveAt(_Chunk, XMINT3(0, TS.ChunkSizeY - 3, 0)));

	_Updater->DropVoxel(XMINT3(0, TS.ChunkSizeY - 2, 0));

	ASSERT_FALSE(_Updater->IsVoxelActiveAt(_Chunk, XMINT3(0, TS.ChunkSizeY - 2, 0)));
	ASSERT_TRUE(_Updater->IsVoxelActiveAt(_Chunk, XMINT3(0, TS.ChunkSizeY - 3, 0)));
}

TEST_F(TestDynamicTerrainChunkUpdater, TestSlideVoxel) {
	ASSERT_TRUE(_Updater->IsVoxelActiveAt(_Chunk, XMINT3(0, TS.ChunkSizeY - 2, 0)));

	_Updater->DeactivateVoxel(_Chunk, XMINT3(0, TS.ChunkSizeY - 2, 1));
	_Updater->DeactivateVoxel(_Chunk, XMINT3(0, TS.ChunkSizeY - 3, 1));
	ASSERT_FALSE(_Updater->IsVoxelActiveAt(_Chunk, XMINT3(0, TS.ChunkSizeY - 2, 1)));
	ASSERT_FALSE(_Updater->IsVoxelActiveAt(_Chunk, XMINT3(0, TS.ChunkSizeY - 3, 1)));

	_Updater->SlideVoxel(XMINT3(0, TS.ChunkSizeY - 2, 0));

	ASSERT_FALSE(_Updater->IsVoxelActiveAt(_Chunk, XMINT3(0, TS.ChunkSizeY - 2, 0)));
	ASSERT_TRUE(_Updater->IsVoxelActiveAt(_Chunk, XMINT3(0, TS.ChunkSizeY - 2, 1)));
}