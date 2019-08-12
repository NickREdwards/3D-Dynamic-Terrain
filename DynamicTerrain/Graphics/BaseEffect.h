#ifndef ENGINE_GRAPHICS_BASEEFFECT_H
#define ENGINE_GRAPHICS_BASEEFFECT_H

#include <Windows.h>
#include <D3D11.h>
#include <D3Dcompiler.h>
#include <D3DX11Effect.h>
#include <exception>
#include <DirectXMath.h>

#include "EffectStructures.h"
#include "IEffect.h"

using DirectX::XMFLOAT3;
using DirectX::XMVECTOR;
using DirectX::XMFLOAT4X4;
using DirectX::XMMATRIX;

namespace Engine 
{
namespace Graphics
{

class _declspec(dllexport) BaseEffect: public IEffect {
public:
	BaseEffect();
	BaseEffect(LPCWSTR fileName, ID3D11Device* const d3dDevice);
	virtual ~BaseEffect();

	virtual RVAL BuildFromFXFile(LPCWSTR fileName, ID3D11Device* const d3dDevice);
	virtual RVAL BuildInputLayout(ID3D11Device* const d3dDevice);
	virtual RVAL LoadFromFXOFile(LPCWSTR fileName, ID3D11Device* const d3dDevice);

	RVAL ApplyForPass(int pass, ID3D11DeviceContext* const d3dDeviceContext) override;

	virtual RVAL SetEyePosW(const XMFLOAT3& position);
	virtual RVAL SetPlayerPosW(const XMFLOAT3& position);
	virtual RVAL SetLookVec(const XMFLOAT3& vec);

	virtual RVAL SetDiffuseMap(ID3D11ShaderResourceView* const diffuseMap);
	virtual RVAL SetNormalMap(ID3D11ShaderResourceView* const normalMap);

	virtual RVAL SetWorld(const XMFLOAT4X4& world);
	virtual RVAL SetWorldInvTran(const XMFLOAT4X4& worldInvTran);
	virtual RVAL SetWorldViewProj(const XMFLOAT4X4& worldViewProj);

	inline ID3D11InputLayout* const GetInputLayout() override { return _inputLayout; }
	inline UINT GetNumPasses() override { return _numTechPasses; }

protected:
	virtual void GetVariableHandles();

private:
	ID3D11InputLayout* _inputLayout;

	ID3DX11Effect* _effect;
	ID3DX11EffectTechnique* _technique;

	ID3DX11EffectMatrixVariable* _world;
	ID3DX11EffectMatrixVariable* _worldInvTran;
	ID3DX11EffectMatrixVariable* _worldViewProj;

	ID3DX11EffectVectorVariable* _eyePosW;
	ID3DX11EffectVectorVariable* _playerPosW;
	ID3DX11EffectVectorVariable* _lookVec;

	ID3DX11EffectShaderResourceVariable* _diffuseMap;
	ID3DX11EffectShaderResourceVariable* _normalMap;

	UINT _numTechPasses;

	void Setup(LPCWSTR fileName, ID3D11Device* const d3dDevice);

	const wchar_t* GetFilenameExtension(LPCWSTR fileName); 

	bool IsFXFile(LPCWSTR fileName);
	bool IsFXOFile(LPCWSTR fileName);
};

}
}

#endif