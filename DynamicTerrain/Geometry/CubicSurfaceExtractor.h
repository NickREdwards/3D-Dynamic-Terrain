#ifndef ENGINE_GEOMETRY_CUBICSURFACEEXTRACTOR_H
#define ENGINE_GEOMETRY_CUBICSURFACEEXTRACTOR_H

#include <vector>

#include "ISurfaceExtractor.h"
#include "DynamicTerrainSettings.h"
#include "Vertex.h"

using std::vector;

namespace Engine
{
namespace Geometry
{

class DynamicTerrainChunk;

class _declspec(dllexport) CubicSurfaceExtractor: public ISurfaceExtractor {
public:
	CubicSurfaceExtractor() {}
	~CubicSurfaceExtractor() {}

	RVAL Extract(ID3D11Device* const d3dDevice,
				 DynamicTerrainChunk* chunk,
				 vector<Vertex>& vertices,
				 vector<UINT>& indices);

private:
	bool VoxelSurrounded(int x, int y, int z, DynamicTerrainChunk* chunk);

	void AddVerticesForFace(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3, XMFLOAT3 p4, XMFLOAT3 normal, vector<Vertex>& vertices);
	void AddIndicesForFace(int numVertices, vector<UINT>& indices);
};

}
}

#endif