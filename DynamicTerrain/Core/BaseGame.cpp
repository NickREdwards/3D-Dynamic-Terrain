#include "BaseGame.h"

#include "..\Graphics\DX11Graphics.h"
#include "..\Graphics\IEffect.h"
#include "..\Graphics\ICamera.h"
#include "..\Graphics\IRenderer.h"
#include "..\Input\BasicWindowsInput.h"

using namespace Engine::Core;

BaseGame::BaseGame()
	:	_graphics(nullptr),
		_input(nullptr),
		_renderer(nullptr),
		_camera(nullptr),
		_terrain("TerrainSettings.txt"),
		_playerPos(XMFLOAT3(0.0f, 512.0f, 0.0f)),
		_playerBounds(AABB(0.0f, TS.NumChunksX * TS.ChunkDimensions.x, 2000.0f, 0.0f, 0.0f, TS.NumChunksZ * TS.ChunkDimensions.z)),
		_waterPlane(nullptr) {}

BaseGame::~BaseGame() {
	delete _waterPlane;
	_camera = nullptr;
	_renderer = nullptr;
	delete _input;
	delete _graphics;
}

RVAL BaseGame::Init(const HINSTANCE& hInstance) {
	_graphics = new DX11Graphics;

	auto rGraphicsInit = _graphics->Init(hInstance);
	if (RFAILURE(rGraphicsInit)) return rGraphicsInit;

	_input = new BasicWindowsInput(_graphics->GetHWND(), Viewport.right, Viewport.bottom);
	_input->LockMouseToCentre(true);

	_renderer = _graphics->GetRenderer();
	_camera = _renderer->GetCamera();

	Reset();

	if (RFAILURE(InitObjects())) {
		MessageBox(nullptr, L"Unable to initialise objects", L"Error during initialisation", MB_OK);
		return R_FAILED;
	}

	if (RFAILURE(InitEffects())) {
		MessageBox(nullptr, L"Unable to initialise effects", L"Error during initialisation", MB_OK);
		return R_FAILED;
	}

	return R_OK;
}

int BaseGame::Run() {
	return _graphics->RunGameLoop(this);
}

void BaseGame::Update(float deltaTime) {
	_input->Update();

	UpdatePlayer(deltaTime);
	UpdateCamera(deltaTime);
	UpdateTerrain();

	if (_input->IsKeyDown(VK_ESCAPE))
		PostQuitMessage(0);
}

void BaseGame::Render() {
	if (_renderer == nullptr) return;

	_renderer->ClearScreen();

	if (RSUCCESS(_renderer->Begin3D())) {
		if (RSUCCESS(_renderer->BeginEffect("Water"))) {
			RenderWater();
			_renderer->EndEffect();
		}
		if (RSUCCESS(_renderer->BeginEffect("Terrain"))) {
			auto currEffect = _renderer->GetCurrentEffect();

			if (RSUCCESS(SetEffectVariablesPerFrame(currEffect))) {
				RenderTerrain();
			}
			_renderer->EndEffect();
		}
		_renderer->End3D();
	}

	_renderer->Present();
}

void BaseGame::HandleMsg(const UINT& msg, const WPARAM& wParam, const LPARAM& lParam) {
	_input->HandleMsg(msg, wParam, lParam);
}

RVAL BaseGame::InitEffects() {
	if (RFAILURE(_renderer->AddEffect("Water", L"..\\Assets\\Effects\\Water.fxo")))
		return R_FAILED;

	if (RFAILURE(_renderer->AddEffect("Terrain", L"..\\Assets\\Effects\\Terrain.fxo")))
		return R_FAILED;

	return R_OK;
}

RVAL BaseGame::InitObjects() {
	auto device = _renderer->GetDevice();

	_playerPos = XMFLOAT3(TS.TerrainCentre.x, 0.0f, TS.TerrainCentre.z);

	_waterPlane = new WaterPlane;
	_waterPlane->SetPosition(XMFLOAT3(TS.TerrainCentre.x, 0.0f, TS.TerrainCentre.z));
	if (RFAILURE(_waterPlane->InitMesh(device))) {
		return R_FAILED;
	}

	if (RFAILURE(_terrain.Init(device))) {
		return R_FAILED;
	}

	return R_OK;
}

