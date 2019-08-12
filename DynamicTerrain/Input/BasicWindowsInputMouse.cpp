#include "BasicWindowsInputMouse.h"

using namespace Engine::Input;

void BasicWindowsInputMouse::Update() {
	UpdateButtons();
	UpdatePosition();
}

void BasicWindowsInputMouse::LockMouseToViewportCentre(bool hideMouse) {
	SetCursorPos(_viewportCentre.X, _viewportCentre.Y);
	_currPos = _viewportCentre;
	_lockMouseToViewportCentre = true;

	if (hideMouse)
		ShowCursor(false);
	else
		ShowCursor(true);
}

IntPoint2 BasicWindowsInputMouse::GetDeltaMouse() {
	if (_lockMouseToViewportCentre)
		return _prevPos - _viewportCentre;

	return _prevPos - _currPos;
}

void BasicWindowsInputMouse::UpdateButtons() {
	_prevState = _currState;

	if (GetKeyState(VK_LBUTTON) < 0) _currState.Left = true;
	else _currState.Left = false;

	if (GetKeyState(VK_RBUTTON) < 0) _currState.Right = true;
	else _currState.Right = false;
}

void BasicWindowsInputMouse::UpdatePosition() {
	_prevPos = _currPos;

	POINT newMousePos;
	GetCursorPos(&newMousePos);

	_currPos = newMousePos;

	if (_lockMouseToViewportCentre)
		SetCursorPos(_viewportCentre.X, _viewportCentre.Y);
}