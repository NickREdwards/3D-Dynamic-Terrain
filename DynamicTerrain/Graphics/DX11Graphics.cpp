#include "DX11Graphics.h"

using namespace Engine::Graphics;

RVAL DX11Graphics::Init(const HINSTANCE& hInstance) {
	if (FAILED(_window.Init(hInstance, &_gameLoop)))
		return RVAL::R_FAILED;

	return InitRenderer();
}

int DX11Graphics::RunGameLoop(IGame* const &gameRef) {
	return _gameLoop.Run(gameRef);
}

void DX11Graphics::StopGameLoop() {
	_gameLoop.Stop();
}

HWND DX11Graphics::GetHWND() {
	return _window.GetHWND();
}

RVAL DX11Graphics::InitRenderer() {
	_renderer = new DX11Renderer;
	return _renderer->Init(_window.GetHWND());
}