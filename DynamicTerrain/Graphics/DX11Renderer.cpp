#include "DX11Renderer.h"

using namespace Engine::Graphics;

RVAL DX11Renderer::Init(HWND hWnd) {
	if (hWnd == nullptr) return RVAL::R_FAILED;

	auto rCreateDevice = CreateDevice();
	if (RFAILURE(rCreateDevice)) return rCreateDevice;

	_effectMgr.SetDevice(_d3dDevice);

	auto rCreateSwapChain = CreateSwapChain(hWnd);
	if (RFAILURE(rCreateSwapChain)) return rCreateSwapChain;

	auto rCreateRenderTarget = CreateRenderTarget();
	if (RFAILURE(rCreateRenderTarget)) return rCreateRenderTarget;

	auto rCreateDepthStencil = CreateDepthStencil();
	if (RFAILURE(rCreateDepthStencil)) return rCreateDepthStencil;

	auto rCreateViewport = CreateViewport();
	if (RFAILURE(rCreateViewport)) return rCreateViewport;

	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.DepthBias = false;
    rasterizerDesc.DepthBiasClamp = 0;
    rasterizerDesc.SlopeScaledDepthBias = 0;
    rasterizerDesc.DepthClipEnable = true;
    rasterizerDesc.ScissorEnable = false;
    rasterizerDesc.MultisampleEnable = false;
    rasterizerDesc.AntialiasedLineEnable = false;

	_d3dDevice->CreateRasterizerState(&rasterizerDesc, &_rasterizerState);
	_d3dDeviceContext->RSSetState(_rasterizerState);

	_camera = new IsometricCamera(Viewport.right, Viewport.bottom);

	_d3dDeviceContext->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);
	_d3dDeviceContext->RSSetViewports(1, &_viewport);

	return RVAL::R_OK;
}

void DX11Renderer::ClearScreen(XMFLOAT4 color) {
	if (_d3dDeviceContext == nullptr ||
		_renderTargetView == nullptr ||
		_depthStencilView == nullptr) return;

	_d3dDeviceContext->ClearRenderTargetView(_renderTargetView, reinterpret_cast<const float*>(&color));
	_d3dDeviceContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DX11Renderer::Present() {
	if (_swapChain == nullptr) return;
	_swapChain->Present(0, 0);
}

RVAL DX11Renderer::Begin2D() {
	return RVAL::R_FAILED;
}

void DX11Renderer::End2D() {
	
}

RVAL DX11Renderer::Begin3D() {
	_camera->Update();
	return RVAL::R_OK;
}

void DX11Renderer::End3D() {
	
}

RVAL DX11Renderer::BeginEffect(string keyName) {
	if (_currentEffect != nullptr) return RVAL::R_FAILED;

	auto effect = _effectMgr.GetEffectByKey(keyName);
	if (effect == nullptr) return RVAL::R_FAILED;

	_currentEffect = effect;

	if (_d3dDeviceContext != nullptr && _d3dDeviceContext) {
		_d3dDeviceContext->IASetInputLayout(effect->GetInputLayout());
		_d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	} else return RVAL::R_FAILED;

	auto eyePos = _camera->GetPosition();
	if (RFAILURE(effect->SetEyePosW(eyePos))) return RVAL::R_FAILED;

	return RVAL::R_OK;
}

void DX11Renderer::EndEffect() {
	_currentEffect = nullptr;
}

RVAL DX11Renderer::DrawMesh(IMesh* const mesh, const XMFLOAT4X4& worldMatrix) {
	if (mesh == nullptr || _currentEffect == nullptr) return RVAL::R_FAILED;
	
	MeshDrawLock meshLock(mesh);

	auto vBuffer = mesh->GetVertexBuffer();
	auto iBuffer = mesh->GetIndexBuffer();
	if (vBuffer == nullptr || iBuffer == nullptr) {
		return RVAL::R_FAILED;	
	}

	auto view = _camera->GetViewMatrix();
	auto proj = _camera->GetProjMatrix();
	auto wvp = XMMatrixToXMFloat4x4(XMLoadFloat4x4(&worldMatrix) * view * proj);

	if (RFAILURE(_currentEffect->SetWorld(worldMatrix))) {
		return RVAL::R_FAILED;	
	}

	if (RFAILURE(_currentEffect->SetWorldViewProj(wvp))) {
		return RVAL::R_FAILED;	
	}

	if (RFAILURE(_currentEffect->ApplyForPass(0, _d3dDeviceContext))) {
		return RVAL::R_FAILED;	
	}

	auto vertexStride = mesh->GetVertexStride();
	auto numIndices = mesh->GetNumIndices();
	UINT vertexOffset = 0;

	if (vBuffer != nullptr && iBuffer != nullptr) {
		try {
			_d3dDeviceContext->IASetVertexBuffers(0, 1, &vBuffer, &vertexStride, &vertexOffset);
			_d3dDeviceContext->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, 0);
			_d3dDeviceContext->DrawIndexed(numIndices, 0, 0);
			return RVAL::R_OK;
		} catch (...) {
			return RVAL::R_FAILED;
		}
	}

	return RVAL::R_FAILED;
}

