#ifndef ENGINE_GEOMETRY_DYNAMICTERRAINSETTINGS_H
#define ENGINE_GEOMETRY_DYNAMICTERRAINSETTINGS_H

#include <DirectXMath.h>

using DirectX::XMFLOAT3;
using DirectX::XMINT3;

namespace Engine
{
namespace Geometry
{

class DynamicTerrainChunk;

struct Voxel {
	bool Active;
	XMFLOAT3 Points[8];
	Voxel() : Active(false) {}
};

struct VoxelRef {
	int X, Y, Z;
	DynamicTerrainChunk* ChunkRef;
	VoxelRef(DynamicTerrainChunk* _chunkRef, int _x, int _y, int _z) : ChunkRef(_chunkRef), X(_x), Y(_y), Z(_z) {}
	bool operator==(const VoxelRef& b) { return ChunkRef == b.ChunkRef && X == b.X && Y == b.Y && Z == b.Z; }
	bool operator!=(const VoxelRef& b) { return ChunkRef != b.ChunkRef || X != b.X | Y != b.Y || Z != b.Z; }
	bool operator<(const VoxelRef& b) { return X < b.X || X <= b.X && Y < b.Y || X <= b.X && Y <= b.Y && Z < b.Z; }
};

struct TerrainSettings {
	int CubeSize;
	int NumMeshingThreads;
	int NumUpdateThreads;
	int NoiseSeed;
	int NoiseOctaves;

	static const int ChunkSizeX = 16;
	static const int ChunkSizeY = 32;
	static const int ChunkSizeZ = 16;

	static const int NumChunksX = 16;
	static const int NumChunksZ = 16;

	float IsoLevel;
	float NoiseFrequency;
	float NoisePersistence;

	XMINT3 ChunkDimensions;
	XMFLOAT3 TerrainCentre;

	TerrainSettings()
		:	CubeSize(8),
			NumMeshingThreads(2),
			NumUpdateThreads(2),
			NoiseSeed(4532345),
			NoiseOctaves(6),
			IsoLevel(0.0f),
			NoiseFrequency(0.5f),
			NoisePersistence(0.5f) {
		Recalculate();
	}

	void Recalculate() {
		ChunkDimensions = XMINT3(ChunkSizeX * CubeSize, ChunkSizeY * CubeSize, ChunkSizeZ * CubeSize);
		TerrainCentre = XMFLOAT3((ChunkDimensions.x * NumChunksX) / 2, 0.0f, (ChunkDimensions.z * NumChunksZ) / 2);
	}
};

}
}

extern Engine::Geometry::TerrainSettings TS;

#endif