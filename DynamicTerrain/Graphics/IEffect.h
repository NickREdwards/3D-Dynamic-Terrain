#ifndef ENGINE_GRAPHICS_IEFFECT_H
#define ENGINE_GRAPHICS_IEFFECT_H

#include <Windows.h>
#include <D3D11.h>
#include <DirectXMath.h>

#include "EffectStructures.h"
#include "..\Core\MethodHelpers.h"

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4X4;
using Engine::Core::RVAL;

namespace Engine 
{
namespace Graphics
{

class _declspec(dllexport) IEffect {
public:
	virtual ~IEffect() {}

	virtual RVAL BuildFromFXFile(LPCWSTR, ID3D11Device* const) = 0;
	virtual RVAL BuildInputLayout(ID3D11Device* const) = 0;
	virtual RVAL LoadFromFXOFile(LPCWSTR, ID3D11Device* const) = 0;

	virtual RVAL ApplyForPass(int, ID3D11DeviceContext* const) = 0;

	virtual RVAL SetEyePosW(const XMFLOAT3&) = 0;
	virtual RVAL SetPlayerPosW(const XMFLOAT3&) = 0;
	virtual RVAL SetLookVec(const XMFLOAT3&) = 0;

	virtual RVAL SetDiffuseMap(ID3D11ShaderResourceView* const) = 0;
	virtual RVAL SetNormalMap(ID3D11ShaderResourceView* const) = 0;

	virtual RVAL SetWorld(const XMFLOAT4X4&) = 0;
	virtual RVAL SetWorldInvTran(const XMFLOAT4X4&) = 0;
	virtual RVAL SetWorldViewProj(const XMFLOAT4X4&) = 0;

	virtual ID3D11InputLayout* const GetInputLayout() = 0;
	virtual UINT GetNumPasses() = 0;
};

}
}

#endif