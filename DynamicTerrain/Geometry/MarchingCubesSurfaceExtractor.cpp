#include "MarchingCubesSurfaceExtractor.h"
#include "DynamicTerrainChunk.h"
#include "DynamicTerrainSystem.h"

using namespace Engine::Geometry;

RVAL MarchingCubesSurfaceExtractor::Extract(ID3D11Device* const d3dDevice, 
											DynamicTerrainChunk* chunk,
											vector<Vertex>& vertices,
											vector<UINT>& indices) {
	vector<vector<XMFLOAT3>> triangles;
	auto numTriangles = 0;

	auto heightField = chunk->_parentRef->GetHeightFieldForChunk(chunk->_chunkIndexX, chunk->_chunkIndexZ);

	Voxel* voxel = nullptr;

	float dA, dB, dC, dD, dE, dF, dG, dH;

	for (auto x = 0; x < TS.ChunkSizeX; x++)
	for (auto y = 0; y < TS.ChunkSizeY; y++)
	for (auto z = 0; z < TS.ChunkSizeZ; z++) {
		voxel = &chunk->_voxels[x][y][z];

		dA = y -     heightField[x  ][z  ] * TS.ChunkSizeY;
		dB = y + 1 - heightField[x  ][z  ] * TS.ChunkSizeY;
		dC = y + 1 - heightField[x+1][z  ] * TS.ChunkSizeY;
		dD = y -     heightField[x+1][z  ] * TS.ChunkSizeY;
		dE = y -     heightField[x  ][z+1] * TS.ChunkSizeY;
		dF = y + 1 - heightField[x  ][z+1] * TS.ChunkSizeY;
		dG = y + 1 - heightField[x+1][z+1] * TS.ChunkSizeY;
		dH = y -     heightField[x+1][z+1] * TS.ChunkSizeY;

		int bitmask = 0;

		if (dA < TS.IsoLevel) bitmask |= 1;
		if (dB < TS.IsoLevel) bitmask |= 2;
		if (dC < TS.IsoLevel) bitmask |= 4;
		if (dD < TS.IsoLevel) bitmask |= 8;
		if (dE < TS.IsoLevel) bitmask |= 16;
		if (dF < TS.IsoLevel) bitmask |= 32;
		if (dG < TS.IsoLevel) bitmask |= 64;
		if (dH < TS.IsoLevel) bitmask |= 128;

		if (MCEdgeTable[bitmask] == 0) continue;

		XMFLOAT3 intersectionVerts[12]; // Vertex on each edge where the surface intersects

		if (MCEdgeTable[bitmask] & 1)
			intersectionVerts[0] = Lerp(voxel->Points[0], voxel->Points[1], dA, dB);
												  
		if (MCEdgeTable[bitmask] & 2)	  
			intersectionVerts[1] = Lerp(voxel->Points[1], voxel->Points[2], dB, dC);
												  
		if (MCEdgeTable[bitmask] & 4)	  
			intersectionVerts[2] = Lerp(voxel->Points[2], voxel->Points[3], dC, dD);
												  
		if (MCEdgeTable[bitmask] & 8)	  
			intersectionVerts[3] = Lerp(voxel->Points[3], voxel->Points[0], dD, dA);
												  
		if (MCEdgeTable[bitmask] & 16)	  
			intersectionVerts[4] = Lerp(voxel->Points[4], voxel->Points[5], dE, dF);
												  
		if (MCEdgeTable[bitmask] & 32)	  
			intersectionVerts[5] = Lerp(voxel->Points[5], voxel->Points[6], dF, dG);
												  
		if (MCEdgeTable[bitmask] & 64)	  
			intersectionVerts[6] = Lerp(voxel->Points[6], voxel->Points[7], dG, dH);
												  
		if (MCEdgeTable[bitmask] & 128)	  
			intersectionVerts[7] = Lerp(voxel->Points[7], voxel->Points[4], dH, dE);
												  
		if (MCEdgeTable[bitmask] & 256)	  
			intersectionVerts[8] = Lerp(voxel->Points[0], voxel->Points[4], dA, dE);
												  
		if (MCEdgeTable[bitmask] & 512)	  
			intersectionVerts[9] = Lerp(voxel->Points[1], voxel->Points[5], dB, dF);
												   
		if (MCEdgeTable[bitmask] & 1024)   
			intersectionVerts[10] = Lerp(voxel->Points[2], voxel->Points[6], dC, dG);
												   
		if (MCEdgeTable[bitmask] & 2048)   
			intersectionVerts[11] = Lerp(voxel->Points[3], voxel->Points[7], dD, dH);

		for (auto i = 0; MCTriTable[bitmask][i] != -1; i+=3) {
			auto tri = vector<XMFLOAT3>(3);
			tri[0] = intersectionVerts[MCTriTable[bitmask][i]];
			tri[1] = intersectionVerts[MCTriTable[bitmask][i+1]];
			tri[2] = intersectionVerts[MCTriTable[bitmask][i+2]];
			triangles.push_back(tri);
			numTriangles++;
		}

	}

	auto numVerts = 0;

	for (auto i = 0; i < numTriangles; i++) {
		indices.push_back(numVerts);
		indices.push_back(numVerts + 1);
		indices.push_back(numVerts + 2);

		auto vPOne = XMLoadFloat3(&triangles[i][0]);
		auto vPTwo = XMLoadFloat3(&triangles[i][1]);
		auto vPThree = XMLoadFloat3(&triangles[i][2]);
		auto u = vPTwo - vPOne;
		auto v = vPThree - vPOne;
		auto x = DirectX::XMVector3Cross(u, v);

		XMFLOAT3 normal;
		XMStoreFloat3(&normal, x);

		vertices.push_back(Vertex(triangles[i][0], normal));
		vertices.push_back(Vertex(triangles[i][1], normal));
		vertices.push_back(Vertex(triangles[i][2], normal));

		numVerts += 3;
	}

	voxel = nullptr;

	for (auto i = 0; i < TS.ChunkSizeX+1; i++)
		delete [] heightField[i];
	delete [] heightField;

	return RVAL::R_OK;
}

XMFLOAT3 MarchingCubesSurfaceExtractor::Lerp(const XMFLOAT3& pointOne, const XMFLOAT3& pointTwo, float densityOne, float densityTwo) {
	if (abs(TS.IsoLevel - densityOne) < 0.00001f) return pointOne;
	if (abs(TS.IsoLevel - densityTwo) < 0.00001f) return pointTwo;
	if (abs(densityOne - densityTwo) < 0.00001f) return pointOne;

	auto mu = (TS.IsoLevel - densityOne) / (densityTwo - densityOne);
	return XMFLOAT3(pointOne.x + mu * (pointTwo.x - pointOne.x),
					pointOne.y + mu * (pointTwo.y - pointOne.y),
					pointOne.z + mu * (pointTwo.z - pointOne.z));
}