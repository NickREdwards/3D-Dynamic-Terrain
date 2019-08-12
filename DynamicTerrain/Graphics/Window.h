#ifndef ENGINE_GRAPHICS_WINDOW_H
#define ENGINE_GRAPHICS_WINDOW_H

#include <Windows.h>

#include "Settings.h"
#include "GameLoop.h"

namespace Engine 
{
namespace Graphics
{

class _declspec(dllexport) Window {
public:
	Window();

	~Window() {
		_gameLoop = nullptr;
	}

	HRESULT Init(const HINSTANCE& hInstance, GameLoop* const gameLoop);

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT MsgProc(HWND, UINT, WPARAM, LPARAM);

	HWND GetHWND() { return _hWnd; }

private:
	HINSTANCE _hInstance;
	HWND _hWnd;

	GameLoop* _gameLoop;

};

}
}

#endif