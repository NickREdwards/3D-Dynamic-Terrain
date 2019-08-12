#ifndef ENGINE_GEOMETRY_DYNAMICTERRAINCHUNKUPDATER_H
#define ENGINE_GEOMETRY_DYNAMICTERRAINCHUNKUPDATER_H

#include <algorithm>
#include <vector>
#include <mutex>
#include <DirectXMath.h>

#include "DynamicTerrainSettings.h"

using DirectX::XMINT3;
using std::vector;
using std::mutex;

namespace Engine
{
namespace Geometry
{

class DynamicTerrainChunk;

class _declspec(dllexport) DynamicTerrainChunkUpdater {
public:
	DynamicTerrainChunkUpdater(DynamicTerrainChunk* chunk)
		:	_chunk(chunk),
			_north(nullptr),
			_east(nullptr),
			_south(nullptr),
			_west(nullptr),
			_northActive(false),
			_eastActive(false),
			_southActive(false),
			_westActive(false) {}

	~DynamicTerrainChunkUpdater() {
		_chunk = nullptr;
		_north = nullptr;
		_east = nullptr;
		_south = nullptr;
		_west = nullptr;
	}

	void Update();
	void UpdateNeighbours(XMINT3& index);

	bool DropVoxel(XMINT3& index);
	bool SlideVoxel(XMINT3& index);

	void TrySlideVoxelNorth(XMINT3& index, vector<VoxelRef>& slides);
	void TrySlideVoxelEast(XMINT3& index, vector<VoxelRef>& slides);
	void TrySlideVoxelSouth(XMINT3& index, vector<VoxelRef>& slides);
	void TrySlideVoxelWest(XMINT3& index, vector<VoxelRef>& slides);

	void TrySlideVoxelNorthEast(XMINT3& index, vector<VoxelRef>& slides);
	void TrySlideVoxelSouthEast(XMINT3& index, vector<VoxelRef>& slides);
	void TrySlideVoxelSouthWest(XMINT3& index, vector<VoxelRef>& slides);
	void TrySlideVoxelNorthWest(XMINT3& index, vector<VoxelRef>& slides);

	VoxelRef& ChooseRandomSlide(vector<VoxelRef>& slides);

	void UpdateVoxel(DynamicTerrainChunk* chunk, XMINT3& index);
	void UpdateVoxelsInRingAroundIndex(DynamicTerrainChunk* chunk, XMINT3& index, bool inclusive = true);
	void UpdateVoxelsAroundIndex(DynamicTerrainChunk* chunk, XMINT3& index, bool inclusive = true);

	bool IsVoxelActiveAt(DynamicTerrainChunk* chunk, XMINT3& index);
	bool IsVoxelOutOfBounds(DynamicTerrainChunk* chunk, XMINT3& index);
	bool IsVoxelAtEdge(DynamicTerrainChunk* chunk, XMINT3& index);

	void ActivateVoxel(DynamicTerrainChunk* chunk, XMINT3& index);
	void DeactivateVoxel(DynamicTerrainChunk* chunk, XMINT3& index);

	inline void SetNorthChunk(DynamicTerrainChunk* chunk) { _north = chunk; _northActive = true; }
	inline void SetEastChunk(DynamicTerrainChunk* chunk) { _east = chunk; _eastActive = true; }
	inline void SetSouthChunk(DynamicTerrainChunk* chunk) { _south = chunk; _southActive = true; }
	inline void SetWestChunk(DynamicTerrainChunk* chunk) { _west = chunk; _westActive = true; }

	inline mutex& GetAccessLock() { return _accessLock; }

private:
	DynamicTerrainChunk* _chunk;
	DynamicTerrainChunk* _north;
	DynamicTerrainChunk* _east;
	DynamicTerrainChunk* _south;
	DynamicTerrainChunk* _west;

	bool _northActive;
	bool _eastActive;
	bool _southActive;
	bool _westActive;

	mutex _accessLock;
};

}
}

#endif