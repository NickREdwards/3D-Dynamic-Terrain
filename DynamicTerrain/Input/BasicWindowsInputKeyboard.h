#ifndef ENGINE_INPUT_BASICWINDOWSINPUTKEYBOARD_H
#define ENGINE_INPUT_BASICWINDOWSINPUTKEYBOARD_H

#include <map>
#include <Windows.h>

using std::map;

namespace Engine
{
namespace Input
{

class _declspec(dllexport) BasicWindowsInputKeyboard {
public:
	void Update() {}
	void HandleMsg(const UINT& msg, const WPARAM& wParam, const LPARAM& lParam);

	bool WasKeyPressed(int keyCode) { return false; }
	bool IsKeyDown(int keyCode);

private:
	map<int, bool> _keysPressed;

	void KeyDown(int keyCode);
	void KeyUp(int keyCode);
};

}
}

#endif