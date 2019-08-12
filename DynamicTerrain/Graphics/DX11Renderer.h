#ifndef ENGINE_GRAPHICS_DX11RENDERER_H
#define ENGINE_GRAPHICS_DX11RENDERER_H

#include <Windows.h>
#include <D3D11.h>
#include <DirectXMath.h>
#include <dxgidebug.h>

#include "IRenderer.h"
#include "EffectManager.h"
#include "Settings.h"
#include "IsometricCamera.h"
#include "..\Geometry\MeshDrawLock.h"
#include "..\Core\MethodHelpers.h"

using DirectX::XMFLOAT4;
using DirectX::XMVECTOR;
using DirectX::XMFLOAT4X4;
using std::string;
using Engine::Core::RVAL;

class TestDX11Renderer;

namespace Engine 
{
namespace Graphics 
{

class _declspec(dllexport) DX11Renderer: public IRenderer {
friend class TestDX11Renderer;

public:
	DX11Renderer()
		:	_camera(nullptr),
			_currentEffect(nullptr),
			_d3dDevice(nullptr),
			_d3dDeviceContext(nullptr),
			_dxgiDebug(nullptr),
			_swapChain(nullptr),
			_depthStencilBuffer(nullptr),
			_renderTargetView(nullptr),
			_depthStencilView(nullptr),
			_rasterizerState(nullptr) {
	}

	~DX11Renderer() {
		delete _camera;

		if (_rasterizerState != nullptr && _rasterizerState) _rasterizerState->Release();
		if (_depthStencilView != nullptr && _depthStencilView) _depthStencilView->Release();
		if (_renderTargetView != nullptr && _renderTargetView) _renderTargetView->Release();
		if (_depthStencilBuffer != nullptr && _depthStencilBuffer) _depthStencilBuffer->Release();
		if (_swapChain != nullptr && _swapChain) _swapChain->Release();
		if (_dxgiDebug != nullptr && _dxgiDebug) _dxgiDebug->Release();

		if (_d3dDeviceContext != nullptr && _d3dDeviceContext) _d3dDeviceContext->Release();
		if (_d3dDevice != nullptr && _d3dDevice) _d3dDevice->Release();
	}

	RVAL Init(HWND hWnd) override;

	RVAL AddEffect(string keyName, LPCWSTR fileName) override {
		return _effectMgr.SetupEffect(keyName, fileName);
	}

	RVAL AddEffect(string keyName, IEffect* effect) override {
		return _effectMgr.AddEffect(keyName, effect);
	}

	void ClearScreen(XMFLOAT4 color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)) override;
	void Present() override;

	RVAL Begin2D() override;
	void End2D() override;

	RVAL Begin3D() override;
	void End3D() override;

	RVAL BeginEffect(string keyName) override;
	void EndEffect() override;

	RVAL DrawMesh(IMesh* const mesh, const XMFLOAT4X4& worldMatrix) override;

	ID3D11Device* const GetDevice() override { return _d3dDevice; }
	ID3D11DeviceContext* const GetContext() { return _d3dDeviceContext; }
	ICamera* const GetCamera() { return _camera; }
	IEffect* const GetCurrentEffect() { return _currentEffect; }

private:
	EffectManager _effectMgr;
	ICamera* _camera;
	IEffect* _currentEffect;

	ID3D11Device* _d3dDevice;
	ID3D11DeviceContext* _d3dDeviceContext;
	
	IDXGIDebug* _dxgiDebug;
	IDXGISwapChain* _swapChain;
	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11RenderTargetView* _renderTargetView;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11RasterizerState* _rasterizerState;

	D3D11_VIEWPORT _viewport;

	RVAL CreateDevice();
	RVAL CreateSwapChain(const HWND&);
	RVAL CreateRenderTarget();
	RVAL CreateDepthStencil();
	RVAL CreateViewport();
};

}
}

#endif