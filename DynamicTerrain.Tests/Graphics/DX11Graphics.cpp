#include <gtest\gtest.h>
#include <gmock\gmock.h>
#include <Windows.h>

#include <Core\MethodHelpers.h>
#include <Graphics\DX11Graphics.h>

#include "..\Shared.h"
#include "..\Mocks\MockIGame.h"
#include "..\Mocks\MockIEffect.h"
#include "..\Mocks\MockID3D11Device.h"

using namespace Engine::Graphics;
using Engine::Core::RVAL;
using testing::Invoke;
using testing::_;
using testing::Test;

class TestDX11Graphics: public Test {
public:
	void StopGameLoop() {
		_Graphics->StopGameLoop();
	}

protected:
	virtual void SetUp() {
		_Graphics = new DX11Graphics;
		_MockIGame = new MockIGame;
	}

	virtual void TearDown() {
		StopGameLoop();

		delete _MockIGame;
		delete _Graphics;
	}

	DX11Graphics* _Graphics;
	MockIGame* _MockIGame;
};

// Tests the relationship between a game and the graphics component
// by telling the graphics to run the game loop, and asserting that
// the update and render methods of the game get called.
TEST_F(TestDX11Graphics, TestRunGameLoop) {
	EXPECT_CALL(*_MockIGame, Update(_)).Times(1);
	EXPECT_CALL(*_MockIGame, Render()).Times(1);

	ON_CALL(*_MockIGame, Render())
		.WillByDefault(Invoke(this, &TestDX11Graphics::StopGameLoop));

	_Graphics->RunGameLoop(_MockIGame);
}