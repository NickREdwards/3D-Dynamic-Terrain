#include <gtest\gtest.h>
#include <Helpers\Math.h>

using namespace Engine::Helpers;

TEST(Math, TestXMVectorToXMFloat3) {
	auto vec = XMLoadFloat3(&XMFLOAT3(1.2f, 3.4f, 5.6f));
	auto result = XMVectorToXMFloat3(vec);

	ASSERT_FLOAT_EQ(1.2f, result.x);
	ASSERT_FLOAT_EQ(3.4f, result.y);
	ASSERT_FLOAT_EQ(5.6f, result.z);
}

TEST(Math, TestXMMatrixToXMFloat4x4) {
	auto mat = DirectX::XMMatrixIdentity();
	auto result = XMMatrixToXMFloat4x4(mat);

	ASSERT_FLOAT_EQ(1.0f, result._11);
	ASSERT_FLOAT_EQ(1.0f, result._22);
	ASSERT_FLOAT_EQ(1.0f, result._33);
	ASSERT_FLOAT_EQ(1.0f, result._44);
}

TEST(Math, TestMultXMFloat3ByScalar) {
	XMFLOAT3 one(1.3f, 2.7f, 3.1f);

	auto result = MultXMFloat3ByScalar(one, 5.2f);

	ASSERT_FLOAT_EQ(6.76f,	result.x);
	ASSERT_FLOAT_EQ(14.04f,	result.y);
	ASSERT_FLOAT_EQ(16.12f,	result.z);
}

TEST(Math, TestAddXMFloat3ToXMFloat3) {
	XMFLOAT3 one(1.2f, 3.4f, 5.6f);
	XMFLOAT3 two(6.5f, 4.3f, 2.1f);

	auto result = AddXMFloat3ToXMFloat3(one, two);

	ASSERT_FLOAT_EQ(7.7f, result.x);
	ASSERT_FLOAT_EQ(7.7f, result.y);
	ASSERT_FLOAT_EQ(7.7f, result.z);
}

TEST(Math, TestAddXMFloat3ToXMFloat3Neg) {
	XMFLOAT3 one(1.2f, 3.4f, 5.6f);
	XMFLOAT3 two(6.5f, 4.3f, 2.1f);

	auto result = AddXMFloat3ToXMFloat3Neg(one, two);

	ASSERT_FLOAT_EQ(-5.3f,	result.x);
	ASSERT_FLOAT_EQ(-0.9f,	result.y);
	ASSERT_FLOAT_EQ(3.5f,	result.z);
}

TEST(Math, TestDotXMFloat4) {
	XMFLOAT4 one(1.2f, 3.4f, 5.6f, 7.8f);
	XMFLOAT4 two(8.7f, 6.5f, 4.3f, 2.1f);

	auto result = DotXMFloat4(one, two);

	ASSERT_FLOAT_EQ(73.0f, result);
}

TEST(Math, TestConvertNoiseToHeight) {
	ASSERT_FLOAT_EQ(1.0f,	ConvertNoiseToHeight(1.0f));
	ASSERT_FLOAT_EQ(0.75f,	ConvertNoiseToHeight(0.5f));
	ASSERT_FLOAT_EQ(0.5f,	ConvertNoiseToHeight(0.0f));
	ASSERT_FLOAT_EQ(0.25f,	ConvertNoiseToHeight(-0.5f));
	ASSERT_FLOAT_EQ(0.0f,	ConvertNoiseToHeight(-1.0f));
}

TEST(Math, TestClamp) {
	ASSERT_FLOAT_EQ(1.0f, Clamp(2.5f, 0.0f, 1.0f));
	ASSERT_FLOAT_EQ(0.0f, Clamp(-2.5f, 0.0f, 1.0f));
	ASSERT_FLOAT_EQ(2.5f, Clamp(2.5f, 0.0f, 3.0f));
}