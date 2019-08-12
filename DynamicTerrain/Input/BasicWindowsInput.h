#ifndef ENGINE_INPUT_BASICWINDOWSINPUT_H
#define ENGINE_INPUT_BASICWINDOWSINPUT_H

#include <map>

#include "IInputComponent.h"
#include "BasicWindowsInputMouse.h"
#include "BasicWindowsInputKeyboard.h"

using std::map;

namespace Engine
{
namespace Input
{

class _declspec(dllexport) BasicWindowsInput: public IInputComponent {
public:
	BasicWindowsInput(HWND hWnd, int viewportWidth, int viewportHeight)
		:	_mouse(hWnd, viewportWidth, viewportHeight) {}

	~BasicWindowsInput() {}

	void Update() override;
	void HandleMsg(const UINT& msg, const WPARAM& wParam, const LPARAM& lParam) override;

	void LockMouseToCentre(bool hide = true) override { _mouse.LockMouseToViewportCentre(hide); }

	inline IntPoint2 GetDeltaMouse() override { return _mouse.GetDeltaMouse(); }

	inline bool WasLMousePressed() override { return _mouse.WasLMousePressed(); }
	inline bool WasRMousePressed() override { return _mouse.WasRMousePressed(); }

	inline bool IsLMouseDown() override { return _mouse.IsLMouseDown(); }
	inline bool IsRMouseDown() override { return _mouse.IsRMouseDown(); }

	bool WasKeyPressed(int keyCode) override { return _keyboard.WasKeyPressed(keyCode); } 
	bool IsKeyDown(int keyCode) override { return _keyboard.IsKeyDown(keyCode); }

private:
	BasicWindowsInputMouse _mouse;
	BasicWindowsInputKeyboard _keyboard;
};

}
}

#endif