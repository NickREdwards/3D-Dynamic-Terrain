#ifndef ENGINE_GEOMETRY_DYNAMICTERRAINMANIPULATOR_H
#define ENGINE_GEOMETRY_DYNAMICTERRAINMANIPULATOR_H

#include <DirectXMath.h>
#include <mutex>

#include "DynamicTerrainChunk.h"
#include "DynamicTerrainChunkUpdater.h"
#include "DynamicTerrainSettings.h"
#include "DynamicTerrainHelpers.h"

using DirectX::XMFLOAT3;
using DirectX::XMINT3;
using DirectX::operator-;
using std::mutex;

namespace Engine
{
namespace Geometry
{

class _declspec(dllexport) DynamicTerrainManipulator {
public:
	DynamicTerrainManipulator() {}
	~DynamicTerrainManipulator() {}

	void RemoveVoxelsAroundPoint(DynamicTerrainChunk* const chunk, const XMFLOAT3& point, float radius);
	void AddVoxelsAroundPoint(DynamicTerrainChunk* const chunk, const XMFLOAT3& point, float radius);
	void DisplaceVoxelsAroundPoint(DynamicTerrainChunk* const chunk, const XMFLOAT3& point, float radius);

	void AddVoxelsAtXZIndex(DynamicTerrainChunk* const chunk, int x, int z, int amount);

	XMINT3 GetClosestIndexToPoint(DynamicTerrainChunk* const chunk, const XMFLOAT3& point);
	XMINT3 GetIndexStart(XMINT3& indexPoint, float radius);
	XMINT3 GetIndexEnd(XMINT3& indexPoint, float radius);

	vector<VoxelRef> GetVoxelsInRadiusAroundIndex(DynamicTerrainChunk* const chunk, XMINT3& indexPoint, float radius);
	vector<VoxelRef> GetVoxelsInRingAroundIndex(DynamicTerrainChunk* const chunk, XMINT3& indexPoint, float radius);
};

}
}

#endif