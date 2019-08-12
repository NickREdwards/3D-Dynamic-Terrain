#ifndef ENGINE_GEOMETRY_DYNAMICTERRAINCHUNK_H
#define ENGINE_GEOMETRY_DYNAMICTERRAINCHUNK_H

#include <vector>
#include <DirectXMath.h>

#include "Object3D.h"
#include "DynamicTerrainSettings.h"
#include "DynamicTerrainChunkUpdater.h"
#include "DynamicTerrainHelpers.h"
#include "..\Helpers\Math.h"
#include "..\Helpers\AABB.h"

using std::vector;

namespace Engine
{
namespace Geometry
{

class IDynamicTerrainSystem;
class ISurfaceExtractor;

class _declspec(dllexport) DynamicTerrainChunk: public Object3D {
friend class DynamicTerrainSystem;
friend class DynamicTerrainChunkUpdater;
friend class DynamicTerrainManipulator;
friend class DynamicTerrainHelpers;
friend class CubicSurfaceExtractor;
friend class MarchingCubesSurfaceExtractor;

public:
	DynamicTerrainChunk(IDynamicTerrainSystem* const parentRef);
	~DynamicTerrainChunk();

	bool operator==(DynamicTerrainChunk& c);

	RVAL Init();
	RVAL RebuildMesh(ID3D11Device* const d3dDevice);
	RVAL InitMesh(ID3D11Device* const d3dDevice) override;

	void RebuildAABB() override;

	void Reset();
	void Update();

	void ForceActiveExtractor();
	void DirtyBuffer();

	inline void SetChunkIndex(int x, int z) { _chunkIndexX = x; _chunkIndexZ = z; }

	inline IMesh* GetBackupMesh() { return _backupMesh; }

	inline bool IsLoaded() { return _isLoaded; }
	inline bool IsAwaitingRemesh() { return _awaitingReMesh; }

private:
	IDynamicTerrainSystem* _parentRef;
	IMesh* _backupMesh;

	ISurfaceExtractor* _activeExtractor;
	ISurfaceExtractor* _smoothExtractor;

	vector<VoxelRef> _voxelsToUpdate;
	DynamicTerrainChunkUpdater _chunkUpdater;

	bool _isLoaded;
	bool _awaitingReMesh;
	bool _backupMeshRebuilding;
	bool _smoothTerrain;
	bool _bufferIsDirty;

	int _chunkIndexX;
	int _chunkIndexZ;

	Voxel _voxels[TS.ChunkSizeX]
				 [TS.ChunkSizeY]
				 [TS.ChunkSizeZ];

	void InitFromHeightfield();

	void CalculatePointsForVoxel(Voxel& voxel, int x, int y, int z);
};

}
}

#endif