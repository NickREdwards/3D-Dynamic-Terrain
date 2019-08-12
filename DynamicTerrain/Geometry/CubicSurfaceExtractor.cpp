#include "CubicSurfaceExtractor.h"
#include "DynamicTerrainChunk.h"

using namespace Engine::Geometry;

const XMFLOAT3 normLeft  (-1.0f, 0.0f, 0.0f);
const XMFLOAT3 normRight (+1.0f, 0.0f, 0.0f);
const XMFLOAT3 normBottom(0.0f, -1.0f, 0.0f);
const XMFLOAT3 normTop   (0.0f, +1.0f, 0.0f);
const XMFLOAT3 normFront (0.0f, 0.0f, -1.0f);
const XMFLOAT3 normBack  (0.0f, 0.0f, +1.0f);

// Macros are used instead of variables to speed the algorithm up (no variable assignments)
#define pA currentVoxel->Points[0]
#define pB currentVoxel->Points[1]
#define pC currentVoxel->Points[2]
#define pD currentVoxel->Points[3]
#define pE currentVoxel->Points[4]
#define pF currentVoxel->Points[5]
#define pG currentVoxel->Points[6]
#define pH currentVoxel->Points[7]

#define xNeg (x > 0 && chunk->_voxels[x-1][y  ][z  ].Active)
#define yNeg (y > 0 && chunk->_voxels[x  ][y-1][z  ].Active)
#define zNeg (z > 0 && chunk->_voxels[x  ][y  ][z-1].Active)
#define xPos (x < TS.ChunkSizeX - 1 && chunk->_voxels[x+1][y  ][z  ].Active)
#define yPos (y < TS.ChunkSizeY - 1 && chunk->_voxels[x  ][y+1][z  ].Active)
#define zPos (z < TS.ChunkSizeZ - 1 && chunk->_voxels[x  ][y  ][z+1].Active)

RVAL CubicSurfaceExtractor::Extract(ID3D11Device* const d3dDevice, 
									DynamicTerrainChunk* chunk,
									vector<Vertex>& vertices,
									vector<UINT>& indices) {
	Voxel* currentVoxel;

	for (auto x = 0; x < TS.ChunkSizeX; x++)
	for (auto y = 0; y < TS.ChunkSizeY; y++)
	for (auto z = 0; z < TS.ChunkSizeZ; z++) {
		currentVoxel = &chunk->_voxels[x][y][z];

		// Is the voxel completely hidden?
		if (!currentVoxel->Active || VoxelSurrounded(x, y, z, chunk)) continue;
		
		// Check for hidden facets
		// AddIndicesForFace must check vertices.size() each time

		if (!xNeg) {
			AddIndicesForFace(vertices.size(), indices);
			AddVerticesForFace(pE, pF, pB, pA, normLeft, vertices);
		}

		if (!xPos) {
			AddIndicesForFace(vertices.size(), indices);
			AddVerticesForFace(pD, pC, pG, pH, normRight, vertices);
		}

		if (!yNeg) {
			AddIndicesForFace(vertices.size(), indices);
			AddVerticesForFace(pE, pA, pD, pH, normBottom, vertices);
		}

		if (!yPos) {
			AddIndicesForFace(vertices.size(), indices);
			AddVerticesForFace(pB, pF, pG, pC, normTop, vertices);
		}

		if (!zNeg) {
			AddIndicesForFace(vertices.size(), indices);
			AddVerticesForFace(pA, pB, pC, pD, normFront, vertices);
		}

		if (!zPos) {
			AddIndicesForFace(vertices.size(), indices);
			AddVerticesForFace(pH, pG, pF, pE, normBack, vertices);
		}
	}

	return RVAL::R_OK;
}

bool CubicSurfaceExtractor::VoxelSurrounded(int x, int y, int z, DynamicTerrainChunk* chunk) {
	if (x == 0 || x == TS.ChunkSizeX - 1 ||
		y == 0 || y == TS.ChunkSizeY - 1 ||
		z == 0 || z == TS.ChunkSizeZ - 1) return false;

	if (chunk->_voxels[x-1][y][z].Active && chunk->_voxels[x+1][y][z].Active && 
		chunk->_voxels[x][y-1][z].Active && chunk->_voxels[x][y+1][z].Active && 
		chunk->_voxels[x][y][z-1].Active && chunk->_voxels[x][y][z+1].Active) return true;
	
	return false;
}

void CubicSurfaceExtractor::AddVerticesForFace(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3, XMFLOAT3 p4, XMFLOAT3 normal, vector<Vertex>& vertices) {
	vertices.push_back(Vertex(p1, normal));
	vertices.push_back(Vertex(p2, normal));
	vertices.push_back(Vertex(p3, normal));
	vertices.push_back(Vertex(p4, normal));
}

void CubicSurfaceExtractor::AddIndicesForFace(int numVertices, vector<UINT>& indices) {
	indices.push_back(numVertices); 
	indices.push_back(numVertices + 1); 
	indices.push_back(numVertices + 3);
	indices.push_back(numVertices + 2); 
	indices.push_back(numVertices + 3); 
	indices.push_back(numVertices + 1);
}