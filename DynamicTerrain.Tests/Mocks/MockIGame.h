#ifndef MOCKIGAME_H
#define MOCKIGAME_H

#include <gmock\gmock.h>
#include <Windows.h>
#include <Core\IGame.h>
#include <Core\MethodHelpers.h>

class MockIGame: public IGame {
public:
	MOCK_METHOD1(Init, RVAL(const HINSTANCE& hInstance));
	MOCK_METHOD0(Run, int());
	MOCK_METHOD1(Update, void(float deltaTime));
	MOCK_METHOD0(Render, void());
	MOCK_METHOD3(HandleMsg, void(const UINT& msg, const WPARAM& wParam, const LPARAM& lParam));
};

#endif