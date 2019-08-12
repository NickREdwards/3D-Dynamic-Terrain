#include "Window.h"
#include "..\Helpers\Debug.h"

using namespace Engine::Graphics;
using namespace Engine::Helpers;

namespace {
	Window* gWindow = nullptr;
}

Window::Window()
	:	_hInstance(nullptr),
		_hWnd(nullptr),
		_gameLoop(nullptr) {
	gWindow = this;
}

HRESULT Window::Init(const HINSTANCE& hInstance, GameLoop* const gameLoop) {
	if (hInstance == nullptr) return E_FAIL;

	_hInstance = hInstance;
	_gameLoop = gameLoop;

	WNDCLASS windowClass = 
	{
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0, 0,
		hInstance,
		LoadIcon(nullptr, IDI_APPLICATION),
		LoadCursor(nullptr, IDC_ARROW),
		(HBRUSH)GetStockObject(NULL_BRUSH),
		nullptr,
		L"WndClassName"
	};

	if (!RegisterClass(&windowClass)) {
		MessageBox(nullptr, L"RegisterClass Failed.", L"Error in graphics component", MB_OK);
		return E_FAIL;
	} else {
		AdjustWindowRect(&Viewport, WS_MAXIMIZE, false);

		_hWnd = CreateWindow(L"WndClassName", L"Simulation of Dynamic and Interactive Terrain", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
							 CW_USEDEFAULT, Viewport.right, Viewport.bottom, nullptr, nullptr, hInstance, nullptr);

		if (!_hWnd || _hWnd == nullptr) {
			MessageBox(nullptr, L"CreateWindow Failed.", L"Error in graphics component", MB_OK);
			//Debug::ShowLastErrorInMessageBox();
			return E_FAIL;
		} else {
			ShowWindow(_hWnd, SW_SHOW);
			UpdateWindow(_hWnd);
			return S_OK;
		}
	}
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return gWindow->MsgProc(hWnd, msg, wParam, lParam);
}

LRESULT Window::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_MENUCHAR:
        return MAKELRESULT(0, MNC_CLOSE);

	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
		return 0;
	}

	try {
		if (_gameLoop && _gameLoop != nullptr)
			_gameLoop->HandleMsg(hWnd, msg, wParam, lParam);
	} catch (...) {}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}