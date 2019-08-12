#include "DynamicTerrainManipulator.h"

using namespace Engine::Geometry;

void DynamicTerrainManipulator::RemoveVoxelsAroundPoint(DynamicTerrainChunk* const chunk, const XMFLOAT3& point, float radius) {
	chunk->ForceActiveExtractor();

	auto indexPoint = GetClosestIndexToPoint(chunk, point);
	auto affectedVoxels = GetVoxelsInRadiusAroundIndex(chunk, indexPoint, radius);

	if (affectedVoxels.size() < 1) return;

	for (auto& v : affectedVoxels) {
		chunk->_chunkUpdater.DeactivateVoxel(chunk, XMINT3(v.X, v.Y, v.Z));
		chunk->_chunkUpdater.UpdateVoxelsAroundIndex(chunk, XMINT3(v.X, v.Y, v.Z), false);
	}

	chunk->DirtyBuffer();
}

void DynamicTerrainManipulator::AddVoxelsAroundPoint(DynamicTerrainChunk* const chunk, const XMFLOAT3& point, float radius) {
	chunk->ForceActiveExtractor();

	auto indexPoint = GetClosestIndexToPoint(chunk, point);
	indexPoint.y += 10;

	auto affectedVoxels = GetVoxelsInRadiusAroundIndex(chunk, indexPoint, radius);

	if (affectedVoxels.size() < 1) return;

	for (auto& v : affectedVoxels) {
		chunk->_chunkUpdater.ActivateVoxel(chunk, XMINT3(v.X, v.Y, v.Z));
		chunk->_chunkUpdater.UpdateVoxelsAroundIndex(chunk, XMINT3(v.X, v.Y, v.Z), false);
	}

	chunk->DirtyBuffer();
}

void DynamicTerrainManipulator::DisplaceVoxelsAroundPoint(DynamicTerrainChunk* const chunk, const XMFLOAT3& point, float radius) {
	chunk->ForceActiveExtractor();

	auto doubleRadius = radius * 2;

	auto indexPoint = GetClosestIndexToPoint(chunk, point);

	auto affectedVoxels = GetVoxelsInRingAroundIndex(chunk, indexPoint, doubleRadius);
	if (affectedVoxels.size() < 1) return;

	// Remove
	auto voxelsToRemove = GetVoxelsInRadiusAroundIndex(chunk, indexPoint, radius);

	for (auto& v : voxelsToRemove) {
		chunk->_chunkUpdater.DeactivateVoxel(chunk, XMINT3(v.X, v.Y, v.Z));
		chunk->_chunkUpdater.UpdateVoxelsAroundIndex(chunk, XMINT3(v.X, v.Y, v.Z), false);
	}

	auto radiusSquared = radius * radius - (radius / 2);

	// Add
	float dX, dY, dZ;
	float distSquared;
	for (auto& v : affectedVoxels) {
		dX = v.X - indexPoint.x;
		dY = v.Y - indexPoint.y;
		dZ = v.Z - indexPoint.z;

		distSquared = (dX * dX) + (dY * dY) + (dZ * dZ);

		if (distSquared > radiusSquared) {
			AddVoxelsAtXZIndex(chunk, v.X, v.Z, static_cast<int>(distSquared / radiusSquared));
		}
	}

	chunk->DirtyBuffer();
}

void DynamicTerrainManipulator::AddVoxelsAtXZIndex(DynamicTerrainChunk* const chunk, int x, int z, int amount) {
	auto maxY = DTHelpers::GetHeighestVoxelIndexAtXZIndex(chunk, 
														 max(0, min(TS.ChunkSizeX - 1, x)), 
														 max(0, min(TS.ChunkSizeZ - 1, z)));
	
	for (auto i = 1; i <= amount; i++) {
		if (maxY + i > TS.ChunkSizeY - 1) break;

		chunk->_chunkUpdater.ActivateVoxel(chunk, XMINT3(x, maxY + i, z));
		chunk->_chunkUpdater.UpdateVoxelsAroundIndex(chunk, XMINT3(x, maxY + i, z), false);
	}
}

