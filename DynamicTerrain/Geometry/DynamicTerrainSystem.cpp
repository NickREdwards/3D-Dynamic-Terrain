#include "DynamicTerrainSystem.h"

using namespace Engine::Geometry;

TerrainSettings TS;

DynamicTerrainSystem::DynamicTerrainSystem(string settingsFile)
		:	_d3dDevice(nullptr),
			_allChunksLoaded(false),
			_updateChunks(true),
			_heightFieldIsDirty(false),
			_threadsShouldEnd(false),
			_holdThreads(false),
			_numThreadsRunning(0) {

	if (settingsFile.size() > 0) {
		LoadSettingsFromFile(settingsFile);
	}

	_noiseModule.SetSeed(TS.NoiseSeed);
	_noiseModule.SetFrequency(TS.NoiseFrequency);
	_noiseModule.SetPersistence(TS.NoisePersistence);
	_noiseModule.SetOctaveCount(TS.NoiseOctaves);
}

DynamicTerrainSystem::~DynamicTerrainSystem() {
	// Signals threads to end and waits for them
	EndAllThreads();

	_chunksToReMesh.clear();

	for (auto& c : _chunks)
		delete c;
	_chunks.clear();

	_d3dDevice = nullptr;
}

RVAL DynamicTerrainSystem::Init(ID3D11Device* const d3dDevice) {
	_d3dDevice = d3dDevice;

	if (RFAILURE(InitHeightField()))
		return RVAL::R_FAILED;

	if (RFAILURE(InitChunks(d3dDevice)))
		return RVAL::R_FAILED;

	if (RFAILURE(InitThreads()))
		return RVAL::R_FAILED;

	return RVAL::R_OK;
}

void DynamicTerrainSystem::Update() {
	// Updates are now performed on a separate thread - see UpdateThreadProc
}

void DynamicTerrainSystem::Reset() {
	if (_holdThreads) return;
	_holdThreads = true;

	InitHeightField();

	for (auto& c : _chunks)
		c->Reset();

	_holdThreads = false;
}

void DynamicTerrainSystem::RemoveVoxelsAroundPoint(XMFLOAT3& point) {
	auto chunksAffected = GetIntersectedChunks(point);

	for (auto& c : chunksAffected) {
		_chunkManipulator.RemoveVoxelsAroundPoint(c, point, 10);
	}
}

void DynamicTerrainSystem::AddVoxelsAroundPoint(XMFLOAT3& point) {
	auto chunksAffected = GetIntersectedChunks(point);

	for (auto& c : chunksAffected) {
		_chunkManipulator.AddVoxelsAroundPoint(c, point, 10);
	}
}

void DynamicTerrainSystem::DisplaceVoxelsAroundPoint(XMFLOAT3& point) {
	auto chunksAffected = GetIntersectedChunks(point, 20);

	for (auto& c : chunksAffected) {
		_chunkManipulator.DisplaceVoxelsAroundPoint(c, point, 10);
	}
}

void DynamicTerrainSystem::SmoothTerrain() {
	if (_holdThreads) return;
	_holdThreads = true;

	if (_heightFieldIsDirty)
		RegenerateHeightField();

	for (auto i = 0; i < _chunks.size(); i++) {
		_chunks[i]->_smoothTerrain = true;
		_chunks[i]->DirtyBuffer();
	}

	_holdThreads = false;
}

