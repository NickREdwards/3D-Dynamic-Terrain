#include <gtest\gtest.h>

#include <Geometry\DynamicTerrainSettings.h>

using namespace Engine::Geometry;

TEST(VoxelRef, EqualityOperator) {
	VoxelRef one(nullptr, 1, 2, 3);
	VoxelRef two(nullptr, 4, 5, 6);
	VoxelRef three(nullptr, 1, 2, 3);

	ASSERT_FALSE(one == two);
	ASSERT_FALSE(one != three);
	ASSERT_TRUE(one != two);
	ASSERT_TRUE(one == three);
}

TEST(VoxelRef, LessThanOperator) {
	VoxelRef one(nullptr, 1, 2, 3);
	VoxelRef two(nullptr, 4, 5, 6);
	VoxelRef three(nullptr, 1, 2, 4);
	VoxelRef four (nullptr, 1, 1, 3);

	ASSERT_TRUE(one < two);
	ASSERT_FALSE(two < one);

	ASSERT_TRUE(one < three);
	ASSERT_FALSE(one < four);
}