XMINT3 DynamicTerrainManipulator::GetClosestIndexToPoint(DynamicTerrainChunk* const chunk, const XMFLOAT3& point) {
	XMINT3 indexPoint;
	
	auto pointVector = XMLoadFloat3(&point);
	auto chunkOrigin = XMLoadFloat3(&chunk->GetPosition());
	auto differenceVector = pointVector - chunkOrigin;

	XMFLOAT3 difference;
	XMStoreFloat3(&difference, differenceVector);

	auto diffX = difference.x / TS.CubeSize;
	auto diffZ = difference.z / TS.CubeSize;

	indexPoint.x = static_cast<int>(floorf(diffX));
	indexPoint.z = static_cast<int>(floorf(diffZ));
	indexPoint.y = DTHelpers::GetHeighestVoxelIndexAtXZIndex(chunk, 
															 max(0, min(TS.ChunkSizeX - 1, indexPoint.x)), 
															 max(0, min(TS.ChunkSizeZ - 1, indexPoint.z)));

	return indexPoint;
}

XMINT3 DynamicTerrainManipulator::GetIndexStart(XMINT3& indexPoint, float radius) {
	return XMINT3(max(0, indexPoint.x - radius),
				  max(0, indexPoint.y - radius),
				  max(0, indexPoint.z - radius));
}

XMINT3 DynamicTerrainManipulator::GetIndexEnd(XMINT3& indexPoint, float radius) {
	return XMINT3(min(TS.ChunkSizeX-1, indexPoint.x + radius),
				  min(TS.ChunkSizeY-1, indexPoint.y + radius),
				  min(TS.ChunkSizeZ-1, indexPoint.z + radius));
}

vector<VoxelRef> DynamicTerrainManipulator::GetVoxelsInRadiusAroundIndex(DynamicTerrainChunk* const chunk, XMINT3& indexPoint, float radius) {
	vector<VoxelRef> affectedVoxels;

	auto indexStart = GetIndexStart(indexPoint, radius);
	auto indexEnd = GetIndexEnd(indexPoint, radius);
	auto radiusSquared = radius * radius;

	float dX, dY, dZ; // Index distance from the indexPoint
	float distSquared;

	for (auto x = indexStart.x; x <= indexEnd.x; x++)
	for (auto y = indexStart.y; y <= indexEnd.y; y++)
	for (auto z = indexStart.z; z <= indexEnd.z; z++) {
		dX = x - indexPoint.x;
		dY = y - indexPoint.y;
		dZ = z - indexPoint.z;

		distSquared = (dX * dX) + (dY * dY) + (dZ * dZ);

		if (distSquared <= radiusSquared)
			affectedVoxels.push_back(VoxelRef(chunk, x, y, z));
	}

	return affectedVoxels;
}

vector<VoxelRef> DynamicTerrainManipulator::GetVoxelsInRingAroundIndex(DynamicTerrainChunk* const chunk, XMINT3& indexPoint, float radius) {
	vector<VoxelRef> affectedVoxels;

	auto indexStart = GetIndexStart(indexPoint, radius);
	auto indexEnd = GetIndexEnd(indexPoint, radius);
	auto radiusSquared = radius * radius;

	float dX, dY, dZ; // Index distance from the indexPoint
	float distSquared;
	int y;

	for (auto x = indexStart.x; x <= indexEnd.x; x++)
	for (auto z = indexStart.z; z <= indexEnd.z; z++) {
		dX = x - indexPoint.x;
		dZ = z - indexPoint.z;

		y = DTHelpers::GetHeighestVoxelIndexAtXZIndex(chunk, 
													  max(0, min(TS.ChunkSizeX - 1, indexPoint.x)), 
													  max(0, min(TS.ChunkSizeZ - 1, indexPoint.z)));

		distSquared = (dX * dX) + (dZ * dZ);

		if (distSquared <= radiusSquared)
			affectedVoxels.push_back(VoxelRef(chunk, x, y, z));
	}

	return affectedVoxels;
}