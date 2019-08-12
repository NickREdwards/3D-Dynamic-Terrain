#include <gtest\gtest.h>
#include <Helpers\AABB.h>

using namespace Engine::Helpers;

TEST(AABB, TestAABBConstructor) {
	AABB aabb;

	ASSERT_EQ(0.0f, aabb.Left);
	ASSERT_EQ(0.0f, aabb.Right);
	ASSERT_EQ(0.0f, aabb.Top);
	ASSERT_EQ(0.0f, aabb.Bottom);
	ASSERT_EQ(0.0f, aabb.Front);
	ASSERT_EQ(0.0f, aabb.Back);

	aabb = AABB(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);

	ASSERT_EQ(1.0f, aabb.Left);
	ASSERT_EQ(2.0f, aabb.Right);
	ASSERT_EQ(3.0f, aabb.Top);
	ASSERT_EQ(4.0f, aabb.Bottom);
	ASSERT_EQ(5.0f, aabb.Front);
	ASSERT_EQ(6.0f, aabb.Back);
}

TEST(AABB, TestAABBSphereIntersection) {
	AABB aabb(0.0f, 10.0f, 10.0f, 0.0f, 0.0f, 10.0f);

	ASSERT_TRUE(aabb.IntersectedBy(XMFLOAT3(2.5f, 2.5f, 2.5f), 2.0f));
	ASSERT_FALSE(aabb.IntersectedBy(XMFLOAT3(25.0f, 25.0f, 25.0f), 2.0f));
	ASSERT_TRUE(aabb.IntersectedBy(XMFLOAT3(25.0f, 25.0f, 25.0f), 40.0f));
}