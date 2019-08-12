#include "DynamicTerrainChunkUpdater.h"
#include "DynamicTerrainChunk.h"

using namespace Engine::Geometry;

void DynamicTerrainChunkUpdater::Update() {
	if (_chunk->_bufferIsDirty || _chunk->_smoothTerrain) return;

	vector<VoxelRef> updatesThisFrame;

	{
		std::lock_guard<mutex> lock(_accessLock);
		updatesThisFrame = _chunk->_voxelsToUpdate;
		_chunk->_voxelsToUpdate.clear();
	}

	for (auto& v : updatesThisFrame) {
		auto voxelIndex = XMINT3(v.X, v.Y, v.Z);

		if (IsVoxelOutOfBounds(_chunk, voxelIndex) ||
			!IsVoxelActiveAt(_chunk, voxelIndex)) continue;

		if (DropVoxel(voxelIndex)) continue;
		if (SlideVoxel(voxelIndex)) continue;
	}
}

void DynamicTerrainChunkUpdater::UpdateNeighbours(XMINT3& index) {
	if (index.x == 0 && _westActive) {
		UpdateVoxelsInRingAroundIndex(_west, XMINT3(TS.ChunkSizeX-1, index.y, index.z));
		UpdateVoxelsInRingAroundIndex(_west, XMINT3(TS.ChunkSizeX-1, index.y+1, index.z));
		_west->ForceActiveExtractor();
	} else if (index.z == 0 && _southActive) {
		UpdateVoxelsInRingAroundIndex(_south, XMINT3(index.x, index.y, TS.ChunkSizeZ-1));
		UpdateVoxelsInRingAroundIndex(_south, XMINT3(index.x, index.y+1, TS.ChunkSizeZ-1));
		_south->ForceActiveExtractor();
	} else if (index.x == TS.ChunkSizeX-1 && _eastActive) {
		UpdateVoxelsInRingAroundIndex(_east, XMINT3(0, index.y, index.z));
		UpdateVoxelsInRingAroundIndex(_east, XMINT3(0, index.y+1, index.z));
		_east->ForceActiveExtractor();
	} else if (index.z == TS.ChunkSizeZ-1 && _northActive) {
		UpdateVoxelsInRingAroundIndex(_north, XMINT3(index.x, index.y, 0));
		UpdateVoxelsInRingAroundIndex(_north, XMINT3(index.x, index.y+1, 0));
		_north->ForceActiveExtractor();
	}
}

bool DynamicTerrainChunkUpdater::DropVoxel(XMINT3& index) {
	if (!IsVoxelActiveAt(_chunk, XMINT3(index.x, index.y-1, index.z))) {
		ActivateVoxel(_chunk, XMINT3(index.x, index.y-1, index.z));
		DeactivateVoxel(_chunk, XMINT3(index.x, index.y, index.z));

		UpdateVoxel(_chunk, XMINT3(index.x, index.y+1, index.z));
		UpdateVoxel(_chunk, XMINT3(index.x, index.y, index.z));
		UpdateVoxel(_chunk, XMINT3(index.x, index.y-1, index.z));

		_chunk->DirtyBuffer();

		return true;
	}

	return false;
}

bool DynamicTerrainChunkUpdater::SlideVoxel(XMINT3& index) {
	vector<VoxelRef> slides;

	TrySlideVoxelNorth(index, slides);
	TrySlideVoxelEast (index, slides);
	TrySlideVoxelSouth(index, slides);
	TrySlideVoxelWest (index, slides);

	if (!IsVoxelAtEdge(_chunk, index)) {
		TrySlideVoxelNorthEast(index, slides);
		TrySlideVoxelSouthEast(index, slides);
		TrySlideVoxelSouthWest(index, slides);
		TrySlideVoxelNorthWest(index, slides);
	}

	if (slides.size() == 0) return false;

	auto s = ChooseRandomSlide(slides);

	ActivateVoxel(s.ChunkRef, XMINT3(s.X, s.Y, s.Z));
	DeactivateVoxel(_chunk, index);

	UpdateNeighbours(index);
	UpdateVoxelsInRingAroundIndex(_chunk, index);
	UpdateVoxelsInRingAroundIndex(_chunk, XMINT3(index.x, index.y+1, index.z));
	UpdateVoxelsInRingAroundIndex(s.ChunkRef, XMINT3(s.X, s.Y, s.Z));
	UpdateVoxelsInRingAroundIndex(s.ChunkRef, XMINT3(s.X, s.Y+1, s.Z));

	s.ChunkRef->ForceActiveExtractor();

	_chunk->DirtyBuffer();

	return true;
}

