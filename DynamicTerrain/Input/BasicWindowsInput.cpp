#include "BasicWindowsInput.h"

using namespace Engine::Input;

void BasicWindowsInput::Update() {
	_mouse.Update();
	_keyboard.Update();
}

void BasicWindowsInput::HandleMsg(const UINT& msg, const WPARAM& wParam, const LPARAM& lParam) {
	_mouse.HandleMsg(msg, wParam, lParam);
	_keyboard.HandleMsg(msg, wParam, lParam);
}