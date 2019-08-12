#ifndef ENGINE_GEOMETRY_DYNAMICTERRAINSYSTEM_H
#define ENGINE_GEOMETRY_DYNAMICTERRAINSYSTEM_H

#include <D3D11.h>
#include <vector>
#include <string>
#include <fstream>
#include <deque>
#include <thread>
#include <mutex>
#include <noise\noise.h>

#include "DynamicTerrainChunk.h"
#include "DynamicTerrainManipulator.h"
#include "DynamicTerrainSettings.h"

using std::vector;
using std::deque;
using std::thread;
using std::mutex;
using std::string;
using std::ifstream;
using noise::module::Perlin;

using namespace Engine::Helpers;

namespace Engine
{
namespace Geometry
{

class _declspec(dllexport) IDynamicTerrainSystem {
public:
	virtual ~IDynamicTerrainSystem() {}

	virtual RVAL Init(ID3D11Device* const) = 0;

	virtual void Update() = 0;
	virtual void Reset() = 0;

	virtual void RemoveVoxelsAroundPoint(XMFLOAT3&) = 0;
	virtual void AddVoxelsAroundPoint(XMFLOAT3&) = 0;
	virtual void DisplaceVoxelsAroundPoint(XMFLOAT3&) = 0;

	virtual bool AreAllChunksLoaded() = 0;

	virtual void DirtyHeightField() = 0;

	virtual float** GetHeightFieldForChunk(int, int) = 0;
	virtual vector<DynamicTerrainChunk*>& GetChunks() = 0;
};

class _declspec(dllexport) DynamicTerrainSystem: public IDynamicTerrainSystem {
public:
	DynamicTerrainSystem(string settingsFile = "");
	~DynamicTerrainSystem();

	RVAL Init(ID3D11Device* const d3dDevice) override;
	
	void Update() override;
	void Reset() override;

	void RemoveVoxelsAroundPoint(XMFLOAT3& point) override;
	void AddVoxelsAroundPoint(XMFLOAT3& point) override;
	void DisplaceVoxelsAroundPoint(XMFLOAT3& point) override;

	void SmoothTerrain();

	bool AreAllChunksLoaded() { return _allChunksLoaded; }
	void ToggleUpdating(bool shouldUpdate) { _updateChunks = shouldUpdate; }

	void UpdateThreadProc();
	void MeshingThreadProc();

	void DirtyHeightField() { _heightFieldIsDirty = true; }

	float** GetHeightFieldForChunk(int chunkX, int chunkZ);
	vector<DynamicTerrainChunk*>& GetChunks() override { return _chunks; }

private:
	ID3D11Device* _d3dDevice;

	DynamicTerrainManipulator _chunkManipulator;

	float _heightField[TS.NumChunksX * TS.ChunkSizeX + 1]
					  [TS.NumChunksZ * TS.ChunkSizeZ + 1];

	vector<vector<DynamicTerrainChunk*>> _chunksToLoad;
	vector<DynamicTerrainChunk*> _chunks;
	deque<DynamicTerrainChunk*> _chunksToReMesh;
	deque<DynamicTerrainChunk*> _chunksToUpdate;

	vector<thread> _meshingThreads;
	vector<thread> _updateThreads;

	mutex _meshingAccess;
	mutex _updateAccess;
	mutex _generalAccess;

	bool _allChunksLoaded;
	bool _updateChunks;
	bool _heightFieldIsDirty;
	bool _threadsShouldEnd;
	bool _holdThreads;
	int _numThreadsRunning;

	Perlin _noiseModule;

	RVAL InitChunks(ID3D11Device* const d3dDevice);
	void LoadAllChunks();

	RVAL InitThreads();
	void EndAllThreads();

	RVAL InitHeightField();
	void RegenerateHeightField();
	void SmoothHeightField();

	void LoadSettingsFromFile(string fileName);

	void ReMeshChunk(DynamicTerrainChunk* const chunk);

	vector<DynamicTerrainChunk*> GetIntersectedChunks(const XMFLOAT3& position, int radius = 10);
};

}
}

#endif