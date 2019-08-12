#ifndef MOCKIEFFECT_H
#define MOCKIEFFECT_H

#include <gmock\gmock.h>
#include <Graphics\IEffect.h>
#include <Core\MethodHelpers.h>

using namespace Engine::Graphics;

class MockIEffect: public IEffect {
public:
	MOCK_METHOD2(BuildFromFXFile, RVAL(LPCWSTR, ID3D11Device* const));
	MOCK_METHOD1(BuildInputLayout, RVAL(ID3D11Device* const));
	MOCK_METHOD2(LoadFromFXOFile, RVAL(LPCWSTR, ID3D11Device* const));

	MOCK_METHOD2(ApplyForPass, RVAL(int, ID3D11DeviceContext* const));
	
	MOCK_METHOD1(SetEyePosW, RVAL(const XMFLOAT3&));
	MOCK_METHOD1(SetPlayerPosW, RVAL(const XMFLOAT3&));
	MOCK_METHOD1(SetLookVec, RVAL(const XMFLOAT3&));

	MOCK_METHOD1(SetDiffuseMap, RVAL(ID3D11ShaderResourceView* const));
	MOCK_METHOD1(SetNormalMap, RVAL(ID3D11ShaderResourceView* const));

	MOCK_METHOD1(SetWorld, RVAL(const XMFLOAT4X4&));
	MOCK_METHOD1(SetWorldInvTran, RVAL(const XMFLOAT4X4&));
	MOCK_METHOD1(SetWorldViewProj, RVAL(const XMFLOAT4X4&));

	MOCK_METHOD0(GetInputLayout, ID3D11InputLayout* const());
	MOCK_METHOD0(GetNumPasses, UINT());
};

#endif