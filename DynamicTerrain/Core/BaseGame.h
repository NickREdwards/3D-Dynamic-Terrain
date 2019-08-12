#ifndef ENGINE_CORE_BASEGAME_H
#define ENGINE_CORE_BASEGAME_H

#define FWDCL(ns, c) namespace Engine { namespace ns { class c; } }

#include <DirectXMath.h>
#include <vector>

#include "IGame.h"
#include "..\Geometry\WaterPlane.h"
#include "..\Geometry\DynamicTerrainSystem.h"
#include "..\Helpers\AABB.h"

using std::vector;
using DirectX::XMFLOAT3;

FWDCL(Graphics, ICamera);
FWDCL(Graphics, IEffect);
FWDCL(Graphics, IRenderer);
FWDCL(Graphics, IGraphicsComponent);
FWDCL(Input,    IInputComponent);

using namespace Engine::Geometry;
using namespace Engine::Graphics;
using namespace Engine::Input;

namespace Engine
{
namespace Core
{

class _declspec(dllexport) BaseGame: public IGame {
public:
	BaseGame();
	virtual ~BaseGame();

	virtual RVAL Init(const HINSTANCE& hInstance);
	virtual int Run();

	virtual void Update(float deltaTime = 1.0f);
	virtual void Render();

	virtual void HandleMsg(const UINT& msg, const WPARAM& wParam, const LPARAM& lParam);

protected:
	virtual RVAL InitEffects();
	virtual RVAL InitObjects();

private:
	IGraphicsComponent* _graphics;
	IInputComponent* _input;
	IRenderer* _renderer;
	ICamera* _camera;

	XMFLOAT3 _playerPos;
	AABB _playerBounds;

	WaterPlane* _waterPlane;
	DynamicTerrainSystem _terrain;

	void Reset();

	void UpdateTerrain();
	void UpdateCamera(float deltaTime);
	void UpdatePlayer(float deltaTime);

	void RenderWater();
	void RenderTerrain();
	void RenderObject(IObject3D* const obj);

	RVAL SetEffectVariablesPerFrame(IEffect* const effect);
};

}
}

#endif