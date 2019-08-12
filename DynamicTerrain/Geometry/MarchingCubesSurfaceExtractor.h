#ifndef ENGINE_GEOMETRY_MARCHINGCUBESSURFACEEXTRACTOR_H
#define ENGINE_GEOMETRY_MARCHINGCUBESSURFACEEXTRACTOR_H

#include <DirectXMath.h>
#include <vector>

#include "ISurfaceExtractor.h"
#include "DynamicTerrainSettings.h"
#include "MarchingCubesTables.h"
#include "Vertex.h"

using std::vector;
using DirectX::XMFLOAT3;
using DirectX::operator-;

namespace Engine
{
namespace Geometry
{

class DynamicTerrainChunk;

class _declspec(dllexport) MarchingCubesSurfaceExtractor: public ISurfaceExtractor {
public:
	MarchingCubesSurfaceExtractor() {}
	~MarchingCubesSurfaceExtractor() {}

	RVAL Extract(ID3D11Device* const d3dDevice, 
				 DynamicTerrainChunk* chunk,
				 vector<Vertex>& vertices,
				 vector<UINT>& indices);

private:
	XMFLOAT3 Lerp(const XMFLOAT3& pointOne, const XMFLOAT3& pointTwo, float densityOne, float densityTwo);

};

}
}

#endif