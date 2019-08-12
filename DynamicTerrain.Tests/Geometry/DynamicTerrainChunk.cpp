#include <gtest\gtest.h>

#include <Geometry\DynamicTerrainChunk.h>

using namespace Engine::Geometry;

TEST(DynamicTerrainChunk, SetGetPosition) {
	DynamicTerrainChunk chunk(nullptr);
	chunk.SetPosition(XMFLOAT3(1.2f, 3.4f, 5.6f));

	auto pos = chunk.GetPosition();
	ASSERT_EQ(1.2f, pos.x);
	ASSERT_EQ(3.4f, pos.y);
	ASSERT_EQ(5.6f, pos.z);
}

TEST(DynamicTerrainChunk, BuildWorldMatrix) {
	DynamicTerrainChunk chunk(nullptr);
	chunk.SetPosition(XMFLOAT3(1.2f, 3.4f, 5.6f));

	auto worldMatrix = chunk.GetWorldMatrix();

	ASSERT_EQ(1.2f, worldMatrix._41);
	ASSERT_EQ(3.4f, worldMatrix._42);
	ASSERT_EQ(5.6f, worldMatrix._43);
}

TEST(DynamicTerrainChunk, EqualsOperator) {
	auto one = new DynamicTerrainChunk(nullptr);
	auto two = new DynamicTerrainChunk(nullptr);
	auto three = new DynamicTerrainChunk(nullptr);

	one->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	two->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	three->SetPosition(XMFLOAT3(1.0f, 0.0f, 0.0f));
	
	ASSERT_TRUE(*one == *two);
	ASSERT_FALSE(*one == *three);

	delete one;
	delete two;
	delete three;
}