RVAL DX11Renderer::CreateDevice() {
	UINT deviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	deviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;

	auto hrCreateDevice = D3D11CreateDevice(nullptr,
											D3D_DRIVER_TYPE_HARDWARE,
											nullptr,
											deviceFlags,
											nullptr,
											0,
											D3D11_SDK_VERSION,
											&_d3dDevice,
											&featureLevel,
											&_d3dDeviceContext);

	if (FAILED(hrCreateDevice)) {
		return RVAL::R_RR_CREATE_DEVICE_FAILED;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0) {
		MessageBox(nullptr, L"DirectX 11 is not supported by your GPU.", L"Compatibility Error", MB_OK);
		return RVAL::R_RR_CREATE_DEVICE_FAILED;
	}

	//if (FAILED(_d3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&_dxgiDebug))))
	//	return RVAL::R_FAILED;

	//_dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);

	return RVAL::R_OK;
}

RVAL DX11Renderer::CreateSwapChain(const HWND& hWnd) {
	DXGI_SWAP_CHAIN_DESC swapChainDesc = 
	{
		Viewport.right,
		Viewport.bottom,
		60, 1,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
		DXGI_MODE_SCALING_UNSPECIFIED,
		1, 0,
		DXGI_USAGE_RENDER_TARGET_OUTPUT,
		1, hWnd, true,
		DXGI_SWAP_EFFECT_DISCARD, 0
	};

	IDXGIDevice* dxgiDevice = nullptr;
	_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	      
	IDXGIAdapter* dxgiAdapter = nullptr;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

	IDXGIFactory* dxgiFactory = nullptr;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

	auto hrCreateSwapChain = dxgiFactory->CreateSwapChain(_d3dDevice, &swapChainDesc, &_swapChain);

	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	if (FAILED(hrCreateSwapChain)) {
		return RVAL::R_RR_CREATE_SWAP_CHAIN_FAILED;
	}

	return RVAL::R_OK;
}

RVAL DX11Renderer::CreateRenderTarget() {
	ID3D11Texture2D* backBuffer;

	_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), 
						  reinterpret_cast<void**>(&backBuffer));

	auto hrCreateRTV = _d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &_renderTargetView);

	backBuffer->Release();

	if (FAILED(hrCreateRTV)) {
		return RVAL::R_RR_CREATE_RENDER_TARGET_FAILED;
	}

	return RVAL::R_OK;
}

RVAL DX11Renderer::CreateDepthStencil() {
	D3D11_TEXTURE2D_DESC depthStencilDesc = 
	{
		Viewport.right,
		Viewport.bottom,
		1, 1,
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		1, 0,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_DEPTH_STENCIL,
		0, 0
	};

	auto hrCreateDSB = _d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);

	if (FAILED(hrCreateDSB)) {
		return RVAL::R_RR_CREATE_DEPTH_STENCIL_FAILED;
	}

	auto hrCreateDSV = _d3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

	if (FAILED(hrCreateDSV)) {
		return RVAL::R_RR_CREATE_DEPTH_STENCIL_FAILED;
	}

	return RVAL::R_OK;
}

RVAL DX11Renderer::CreateViewport() {
	_viewport.TopLeftX = 0;
	_viewport.TopLeftY = 0;
	_viewport.Width = Viewport.right;
	_viewport.Height = Viewport.bottom;
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;

	return RVAL::R_OK;
}