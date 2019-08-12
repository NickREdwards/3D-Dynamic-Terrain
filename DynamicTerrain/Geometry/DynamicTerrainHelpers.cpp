#include "DynamicTerrainHelpers.h"
#include "DynamicTerrainChunk.h"

using namespace Engine::Geometry;

int DynamicTerrainHelpers::GetHeighestVoxelIndexAtXZIndex(DynamicTerrainChunk* const chunk, int x, int z) {
	std::lock_guard<mutex> lock(chunk->_chunkUpdater.GetAccessLock());
	
	int index = 0;

	for (auto y = 0; y < TS.ChunkSizeY; y++) {
		if (chunk->_voxels[x][y][z].Active && y > index)
			index = y;
	}

	return index;
}