void DynamicTerrainChunkUpdater::TrySlideVoxelNorth(XMINT3& index, vector<VoxelRef>& slides) {
	if (index.z != TS.ChunkSizeZ - 1) {
		if (!IsVoxelActiveAt(_chunk, XMINT3(index.x, index.y, index.z+1)) &&
			!IsVoxelActiveAt(_chunk, XMINT3(index.x, index.y-1, index.z+1)))
			slides.push_back(VoxelRef(_chunk, index.x, index.y, index.z+1));
	} else {
		if (_northActive && _north->IsLoaded()) {
			if (!IsVoxelActiveAt(_north, XMINT3(index.x, index.y, 0)) &&
				!IsVoxelActiveAt(_north, XMINT3(index.x, index.y-1, 0)))
				slides.push_back(VoxelRef(_north, index.x, index.y, 0));
		}
	}
}

void DynamicTerrainChunkUpdater::TrySlideVoxelEast(XMINT3& index, vector<VoxelRef>& slides) {
	if (index.x != TS.ChunkSizeX - 1) {
		if (!IsVoxelActiveAt(_chunk, XMINT3(index.x+1, index.y, index.z)) &&
			!IsVoxelActiveAt(_chunk, XMINT3(index.x+1, index.y-1, index.z)))
			slides.push_back(VoxelRef(_chunk, index.x+1, index.y, index.z));
	} else {
		if (_eastActive && _east->IsLoaded()) {
			if (!IsVoxelActiveAt(_east, XMINT3(0, index.y, index.z)) &&
				!IsVoxelActiveAt(_east, XMINT3(0, index.y-1, index.z)))
				slides.push_back(VoxelRef(_east, 0, index.y, index.z));
		}
	}
}

void DynamicTerrainChunkUpdater::TrySlideVoxelSouth(XMINT3& index, vector<VoxelRef>& slides) {
	if (index.z != 0) {
		if (!IsVoxelActiveAt(_chunk, XMINT3(index.x, index.y, index.z-1)) &&
			!IsVoxelActiveAt(_chunk, XMINT3(index.x, index.y-1, index.z-1)))
			slides.push_back(VoxelRef(_chunk, index.x, index.y, index.z-1));
	} else {
		if (_southActive && _south->IsLoaded()) {
			if (!IsVoxelActiveAt(_south, XMINT3(index.x, index.y, TS.ChunkSizeZ-1)) &&
				!IsVoxelActiveAt(_south, XMINT3(index.x, index.y-1, TS.ChunkSizeZ-1)))
				slides.push_back(VoxelRef(_south, index.x, index.y, TS.ChunkSizeZ-1));
		}
	}
}

void DynamicTerrainChunkUpdater::TrySlideVoxelWest(XMINT3& index, vector<VoxelRef>& slides) {
	if (index.x != 0) {
		if (!IsVoxelActiveAt(_chunk, XMINT3(index.x-1, index.y, index.z)) &&
			!IsVoxelActiveAt(_chunk, XMINT3(index.x-1, index.y-1, index.z)))
			slides.push_back(VoxelRef(_chunk, index.x-1, index.y, index.z));
	} else {
		if (_westActive && _west->IsLoaded()) {
			if (!IsVoxelActiveAt(_west, XMINT3(TS.ChunkSizeX-1, index.y, index.z)) &&
				!IsVoxelActiveAt(_west, XMINT3(TS.ChunkSizeX-1, index.y-1, index.z)))
				slides.push_back(VoxelRef(_west, TS.ChunkSizeX-1, index.y, index.z));
		}
	}
}

void DynamicTerrainChunkUpdater::TrySlideVoxelNorthEast(XMINT3& index, vector<VoxelRef>& slides) {
	if (!IsVoxelActiveAt(_chunk, XMINT3(index.x+1, index.y, index.z+1)) &&
		!IsVoxelActiveAt(_chunk, XMINT3(index.x+1, index.y-1, index.z+1)))
		slides.push_back(VoxelRef(_chunk, index.x+1, index.y, index.z+1));
}

void DynamicTerrainChunkUpdater::TrySlideVoxelSouthEast(XMINT3& index, vector<VoxelRef>& slides) {
	if (!IsVoxelActiveAt(_chunk, XMINT3(index.x+1, index.y, index.z-1)) &&
		!IsVoxelActiveAt(_chunk, XMINT3(index.x+1, index.y-1, index.z-1)))
		slides.push_back(VoxelRef(_chunk, index.x+1, index.y, index.z-1));
}

void DynamicTerrainChunkUpdater::TrySlideVoxelSouthWest(XMINT3& index, vector<VoxelRef>& slides) {
	if (!IsVoxelActiveAt(_chunk, XMINT3(index.x-1, index.y, index.z-1)) &&
		!IsVoxelActiveAt(_chunk, XMINT3(index.x-1, index.y-1, index.z-1)))
		slides.push_back(VoxelRef(_chunk, index.x-1, index.y, index.z-1));
}

