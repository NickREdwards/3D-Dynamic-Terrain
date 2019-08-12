#include "BaseEffect.h"

using namespace Engine::Graphics;

BaseEffect::BaseEffect()
	:	_inputLayout(nullptr),
		_effect(nullptr),
		_technique(nullptr),
		_world(nullptr),
		_worldInvTran(nullptr),
		_worldViewProj(nullptr),
		_eyePosW(nullptr),
		_playerPosW(nullptr),
		_lookVec(nullptr),
		_diffuseMap(nullptr),
		_normalMap(nullptr),
		_numTechPasses(0) {}

BaseEffect::BaseEffect(LPCWSTR fileName, ID3D11Device* const d3dDevice)
	:	_inputLayout(nullptr),
		_effect(nullptr),
		_technique(nullptr),
		_world(nullptr),
		_worldInvTran(nullptr),
		_worldViewProj(nullptr),
		_eyePosW(nullptr),
		_playerPosW(nullptr),
		_lookVec(nullptr),
		_diffuseMap(nullptr),
		_normalMap(nullptr),
		_numTechPasses(0) {
	Setup(fileName, d3dDevice);
}

BaseEffect::~BaseEffect() {
	if (_normalMap) _normalMap->Release();
	if (_diffuseMap) _diffuseMap->Release();

	if (_lookVec) _lookVec->Release();
	if (_playerPosW) _playerPosW->Release();
	if (_eyePosW) _eyePosW->Release();

	if (_worldViewProj) _worldViewProj->Release();
	if (_worldInvTran) _worldInvTran->Release();
	if (_world) _world->Release();

	if (_technique) _technique->Release();
	if (_effect) _effect->Release();
	
	if (_inputLayout) _inputLayout->Release();
}

RVAL BaseEffect::BuildFromFXFile(LPCWSTR fileName, ID3D11Device* const d3dDevice) {
	if (!IsFXFile(fileName)) return RVAL::R_FAILED;
	
	//GetVariableHandles();
	return RVAL::R_FAILED;
}

RVAL BaseEffect::BuildInputLayout(ID3D11Device* const d3dDevice) {
	if (_technique == nullptr) return RVAL::R_FAILED;

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	D3DX11_PASS_DESC passDesc;
	_technique->GetPassByIndex(0)->GetDesc(&passDesc);

	if (FAILED(d3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &_inputLayout))) {
		MessageBox(nullptr, L"Unable to build input layout when loading effect", L"Loading Error", MB_OK);
		return RVAL::R_FAILED;
	}

	return RVAL::R_OK;
}

RVAL BaseEffect::LoadFromFXOFile(LPCWSTR fileName, ID3D11Device* const d3dDevice) {
	if (!IsFXOFile(fileName)) return RVAL::R_FAILED;

	DWORD shaderFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
	
	auto hrCreateEffect = D3DX11CreateEffectFromFile(fileName, shaderFlags, d3dDevice, &_effect);

	if (FAILED(hrCreateEffect))
		return RVAL::R_FAILED;

	GetVariableHandles();

	return RVAL::R_OK;
}

RVAL BaseEffect::ApplyForPass(int pass, ID3D11DeviceContext* const d3dDeviceContext) {
	if (FAILED(_technique->GetPassByIndex(pass)->Apply(0, d3dDeviceContext))) {
		return RVAL::R_FAILED;
	}
	return RVAL::R_OK;
}

RVAL BaseEffect::SetEyePosW(const XMFLOAT3& position) {
	if (FAILED(_eyePosW->SetRawValue(&position, 0, sizeof(XMFLOAT3))))
		return RVAL::R_FAILED;

	return RVAL::R_OK;
}

RVAL BaseEffect::SetPlayerPosW(const XMFLOAT3& position) {
	if (FAILED(_playerPosW->SetRawValue(&position, 0, sizeof(XMFLOAT3))))
		return RVAL::R_FAILED;

	return RVAL::R_OK;
}

