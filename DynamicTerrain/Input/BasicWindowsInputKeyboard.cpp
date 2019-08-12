#include "BasicWindowsInputKeyboard.h"

using namespace Engine::Input;

void BasicWindowsInputKeyboard::HandleMsg(const UINT& msg, const WPARAM& wParam, const LPARAM& lParam) {
	switch (msg) {
	case WM_KEYDOWN: KeyDown(static_cast<int>(wParam)); break;
	case WM_KEYUP: KeyUp(static_cast<int>(wParam)); break;
	}
}

bool BasicWindowsInputKeyboard::IsKeyDown(int keyCode) {
	if (_keysPressed.find(keyCode) == _keysPressed.end()) return false;
	return _keysPressed[keyCode];
}

void BasicWindowsInputKeyboard::KeyDown(int keyCode) {
	if (_keysPressed.find(keyCode) == _keysPressed.end())
		_keysPressed.insert(std::make_pair(keyCode, true));
	else
		_keysPressed[keyCode] = true;
}

void BasicWindowsInputKeyboard::KeyUp(int keyCode) {
	if (_keysPressed.find(keyCode) == _keysPressed.end())
		_keysPressed.insert(std::make_pair(keyCode, false));
	else
		_keysPressed[keyCode] = false;
}