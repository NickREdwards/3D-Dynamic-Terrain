#ifndef MOCKIDYNAMICTERRAINSYSTEM_H
#define MOCKIDYNAMICTERRAINSYSTEM_H

#include <gmock\gmock.h>

#include <Geometry\DynamicTerrainSystem.h>

using namespace Engine::Geometry;

class MockIDynamicTerrainSystem: public IDynamicTerrainSystem {
public:
	MOCK_METHOD1(Init, RVAL(ID3D11Device* const));
	MOCK_METHOD0(Update, void());
	MOCK_METHOD0(Reset, void());
	MOCK_METHOD1(RemoveVoxelsAroundPoint, void(XMFLOAT3&));
	MOCK_METHOD1(AddVoxelsAroundPoint, void(XMFLOAT3&));
	MOCK_METHOD1(DisplaceVoxelsAroundPoint, void(XMFLOAT3&));
	MOCK_METHOD0(AreAllChunksLoaded, bool());
	MOCK_METHOD0(DirtyHeightField, void());
	MOCK_METHOD2(GetHeightFieldForChunk, float**(int, int));
	MOCK_METHOD0(GetChunks, vector<DynamicTerrainChunk*>&());
};

#endif