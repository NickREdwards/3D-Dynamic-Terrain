#ifndef ENGINE_INPUT_IINPUTCOMPONENT_H
#define ENGINE_INPUT_IINPUTCOMPONENT_H

#include <Windows.h>

#include "..\Helpers\IntPoint.h"

using Engine::Helpers::IntPoint2;

namespace Engine
{
namespace Input
{

class _declspec(dllexport) IInputComponent {
public:
	virtual ~IInputComponent() {}

	virtual void Update() = 0;
	virtual void HandleMsg(const UINT&, const WPARAM&, const LPARAM&) = 0;

	virtual void LockMouseToCentre(bool) = 0;

	virtual IntPoint2 GetDeltaMouse() = 0;

	virtual bool WasLMousePressed() = 0;
	virtual bool WasRMousePressed() = 0;
	virtual bool WasKeyPressed(int) = 0;

	virtual bool IsLMouseDown() = 0;
	virtual bool IsRMouseDown() = 0;
	virtual bool IsKeyDown(int) = 0;
};

}
}

#endif