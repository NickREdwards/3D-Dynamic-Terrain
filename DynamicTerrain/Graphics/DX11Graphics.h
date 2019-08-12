#ifndef ENGINE_GRAPHICS_DX11GRAPHICS_H
#define ENGINE_GRAPHICS_DX11GRAPHICS_H

#include <memory>

#include "..\Core\MethodHelpers.h"
#include "IGraphicsComponent.h"
#include "Window.h"
#include "GameLoop.h"
#include "DX11Renderer.h"

using std::unique_ptr;

namespace Engine 
{
namespace Graphics
{

class _declspec(dllexport) DX11Graphics: public IGraphicsComponent {
public:
	DX11Graphics()
		:	_renderer(nullptr) {}

	~DX11Graphics() {
		delete _renderer;
	}

	RVAL Init(const HINSTANCE& hInstance) override;
	int RunGameLoop(IGame* const &gameRef) override;
	void StopGameLoop() override;

	IRenderer* const GetRenderer() override { return _renderer; }
	HWND GetHWND() override;

private:
	Window _window;
	GameLoop _gameLoop;
	IRenderer* _renderer;

	RVAL InitRenderer();
};

}
}

#endif