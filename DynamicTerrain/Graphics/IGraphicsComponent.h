#ifndef ENGINE_GRAPHICS_IGRAPHICSCOMPONENT_H
#define ENGINE_GRAPHICS_IGRAPHICSCOMPONENT_H

#include <Windows.h>

#include "..\Core\IGame.h"
#include "..\Core\MethodHelpers.h"
#include "IRenderer.h"

using Engine::Core::IGame;

namespace Engine 
{
namespace Graphics
{

class _declspec(dllexport) IGraphicsComponent {
public:
	virtual ~IGraphicsComponent() {}

	virtual RVAL Init(const HINSTANCE&) = 0;
	virtual int RunGameLoop(IGame* const &) = 0;
	virtual void StopGameLoop() = 0;

	virtual IRenderer* const GetRenderer() = 0;
	virtual HWND GetHWND() = 0;
};

}
}

#endif