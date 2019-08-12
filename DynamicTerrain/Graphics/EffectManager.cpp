#include "EffectManager.h"
#include "BaseEffect.h"

using namespace Engine::Graphics;

RVAL EffectManager::SetupEffect(string keyName, LPCWSTR fileName) {
	if (_d3dDevice == nullptr) return RVAL::R_FAILED;
	if (keyName.size() < 1) return RVAL::R_EM_KEY_BLANK;
	if (_effects.find(keyName) != _effects.end()) return RVAL::R_EM_KEY_EXISTS;

	IEffect* effect = nullptr;

	try {
		effect = new BaseEffect(fileName, _d3dDevice);
	} catch (...) {
		delete effect;
		return RVAL::R_EM_INVALID_FILE;
	}

	if (RFAILURE(AddEffect(keyName, effect))) {
		delete effect;
		return RVAL::R_FAILED;
	}

	return RVAL::R_OK;
}

RVAL EffectManager::AddEffect(string keyName, IEffect* effect) {
	if (keyName.size() < 1) return RVAL::R_EM_KEY_BLANK;
	if (effect == nullptr) return RVAL::R_EM_EFFECT_NULL;
	if (_effects.find(keyName) != _effects.end()) return RVAL::R_EM_KEY_EXISTS;

	_effects.insert(make_pair(keyName, effect));

	return RVAL::R_OK;
}