#ifndef ENGINE_GRAPHICS_IRENDERER_H
#define ENGINE_GRAPHICS_IRENDERER_H

#include <Windows.h>
#include <D3D11.h>
#include <string>
#include <DirectXMath.h>

#include "..\Core\MethodHelpers.h"
#include "..\Geometry\Mesh.h"

using namespace Engine::Geometry;

using DirectX::XMFLOAT4;
using DirectX::XMVECTOR;
using DirectX::XMFLOAT4X4;
using std::string;
using Engine::Core::RVAL;

namespace Engine 
{
namespace Graphics 
{

class ICamera;
class IEffect;

class _declspec(dllexport) IRenderer {
public:
	virtual ~IRenderer() {}

	virtual RVAL Init(HWND) = 0;

	virtual RVAL AddEffect(string, LPCWSTR) = 0;
	virtual RVAL AddEffect(string, IEffect*) = 0;

	virtual void ClearScreen(XMFLOAT4 color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)) = 0;
	virtual void Present() = 0;

	virtual RVAL Begin2D() = 0;
	virtual void End2D() = 0;

	virtual RVAL Begin3D() = 0;
	virtual void End3D() = 0;

	virtual RVAL BeginEffect(string) = 0;
	virtual void EndEffect() = 0;

	virtual RVAL DrawMesh(IMesh* const, const XMFLOAT4X4&) = 0;

	virtual ID3D11Device* const GetDevice() = 0;
	virtual ICamera* const GetCamera() = 0;
	virtual IEffect* const GetCurrentEffect() = 0;
};
			
}
}

#endif