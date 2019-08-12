#ifndef ENGINE_GRAPHICS_GAMELOOP_H
#define ENGINE_GRAPHICS_GAMELOOP_H

#include <Windows.h>

#include "..\Core\IGame.h"

using Engine::Core::IGame;

namespace Engine 
{
namespace Graphics
{

class _declspec(dllexport) GameLoop {
public:
	GameLoop()
		:	_stopLoop(false) {}
	
	~GameLoop() {
		_gameRef = nullptr;
	}

	int Run(IGame* const &gameRef);
	void Stop();
	void HandleMsg(const HWND& hWnd, const UINT& msg, const WPARAM& wParam, const LPARAM& lParam);

private:
	IGame* _gameRef;
	bool _stopLoop;
};

}
}

#endif