void BaseGame::Reset() {
	_camera->SetPosition(XMFLOAT3(TS.TerrainCentre.x, 0.0f, TS.TerrainCentre.z-200.0f));
	_camera->SetTarget(XMFLOAT3(TS.TerrainCentre.x, 0.0f, TS.TerrainCentre.z));
}

void BaseGame::UpdateTerrain() {
	if (_input->IsKeyDown('U'))
		_terrain.ToggleUpdating(false);
	else if (_input->IsKeyDown('I'))
		_terrain.ToggleUpdating(true);

	// Activate smooth meshing algorithm
	if (_input->IsKeyDown('M'))
		_terrain.SmoothTerrain();

	_terrain.Update();
}

void BaseGame::UpdateCamera(float deltaTime) {
	auto speed = _input->IsKeyDown(VK_SHIFT) ? 10.0f : 2.0f;
	speed *= max(0, deltaTime);

	if (_input->IsKeyDown(VK_LEFT))
		_camera->Yaw(0.001f * speed);

	if (_input->IsKeyDown(VK_RIGHT))
		_camera->Yaw(-0.001f * speed);

	if (_input->IsKeyDown(VK_UP))
		_camera->Zoom(1.0f * speed);

	if (_input->IsKeyDown(VK_DOWN))
		_camera->Zoom(-1.0f * speed);

	if (_input->IsKeyDown('W'))
		_camera->MoveForward(0.5f * speed);

	if (_input->IsKeyDown('S'))
		_camera->MoveBackward(0.5f * speed);

	if (_input->IsKeyDown('A'))
		_camera->MoveLeft(0.5f * speed);

	if (_input->IsKeyDown('D'))
		_camera->MoveRight(0.5f * speed);
}

void BaseGame::UpdatePlayer(float deltaTime) {
	// Update player position by the change in mouse position since last update
	auto deltaMouse = _input->GetDeltaMouse();

	auto camForward = XMLoadFloat3(&_camera->GetForward());
	auto camRight = XMLoadFloat3(&_camera->GetRight());
	auto playerPos = XMLoadFloat3(&_playerPos);

	playerPos += camRight * deltaMouse.X;
	playerPos -= camForward * deltaMouse.Y;

	XMStoreFloat3(&_playerPos, playerPos);

	// Constrain player marker to terrain boundaries
	if (_playerPos.x > _playerBounds.Right) _playerPos.x = _playerBounds.Right;
	if (_playerPos.x < _playerBounds.Left) _playerPos.x = _playerBounds.Left;
	if (_playerPos.z > _playerBounds.Back) _playerPos.z = _playerBounds.Back;
	if (_playerPos.z < _playerBounds.Front) _playerPos.z = _playerBounds.Front;

	// Add/remove/displace terrain depending on input
	if (_input->WasLMousePressed()) {
		if (_input->IsKeyDown(VK_CONTROL)) {
			_terrain.DisplaceVoxelsAroundPoint(_playerPos);
		} else {
			_terrain.RemoveVoxelsAroundPoint(_playerPos);
		}
	} else if (_input->WasRMousePressed()) {
		_terrain.AddVoxelsAroundPoint(_playerPos);
	}
}

void BaseGame::RenderWater() {
	RenderObject(_waterPlane);
}

void BaseGame::RenderTerrain() {
	if (!_terrain.AreAllChunksLoaded()) return;

	auto chunks = _terrain.GetChunks();

	IMesh* mesh = nullptr;
	XMFLOAT4X4 world;

	for (auto& c : chunks) {
		// Perform frustum culling to prevent drawing unnecessary chunks
		auto aabb = c->GetAABB();
		if (!_camera->IsAABBInViewFrustum(aabb)) continue;

		// Prevent access violations during re-meshing by using the backup mesh
		if (c->IsAwaitingRemesh())
			mesh = c->GetBackupMesh();
		else
			mesh = c->GetMesh();

		world = c->GetWorldMatrix();

		_renderer->DrawMesh(mesh, world);
	}
}

void BaseGame::RenderObject(IObject3D* const obj) {
	auto mesh = obj->GetMesh();
	auto world = obj->GetWorldMatrix();
	_renderer->DrawMesh(mesh, world);
}

RVAL BaseGame::SetEffectVariablesPerFrame(IEffect* const effect) {
	return effect->SetPlayerPosW(_playerPos);
}