/*
	Update procedure for chunks. Runs on multiple
	separate threads.
*/
void DynamicTerrainSystem::UpdateThreadProc() {
	{
		std::lock_guard<mutex> lock(_generalAccess);
		_numThreadsRunning++;
	}

	DynamicTerrainChunk* chunk = nullptr;

	while (!_threadsShouldEnd) {
		if (_holdThreads || !_updateChunks) continue;

		chunk = nullptr;

		// Get the next chunk to update in line
		{
			std::lock_guard<mutex> lock(_updateAccess);
			if (_chunksToUpdate.size() > 0) {
				chunk = _chunksToUpdate.front();
				_chunksToUpdate.pop_front();
			}
		}

		// If we got one - update it
		if (chunk != nullptr) {
			chunk->Update();

			// Does it need re-meshing?
			if (chunk->_bufferIsDirty && 
				!chunk->_awaitingReMesh && 
				!chunk->_backupMeshRebuilding)
				ReMeshChunk(chunk);

			{
				std::lock_guard<mutex> lock(_updateAccess);
				_chunksToUpdate.push_back(chunk);
			}
		} else {
			//std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}

	}

	{
		std::lock_guard<mutex> lock(_generalAccess);
		_numThreadsRunning--;
	}
}

/*
	Meshing procedure for chunks. Runs on multiple
	separate threads.
*/
void DynamicTerrainSystem::MeshingThreadProc() {
	{
		std::lock_guard<mutex> lock(_generalAccess);
		_numThreadsRunning++;
	}

	DynamicTerrainChunk* chunk = nullptr;

	while (!_threadsShouldEnd) {
		if (_holdThreads) continue;

		chunk = nullptr;

		// If there are chunks waiting to be re-meshed, pick one up
		{
			std::lock_guard<mutex> lock(_meshingAccess);
			if (_chunksToReMesh.size() > 0) {
				chunk = _chunksToReMesh.front();
				_chunksToReMesh.pop_front();
			}
		}

		if (chunk != nullptr) {
			if (RFAILURE(chunk->RebuildMesh(_d3dDevice))) {
				MessageBox(nullptr, L"Unable to build mesh", L"Error", MB_OK); // Is MessageBox from a thread bad?
			}
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	{
		std::lock_guard<mutex> lock(_generalAccess);
		_numThreadsRunning--;
	}
}

float** DynamicTerrainSystem::GetHeightFieldForChunk(int chunkX, int chunkZ) {
	// Returns the X*Z 2D height-field for a specific chunk.
	// Height-field contains values for all chunks, so need to know the correct index.

	float** heightField = new float*[TS.ChunkSizeX+1];
	for (auto i = 0; i < TS.ChunkSizeX+1; i++)
		heightField[i] = new float[TS.ChunkSizeZ+1];

	int startX = chunkX * TS.ChunkSizeX;
	int startZ = chunkZ * TS.ChunkSizeZ;

	int x, z, i, k;
	
	for (x = startX, i = 0; x < startX + TS.ChunkSizeX + 1; x++, i++)
	for (z = startZ, k = 0; z < startZ + TS.ChunkSizeZ + 1; z++, k++) {
		heightField[i][k] = _heightField[x][z];
	}

	return heightField;
}

/*
	Initialise all chunks and put them on the queue
	ready to be meshed.
*/
RVAL DynamicTerrainSystem::InitChunks(ID3D11Device* const d3dDevice) {
	if (d3dDevice == nullptr) return RVAL::R_FAILED;
	
	/*
		All chunks need to be created before being initialised,
		so that neighbours can be set up.
	*/
	_chunksToLoad.resize(TS.NumChunksX);
	for (auto x = 0; x < TS.NumChunksX; x++) {
		_chunksToLoad[x].reserve(TS.NumChunksZ);
		for (auto z = 0; z < TS.NumChunksZ; z++) {
			_chunksToLoad[x].push_back(new DynamicTerrainChunk(this));
		}
	}

	_chunks.reserve(TS.NumChunksX * TS.NumChunksZ);

	DynamicTerrainChunk* chunk = nullptr;


	for (auto x = TS.NumChunksX-1; x >= 0; x--)
	for (auto z = TS.NumChunksZ-1; z >= 0; z--) {
		chunk = _chunksToLoad[x][z];

		/* Set up neighbours */
		if (z < TS.NumChunksZ-1) chunk->_chunkUpdater.SetNorthChunk(_chunksToLoad[x][z+1]);
		if (x < TS.NumChunksX-1) chunk->_chunkUpdater.SetEastChunk (_chunksToLoad[x+1][z]);
		if (z > 0)				 chunk->_chunkUpdater.SetSouthChunk(_chunksToLoad[x][z-1]);
		if (x > 0)				 chunk->_chunkUpdater.SetWestChunk (_chunksToLoad[x-1][z]);

		chunk->SetPosition(XMFLOAT3(TS.ChunkDimensions.x*x, 0.0f, TS.ChunkDimensions.z*z));
		chunk->SetChunkIndex(x, z);
	}

	/* Chunks are initially meshed on a separate thread */
	thread loadChunksThread(&DynamicTerrainSystem::LoadAllChunks, this);
	loadChunksThread.detach();

	return RVAL::R_OK;
}

/*
	Method which initialises all chunk meshes. Runs on a separate
	thread and ends on its own when done.
*/
void DynamicTerrainSystem::LoadAllChunks() {
	{
		std::lock_guard<mutex> lock(_generalAccess);
		_numThreadsRunning++;
	}

	DynamicTerrainChunk* chunk = nullptr;

	for (auto x = 0; x < _chunksToLoad.size(); x++) {
		for (auto z = 0; z < _chunksToLoad[x].size(); z++) {
			if (_threadsShouldEnd) break;

			chunk = _chunksToLoad[x][z];

			if (RSUCCESS(chunk->Init()) &&
				RSUCCESS(chunk->InitMesh(_d3dDevice))) {
				_chunks.push_back(chunk);

				{
					std::lock_guard<mutex> lock(_updateAccess);
					_chunksToUpdate.push_back(chunk);
				}
			}
		}
		_chunksToLoad[x].clear();
	}
	_chunksToLoad.clear();

	_allChunksLoaded = true;

	{
		std::lock_guard<mutex> lock(_generalAccess);
		_numThreadsRunning--;
	}
}

RVAL DynamicTerrainSystem::InitThreads() {
	for (auto i = 0; i < TS.NumMeshingThreads; i++) {
		_meshingThreads.push_back(thread(&DynamicTerrainSystem::MeshingThreadProc, this));
		_meshingThreads[i].detach();
	}

	for (auto i = 0; i < TS.NumUpdateThreads; i++) {
		_updateThreads.push_back(thread(&DynamicTerrainSystem::UpdateThreadProc, this));
		_updateThreads[i].detach();
	}

	return RVAL::R_OK;
}

void DynamicTerrainSystem::EndAllThreads() {
	{
		std::lock_guard<mutex> lock(_generalAccess);
		_threadsShouldEnd = true;
	}

	bool allEnded = false;

	while (!allEnded) {
		{
			std::lock_guard<mutex> lock(_generalAccess);
			if (_numThreadsRunning <= 0)
				allEnded = true;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

RVAL DynamicTerrainSystem::InitHeightField() {
	// Samples noise to create a height-map for the entire terrain (all chunks)

	float sampleX, sampleZ;

	for (auto x = 0; x < TS.NumChunksX * TS.ChunkSizeX + 1; x++)
	for (auto z = 0; z < TS.NumChunksZ * TS.ChunkSizeZ + 1; z++) {
		sampleX = x * TS.CubeSize / 1000.0f;
		sampleZ = z * TS.CubeSize / 1000.0f;
		_heightField[x][z] = Helpers::ConvertNoiseToHeight(_noiseModule.GetValue(sampleX, sampleZ, 0.0f))/* - 0.25f*/;
	}

	return RVAL::R_OK;
}

void DynamicTerrainSystem::RegenerateHeightField() {
	float indexHeights[TS.ChunkSizeX][TS.ChunkSizeZ];

	int startX, startZ;
	auto indexBias = 1.0f / (float)TS.ChunkSizeY;
	auto doubleIndexBias = indexBias * 2;

	float originalHeight, originalIndex, indexDiff, indexBiasDiff;

	for (auto i = 0; i < _chunks.size(); i++) {
		{
			for (auto x = 0; x < TS.ChunkSizeX; x++)
			for (auto z = 0; z < TS.ChunkSizeZ; z++) {
				indexHeights[x][z] = DTHelpers::GetHeighestVoxelIndexAtXZIndex(_chunks[i], x, z);
			}
		}

		// Start of index into the height-field for this chunk
		startX = _chunks[i]->_chunkIndexX * TS.ChunkSizeX;
		startZ = _chunks[i]->_chunkIndexZ * TS.ChunkSizeZ;

		for (auto x = 0; x < TS.ChunkSizeX; x++)
		for (auto z = 0; z < TS.ChunkSizeZ; z++) {
			/*
				Get original height value, convert to original index
				Find the difference between the original index and the new
				Convert this new index into a height value, and subtract
				it from the original height value
			*/

			originalHeight = _heightField[startX + x][startZ + z];
			originalIndex = floorf(originalHeight / indexBias);

			indexDiff = originalIndex - indexHeights[x][z];
			indexBiasDiff = indexDiff * indexBias;

			_heightField[startX + x][startZ + z] = originalHeight - indexBiasDiff;
			if (_heightField[startX + x][startZ + z] < doubleIndexBias)
				_heightField[startX + x][startZ + z] -= doubleIndexBias;
		}
	}

	// 3 passes of linear smoothing
	for (auto i = 0; i < 3; i++)
		SmoothHeightField();
}

void DynamicTerrainSystem::SmoothHeightField() {
	// Average left-to-right
	for (auto x = 1; x < TS.NumChunksX * TS.ChunkSizeX-1; x++)
	for (auto z = 0; z < TS.NumChunksZ * TS.ChunkSizeZ; z++) {
		_heightField[x][z] = (_heightField[x-1][z] + _heightField[x][z] + _heightField[x+1][z]) / 3;
	}

	// Average top-to-bottom
	for (auto x = 0; x < TS.NumChunksX * TS.ChunkSizeX; x++)
	for (auto z = 1; z < TS.NumChunksZ * TS.ChunkSizeZ-1; z++) {
		_heightField[x][z] = (_heightField[x][z-1] + _heightField[x][z] + _heightField[x][z+1]) / 3;
	}
}

void DynamicTerrainSystem::LoadSettingsFromFile(string fileName) {
	ifstream fIn(fileName);

	if (!fIn) return; // Just use defaults

	string str;

	while (fIn) {
		fIn >> str;

		if (str == "#")
			getline(fIn, str); // Ignore the rest
		else if (str == "CubeSize")
			fIn >> TS.CubeSize;
		else if (str == "NumMeshingThreads")
			fIn >> TS.NumMeshingThreads;
		else if (str == "NumUpdateThreads")
			fIn >> TS.NumUpdateThreads;
		else if (str == "NoiseSeed")
			fIn >> TS.NoiseSeed;
		else if (str == "NoiseOctaves")
			fIn >> TS.NoiseOctaves;
		else if (str == "NoiseFrequency")
			fIn >> TS.NoiseFrequency;
		else if (str == "NoisePersistence")
			fIn >> TS.NoisePersistence;
		else if (str == "IsoLevel")
			fIn >> TS.IsoLevel;
	}
}

void DynamicTerrainSystem::ReMeshChunk(DynamicTerrainChunk* const chunk) {
	std::lock_guard<mutex> lock(_meshingAccess);
	chunk->_awaitingReMesh = true;
	_chunksToReMesh.push_back(chunk);
}

vector<DynamicTerrainChunk*> DynamicTerrainSystem::GetIntersectedChunks(const XMFLOAT3& position, int radius) {
	vector<DynamicTerrainChunk*> chunks;

	// AABB->Sphere intersection test. Sphere works well, however might be better to use AABB->AABB
	for (auto& c : _chunks) {
		auto aabb = c->GetAABB();
		if (aabb.IntersectedBy(position, radius * TS.CubeSize))
			chunks.push_back(c);
	}

	return chunks;
}