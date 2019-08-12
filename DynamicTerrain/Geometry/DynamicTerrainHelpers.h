#ifndef ENGINE_GEOMETRY_DYNAMICTERRAINHELPERS_H
#define ENGINE_GEOMETRY_DYNAMICTERRAINHELPERS_H

#include <DirectXMath.h>

namespace Engine
{
namespace Geometry
{

class DynamicTerrainChunk;

class _declspec(dllexport) DynamicTerrainHelpers {
public:
	static int GetHeighestVoxelIndexAtXZIndex(DynamicTerrainChunk* const chunk, int x, int z);
};

typedef DynamicTerrainHelpers DTHelpers;

}
}

#endif