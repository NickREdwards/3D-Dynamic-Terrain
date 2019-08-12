#include <gtest\gtest.h>

#include <Graphics\IsometricCamera.h>
#include <Helpers\Math.h>

using namespace Engine::Graphics;
using namespace Engine::Helpers;
using testing::Test;

class TestIsometricCamera: public Test {
protected:
	virtual void SetUp() {
		_Camera = new IsometricCamera(800.0f, 600.0f);
		_Camera->SetPosition(XMFLOAT3(50.0f, 50.0f, 50.0f));
		_Camera->SetTarget(XMFLOAT3(0.0f, 0.0f, 0.0f));
	}

	virtual void TearDown() {
		delete _Camera;
	}

	IsometricCamera* _Camera;
};

TEST_F(TestIsometricCamera, TestViewMatrixCalculation) {
	auto viewMatrix = _Camera->GetViewMatrix();

	XMFLOAT4X4 viewMatrixF;
	XMStoreFloat4x4(&viewMatrixF, viewMatrix);

	ASSERT_FLOAT_EQ(-0.70710677f, viewMatrixF._12);
	ASSERT_FLOAT_EQ(-0.70710677f, viewMatrixF._13);

	ASSERT_FLOAT_EQ(0.70710677f, viewMatrixF._22);
	ASSERT_FLOAT_EQ(-0.70710677f, viewMatrixF._23);

	ASSERT_FLOAT_EQ(1.0f, viewMatrixF._31);

	ASSERT_FLOAT_EQ(848.5281f, viewMatrixF._43);
	ASSERT_FLOAT_EQ(1.0f, viewMatrixF._44);
}

TEST_F(TestIsometricCamera, TestProjMatrixCalculation) {
	auto projMatrix = _Camera->GetProjMatrix();

	XMFLOAT4X4 projMatrixF;
	XMStoreFloat4x4(&projMatrixF, projMatrix);

	ASSERT_FLOAT_EQ(1.29903805f, projMatrixF._11);

	ASSERT_FLOAT_EQ(1.73205078f, projMatrixF._22);

	ASSERT_FLOAT_EQ(1.00010002f, projMatrixF._33);
	ASSERT_FLOAT_EQ(1.0f, projMatrixF._34);

	ASSERT_FLOAT_EQ(-1.00010002f, projMatrixF._43);
}

TEST_F(TestIsometricCamera, TestMovement) {
	auto right = _Camera->GetRight();
	auto target = _Camera->GetTarget();

	auto movement = 2.0f;
	auto expectedNewTarget = AddXMFloat3ToXMFloat3(MultXMFloat3ByScalar(right, movement), target);

	_Camera->MoveRight(movement);

	auto newTarget = _Camera->GetTarget();

	EXPECT_FLOAT_EQ(expectedNewTarget.x, newTarget.x);
	EXPECT_FLOAT_EQ(expectedNewTarget.y, newTarget.y);
	EXPECT_FLOAT_EQ(expectedNewTarget.z, newTarget.z);
}

TEST_F(TestIsometricCamera, TestRotation) {
	auto position = _Camera->GetPosition();

	auto expectedNewPosition = XMFLOAT3(-249.6881f, 600.0f, 545.57843f);

	_Camera->Yaw(2.0f);
	_Camera->Update();

	auto newPosition = _Camera->GetPosition();

	EXPECT_FLOAT_EQ(expectedNewPosition.x, newPosition.x);
	EXPECT_FLOAT_EQ(expectedNewPosition.y, newPosition.y);
	EXPECT_FLOAT_EQ(expectedNewPosition.z, newPosition.z);
}

TEST_F(TestIsometricCamera, TestZooming) {
	auto position = _Camera->GetPosition();

	auto expectedNewPosition = XMFLOAT3(598.0f, 598.0f, 0.0f);

	_Camera->Zoom(2.0f);
	_Camera->Update();

	auto newPosition = _Camera->GetPosition();

	EXPECT_FLOAT_EQ(expectedNewPosition.x, newPosition.x);
	EXPECT_FLOAT_EQ(expectedNewPosition.y, newPosition.y);
	EXPECT_FLOAT_EQ(expectedNewPosition.z, newPosition.z);
}