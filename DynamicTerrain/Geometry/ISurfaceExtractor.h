#ifndef ENGINE_GEOMETRY_ISURFACEEXTRACTOR_H
#define ENGINE_GEOMETRY_ISURFACEEXTRACTOR_H

#include <D3D11.h>
#include <vector>

#include "..\Geometry\Vertex.h"
#include "..\Core\MethodHelpers.h"
#include "DynamicTerrainSettings.h"

using Engine::Core::RVAL;
using std::vector;

namespace Engine
{
namespace Geometry
{

class DynamicTerrainChunk;

class _declspec(dllexport) ISurfaceExtractor {
public:
	virtual ~ISurfaceExtractor() {}

	virtual RVAL Extract(ID3D11Device* const d3dDevice, 
						 DynamicTerrainChunk* chunk,
						 //Voxel (&voxels)[TS.ChunkSizeX][TS.ChunkSizeY][TS.ChunkSizeZ],
						 vector<Vertex>& vertices,
						 vector<UINT>& indices) = 0;
};

}
}

#endif