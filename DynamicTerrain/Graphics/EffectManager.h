#ifndef ENGINE_GRAPHICS_EFFECTMANAGER_H
#define ENGINE_GRAPHICS_EFFECTMANAGER_H

#include <map>
#include <string>
#include <D3D11.h>

#include "IEffect.h"
#include "..\Core\MethodHelpers.h"

using std::map;
using std::string;
using Engine::Core::RVAL;

namespace Engine 
{
namespace Graphics
{

class _declspec(dllexport) IEffectManager {
public:
	virtual ~IEffectManager() {}

	virtual void SetDevice(ID3D11Device* const d3dDevice) = 0;

	virtual RVAL SetupEffect(string, LPCWSTR) = 0;
	virtual RVAL AddEffect(string, IEffect*) = 0;
	virtual IEffect* const GetEffectByKey(string) = 0;
};

class _declspec(dllexport) EffectManager: public IEffectManager {
public:
	EffectManager()
		:	_d3dDevice(nullptr) {}

	EffectManager(ID3D11Device* const d3dDevice)
		:	_d3dDevice(d3dDevice) {}

	~EffectManager() {
		for (auto& e : _effects) {
			delete e.second;
		}
		_d3dDevice = nullptr;
	}

	void SetDevice(ID3D11Device* const d3dDevice) override { _d3dDevice = d3dDevice; }

	RVAL SetupEffect(string keyName, LPCWSTR fileName) override;
	RVAL AddEffect(string keyName, IEffect* effect) override;

	IEffect* const GetEffectByKey(string keyName) override { 
		if (_effects.find(keyName) != _effects.end())
			return _effects[keyName]; 
		else
			return nullptr;
	}

private:
	ID3D11Device* _d3dDevice;
	map<string, IEffect*> _effects;
};

}
}

#endif