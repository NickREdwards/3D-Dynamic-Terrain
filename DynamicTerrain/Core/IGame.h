#ifndef ENGINE_CORE_IGAME_H
#define ENGINE_CORE_IGAME_H

#include <Windows.h>

#include "MethodHelpers.h"

namespace Engine
{
namespace Core
{

class _declspec(dllexport) IGame {
public:
	virtual ~IGame() {}

	virtual RVAL Init(const HINSTANCE&) = 0;
	virtual int Run() = 0;

	virtual void Update(float deltaTime = 1.0f) = 0;
	virtual void Render() = 0;

	virtual void HandleMsg(const UINT&, const WPARAM&, const LPARAM&) = 0;
};

}
}

#endif