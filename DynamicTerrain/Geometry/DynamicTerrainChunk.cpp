#include "DynamicTerrainChunk.h"
#include "DynamicTerrainSystem.h"
#include "CubicSurfaceExtractor.h"
#include "MarchingCubesSurfaceExtractor.h"

using namespace Engine::Geometry;
using namespace noise;

DynamicTerrainChunk::DynamicTerrainChunk(IDynamicTerrainSystem* const parentRef)
	:	_parentRef(parentRef),
		_backupMesh(nullptr),
		_activeExtractor(nullptr),
		_smoothExtractor(nullptr),
		_chunkUpdater(this),
		_isLoaded(false),
		_awaitingReMesh(false),
		_backupMeshRebuilding(false),
		_smoothTerrain(false),
		_bufferIsDirty(false) {
}

DynamicTerrainChunk::~DynamicTerrainChunk() { 
	_parentRef = nullptr;
	delete _activeExtractor;
	delete _smoothExtractor;
}

bool DynamicTerrainChunk::operator==(DynamicTerrainChunk& c) {
	auto thisPos = GetPosition();
	auto thatPos = c.GetPosition();
	return	thisPos.x == thatPos.x &&
			thisPos.y == thatPos.y &&
			thisPos.z == thatPos.z;
}

RVAL DynamicTerrainChunk::Init() {
	_backupMesh = new Mesh;
	_activeExtractor = new CubicSurfaceExtractor;
	_smoothExtractor = new MarchingCubesSurfaceExtractor;

	Reset();
	return RVAL::R_OK;
}

RVAL DynamicTerrainChunk::RebuildMesh(ID3D11Device* const d3dDevice) {
	auto extractor = (_smoothTerrain) ? _smoothExtractor : _activeExtractor;
	if (extractor == nullptr) return RVAL::R_FAILED;

	vector<Vertex> vertices;
	vector<UINT> indices;

	// Make a guess at the total vertices/indices to increase speed
	vertices.reserve(100000);
	indices.reserve(150000);

	// Extract new surface, and rebuild the buffers on the mesh.
	// If we are currently in a rendering pass, wait for it to finish
	// then rebuild the backup mesh.

	if (RSUCCESS(extractor->Extract(d3dDevice, this, vertices, indices))) {
		if (RSUCCESS(GetMesh()->RebuildBuffers(d3dDevice,
											   vertices,
											   indices))) {
			_awaitingReMesh = false;
		} else return RVAL::R_FAILED;

		while (_backupMesh->IsBeingDrawn()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}

		_backupMeshRebuilding = true;

		if (RSUCCESS(_backupMesh->RebuildBuffers(d3dDevice,
												 vertices,
												 indices))) {
			_bufferIsDirty = false;
			_backupMeshRebuilding = false;
		} else return RVAL::R_FAILED;

		return RVAL::R_OK;
	}

	return RVAL::R_FAILED;
}

RVAL DynamicTerrainChunk::InitMesh(ID3D11Device* const d3dDevice) {
	if (RFAILURE(RebuildMesh(d3dDevice))) {
		_isLoaded = false;
		return RVAL::R_FAILED;
	}

	_isLoaded = true;
	return RVAL::R_OK;
}

void DynamicTerrainChunk::RebuildAABB() {
	auto position = GetPosition();
	_BoundingBox.Left	= position.x;
	_BoundingBox.Right	= position.x + TS.ChunkDimensions.x;
	_BoundingBox.Bottom	= position.y;
	_BoundingBox.Top	= position.y + TS.ChunkDimensions.y;
	_BoundingBox.Front	= position.z;
	_BoundingBox.Back	= position.z + TS.ChunkDimensions.z;
}

void DynamicTerrainChunk::Reset() {
	InitFromHeightfield();
}

void DynamicTerrainChunk::Update() {
	if (_smoothTerrain || _bufferIsDirty) return;
	_chunkUpdater.Update();
}

void DynamicTerrainChunk::ForceActiveExtractor() {
	if (!_smoothTerrain) return;
	_smoothTerrain = false;
	DirtyBuffer();
}

void DynamicTerrainChunk::DirtyBuffer() {
	_bufferIsDirty = true;
	_parentRef->DirtyHeightField();
}

void DynamicTerrainChunk::InitFromHeightfield() {
	auto heightField = _parentRef->GetHeightFieldForChunk(_chunkIndexX, _chunkIndexZ);

	float voxelHeight;

	for (auto x = 0; x < TS.ChunkSizeX; x++)
	for (auto y = 0; y < TS.ChunkSizeY; y++)
	for (auto z = 0; z < TS.ChunkSizeZ; z++) {
		voxelHeight = heightField[x][z] * TS.ChunkSizeY;
		_voxels[x][y][z].Active = y <= voxelHeight;
		CalculatePointsForVoxel(_voxels[x][y][z], x, y, z);
		_chunkUpdater.UpdateVoxel(this, XMINT3(x, y, z));
	}

	for (auto i = 0; i < TS.ChunkSizeX+1; i++)
		delete [] heightField[i];
	delete [] heightField;
}

void DynamicTerrainChunk::CalculatePointsForVoxel(Voxel& voxel, int x, int y, int z) {
	// CS = CUBE_SIZE, CSPCS = CUBE_SIZE + CUBE_SIZE
	float xCS, yCS, zCS, xCSPCS, yCSPCS, zCSPCS;

	xCS		= x * TS.CubeSize;
	yCS		= y * TS.CubeSize;
	zCS		= z * TS.CubeSize;
	xCSPCS	= x * TS.CubeSize + TS.CubeSize;
	yCSPCS	= y * TS.CubeSize + TS.CubeSize;
	zCSPCS	= z * TS.CubeSize + TS.CubeSize;

	voxel.Points[0] = XMFLOAT3(xCS,		yCS,	zCS);
	voxel.Points[1] = XMFLOAT3(xCS,		yCSPCS,	zCS);
	voxel.Points[2] = XMFLOAT3(xCSPCS,	yCSPCS,	zCS);
	voxel.Points[3] = XMFLOAT3(xCSPCS,	yCS,	zCS);
	voxel.Points[4] = XMFLOAT3(xCS,		yCS,	zCSPCS);
	voxel.Points[5] = XMFLOAT3(xCS,		yCSPCS,	zCSPCS);
	voxel.Points[6] = XMFLOAT3(xCSPCS,	yCSPCS,	zCSPCS);
	voxel.Points[7] = XMFLOAT3(xCSPCS,	yCS,	zCSPCS);
}