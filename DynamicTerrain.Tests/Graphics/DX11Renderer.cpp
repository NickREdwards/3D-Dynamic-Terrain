#include <gtest\gtest.h>
#include <gmock\gmock.h>
#include <DirectXMath.h>

#include <Graphics\DX11Renderer.h>
#include <Core\MethodHelpers.h>

#include "..\Mocks\MockIEffect.h"
#include "..\Mocks\MockID3D11DeviceContext.h"
#include "..\Mocks\MockID3D11Buffer.h"
#include "..\Mocks\MockID3D11InputLayout.h"
#include "..\Mocks\MockIMesh.h"

using namespace Engine::Graphics;
using namespace Engine::Geometry;
using Engine::Core::RVAL;
using testing::Test;
using testing::Return;
using testing::DefaultValue;
using testing::_;

class TestDX11Renderer: public Test {
protected:
	virtual void SetUp() {
		_Renderer = new DX11Renderer;
		_Renderer->_camera = new IsometricCamera(800.0f, 600.0f);
		_Renderer->_d3dDeviceContext = &_MockDeviceContext;
	}

	virtual void TearDown() {
		_Renderer->_d3dDeviceContext = nullptr;
		delete _Renderer;
	}

	DX11Renderer* _Renderer;
	MockID3D11DeviceContext _MockDeviceContext;
};

TEST_F(TestDX11Renderer, TestInitialisation) {
	ASSERT_TRUE(RFAILURE(_Renderer->Init(nullptr)));
}

TEST_F(TestDX11Renderer, TestDrawingAMesh) {
	auto worldMatrix = XMMatrixToXMFloat4x4(DirectX::XMMatrixIdentity());

	ASSERT_TRUE(RFAILURE(_Renderer->DrawMesh(nullptr, worldMatrix)));

	MockIMesh mesh;
	ASSERT_TRUE(RFAILURE(_Renderer->DrawMesh(&mesh, worldMatrix)));

	auto mockIEffect = new MockIEffect;
	ON_CALL(*mockIEffect, SetEyePosW(_)).WillByDefault(Return(RVAL::R_OK));
	ON_CALL(*mockIEffect, SetWorld(_)).WillByDefault(Return(RVAL::R_OK));
	ON_CALL(*mockIEffect, SetWorldViewProj(_)).WillByDefault(Return(RVAL::R_OK));
	ON_CALL(*mockIEffect, ApplyForPass(_, _)).WillByDefault(Return(RVAL::R_OK));

	if (RFAILURE(_Renderer->AddEffect("TestEffect", mockIEffect)))
		GTEST_FATAL_FAILURE_("Unable to add effect");

	_Renderer->BeginEffect("TestEffect");

	MockID3D11Buffer vBuffer;
	MockID3D11Buffer iBuffer;

	ON_CALL(mesh, GetVertexBuffer()).WillByDefault(Return(&vBuffer));
	ON_CALL(mesh, GetIndexBuffer()).WillByDefault(Return(&iBuffer));
	ON_CALL(mesh, GetVertexStride()).WillByDefault(Return(sizeof(Vertex)));
	ON_CALL(mesh, GetNumIndices()).WillByDefault(Return(0));

	EXPECT_CALL(_MockDeviceContext, IASetVertexBuffers(_, _, _, _, _)).Times(1);
	EXPECT_CALL(_MockDeviceContext, IASetIndexBuffer(_, _, _)).Times(1);
	EXPECT_CALL(_MockDeviceContext, DrawIndexed(_, _, _)).Times(1);

	ASSERT_TRUE(RSUCCESS(_Renderer->DrawMesh(&mesh, worldMatrix)));

	_Renderer->EndEffect();
}