RVAL BaseEffect::SetLookVec(const XMFLOAT3& vec) {
	if (FAILED(_lookVec->SetRawValue(&vec, 0, sizeof(XMFLOAT3))))
		return RVAL::R_FAILED;

	return RVAL::R_OK;
}

RVAL BaseEffect::SetDiffuseMap(ID3D11ShaderResourceView* const diffuseMap) {
	if (FAILED(_diffuseMap->SetResource(diffuseMap)))
		return RVAL::R_FAILED;

	return RVAL::R_OK;
}

RVAL BaseEffect::SetNormalMap(ID3D11ShaderResourceView* const normalMap) {
	if (FAILED(_normalMap->SetResource(normalMap)))
		return RVAL::R_FAILED;

	return RVAL::R_OK;
}

RVAL BaseEffect::SetWorld(const XMFLOAT4X4& world) {
	if (FAILED(_world->SetMatrix(reinterpret_cast<const float*>(&world))))
		return RVAL::R_FAILED;

	return RVAL::R_OK;
}

RVAL BaseEffect::SetWorldInvTran(const XMFLOAT4X4& worldInvTran) {
	if (FAILED(_worldInvTran->SetMatrix(reinterpret_cast<const float*>(&worldInvTran))))
		return RVAL::R_FAILED;

	return RVAL::R_OK;
}

RVAL BaseEffect::SetWorldViewProj(const XMFLOAT4X4& worldViewProj) {
	if (FAILED(_worldViewProj->SetMatrix(reinterpret_cast<const float*>(&worldViewProj))))
		return RVAL::R_FAILED;

	return RVAL::R_OK;
}

void BaseEffect::GetVariableHandles() {
	_technique = _effect->GetTechniqueByName("MainTech");

	_world = _effect->GetVariableByName("gWorld")->AsMatrix();
	_worldInvTran = _effect->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	_worldViewProj = _effect->GetVariableByName("gWorldViewProj")->AsMatrix();

	_eyePosW = _effect->GetVariableByName("gEyePosW")->AsVector();
	_playerPosW = _effect->GetVariableByName("gPlayerPosW")->AsVector();
	_lookVec = _effect->GetVariableByName("gLookVec")->AsVector();

	_diffuseMap = _effect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	_normalMap = _effect->GetVariableByName("gNormalMap")->AsShaderResource();
}

void BaseEffect::Setup(LPCWSTR fileName, ID3D11Device* const d3dDevice) {
	auto effectLoaded = false;

	if (IsFXFile(fileName))
		effectLoaded = RSUCCESS(BuildFromFXFile(fileName, d3dDevice));
	else if (IsFXOFile(fileName))
		effectLoaded = RSUCCESS(LoadFromFXOFile(fileName, d3dDevice));
	else
		throw std::exception("Unable to load effect - invalid file specified");

	if (effectLoaded) BuildInputLayout(d3dDevice);
	else
		throw std::exception("Unable to load effect - could not load from file");
}

/* Includes '.' */
const wchar_t* BaseEffect::GetFilenameExtension(LPCWSTR fileName) {
	auto ext = wcsrchr(fileName, '.');
	
	if (ext == nullptr || wcslen(ext) < 2)
		throw std::exception("Unable to load effect - invalid file specified");

	return ext;
}

bool BaseEffect::IsFXFile(LPCWSTR fileName) {
	auto extension = GetFilenameExtension(fileName);

	if (wcslen(extension) != 3) return false;

	if (extension[1] == 'f' || extension[1] == 'F' &&
		extension[2] == 'x' || extension[2] == 'X' ) return true;

	return false;
}

bool BaseEffect::IsFXOFile(LPCWSTR fileName) {
	auto extension = GetFilenameExtension(fileName);

	if (wcslen(extension) < 4) return false;

	if (extension[1] == 'f' || extension[1] == 'F' &&
		extension[2] == 'x' || extension[2] == 'X' &&
		extension[3] == 'o' || extension[3] == 'O' ) return true;

	return false;
}