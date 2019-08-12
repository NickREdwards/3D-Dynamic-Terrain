#include "GameLoop.h"
#include <time.h>

using namespace Engine::Graphics;

int GameLoop::Run(IGame* const &gameRef) {
	_gameRef = gameRef;

	MSG msg = {nullptr};

	auto lastTime = GetCurrentTime();
	while (!_stopLoop) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			auto currTime = GetCurrentTime();
			auto elapsed = currTime - lastTime;

			gameRef->Update(static_cast<float>(elapsed));
			gameRef->Render();

			lastTime = currTime;
		}

		_stopLoop |= msg.message == WM_QUIT || msg.message == WM_DESTROY || msg.message == WM_CLOSE;
	}

	return (int)msg.wParam;
}

void GameLoop::Stop() {
	_stopLoop = true;
}

void GameLoop::HandleMsg(const HWND& hWnd, const UINT& msg, const WPARAM& wParam, const LPARAM& lParam) {
	if (_gameRef && _gameRef != nullptr)
		_gameRef->HandleMsg(msg, wParam, lParam);
}