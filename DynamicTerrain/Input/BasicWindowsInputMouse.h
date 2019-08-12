#ifndef ENGINE_INPUT_BASICWINDOWSINPUTMOUSE_H
#define ENGINE_INPUT_BASICWINDOWSINPUTMOUSE_H

#include <Windows.h>

#include "..\Helpers\IntPoint.h"

using Engine::Helpers::IntPoint2;

namespace Engine
{
namespace Input
{

struct MouseState {
	bool Left;
	bool Right;
	MouseState() : Left(false), Right(false) {};
	MouseState(bool left, bool right) : Left(left), Right(right) {};

	void operator=(const MouseState& b) { Left = b.Left; Right = b.Right; }
};

class _declspec(dllexport) BasicWindowsInputMouse {
public:
	BasicWindowsInputMouse(HWND hWnd, int viewportWidth, int viewportHeight)
		:	_hWnd(nullptr),
			_viewportCentre(IntPoint2(viewportWidth / 2, viewportHeight / 2)),
			_currPos(IntPoint2::Zero()),
			_prevPos(IntPoint2::Zero()),
			_lockMouseToViewportCentre(false) {}

	void Update();
	void HandleMsg(const UINT& msg, const WPARAM& wParam, const LPARAM& lParam) {}

	void LockMouseToViewportCentre(bool hideMouse = true);

	IntPoint2 GetDeltaMouse();

	inline bool WasLMousePressed() { return !_prevState.Left && _currState.Left; }
	inline bool WasRMousePressed() { return !_prevState.Right && _currState.Right; }

	inline bool IsLMouseDown() { return _currState.Left; }
	inline bool IsRMouseDown() { return _currState.Right; }

private:
	HWND _hWnd;

	IntPoint2 _viewportCentre;
	IntPoint2 _currPos;
	IntPoint2 _prevPos;

	MouseState _prevState;
	MouseState _currState;

	bool _lockMouseToViewportCentre;

	void UpdateButtons();
	void UpdatePosition();
};

}
}

#endif