void DynamicTerrainChunkUpdater::TrySlideVoxelNorthWest(XMINT3& index, vector<VoxelRef>& slides) {
	if (!IsVoxelActiveAt(_chunk, XMINT3(index.x-1, index.y, index.z+1)) &&
		!IsVoxelActiveAt(_chunk, XMINT3(index.x-1, index.y-1, index.z+1)))
		slides.push_back(VoxelRef(_chunk, index.x-1, index.y, index.z+1));
}

VoxelRef& DynamicTerrainChunkUpdater::ChooseRandomSlide(vector<VoxelRef>& slides) {
	int r = rand() % slides.size();
	assert(r >= 0 && r < slides.size());
	return slides[r];
}

void DynamicTerrainChunkUpdater::UpdateVoxel(DynamicTerrainChunk* chunk, XMINT3& index) { 
	std::lock_guard<mutex> lock(chunk->_chunkUpdater.GetAccessLock());
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x, index.y, index.z)); 
}

void DynamicTerrainChunkUpdater::UpdateVoxelsInRingAroundIndex(DynamicTerrainChunk* chunk, XMINT3& index, bool inclusive) {
	if (inclusive) UpdateVoxel(chunk, index);

	UpdateVoxel(chunk, XMINT3(index.x-1, index.y, index.z  ));
	UpdateVoxel(chunk, XMINT3(index.x,   index.y, index.z-1));
	UpdateVoxel(chunk, XMINT3(index.x+1, index.y, index.z  ));
	UpdateVoxel(chunk, XMINT3(index.x,   index.y, index.z+1));
}

void DynamicTerrainChunkUpdater::UpdateVoxelsAroundIndex(DynamicTerrainChunk* chunk, XMINT3& index, bool inclusive) {
	std::lock_guard<mutex> lock(chunk->_chunkUpdater.GetAccessLock());

	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x, index.y-1, index.z));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x, index.y+1, index.z));
													 
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x-1, index.y, index.z-1));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x-1, index.y, index.z));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x-1, index.y, index.z+1));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x,   index.y, index.z+1));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x+1, index.y, index.z+1));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x+1, index.y, index.z));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x+1, index.y, index.z-1));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x,   index.y, index.z-1));
													 
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x-1, index.y-1, index.z-1));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x-1, index.y-1, index.z));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x-1, index.y-1, index.z+1));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x,   index.y-1, index.z+1));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x+1, index.y-1, index.z+1));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x+1, index.y-1, index.z));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x+1, index.y-1, index.z-1));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x,   index.y-1, index.z-1));
													 
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x-1, index.y+1, index.z-1));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x-1, index.y+1, index.z));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x-1, index.y+1, index.z+1));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x,   index.y+1, index.z+1));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x+1, index.y+1, index.z+1));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x+1, index.y+1, index.z));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x+1, index.y+1, index.z-1));
	chunk->_voxelsToUpdate.push_back(VoxelRef(chunk, index.x,   index.y+1, index.z-1));
}

bool DynamicTerrainChunkUpdater::IsVoxelActiveAt(DynamicTerrainChunk* chunk, XMINT3& index) {
	if (IsVoxelOutOfBounds(chunk, index)) return true;

	std::lock_guard<mutex> lock(chunk->_chunkUpdater.GetAccessLock());
	return chunk->_voxels[index.x][index.y][index.z].Active;
}

bool DynamicTerrainChunkUpdater::IsVoxelOutOfBounds(DynamicTerrainChunk* chunk, XMINT3& index) {
	return (index.x < 0 || index.y < 1 || index.z < 0 ||
			index.x > TS.ChunkSizeX - 1 || 
			index.y > TS.ChunkSizeY - 1 || 
			index.z > TS.ChunkSizeZ - 1);
}

bool DynamicTerrainChunkUpdater::IsVoxelAtEdge(DynamicTerrainChunk* chunk, XMINT3& index) {
	return (index.x == 0 || index.y == 0 || index.z == 0 ||
			index.x == TS.ChunkSizeX - 1 || 
			index.y == TS.ChunkSizeY - 1 || 
			index.z == TS.ChunkSizeZ - 1);
}

void DynamicTerrainChunkUpdater::ActivateVoxel(DynamicTerrainChunk* chunk, XMINT3& index) {
	std::lock_guard<mutex> lock(chunk->_chunkUpdater.GetAccessLock());
	chunk->_voxels[index.x][index.y][index.z].Active = true;
}

void DynamicTerrainChunkUpdater::DeactivateVoxel(DynamicTerrainChunk* chunk, XMINT3& index) {
	std::lock_guard<mutex> lock(chunk->_chunkUpdater.GetAccessLock());
	chunk->_voxels[index.x][index.y][index.z].Active = false;
}