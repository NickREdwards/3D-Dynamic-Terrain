#ifndef MOCKIEFFECTMANAGER_H
#define MOCKIEFFECTMANAGER_H

#include <gmock\gmock.h>

#include <Graphics\EffectManager.h>

class MockIEffectManager: public IEffectManager {
public:
	MOCK_METHOD1(SetDevice, void(ID3D11Device* const));
	MOCK_METHOD2(SetupEffect, RVAL(string, LPCWSTR));
	MOCK_METHOD2(AddEffect, RVAL(string, IEffect*));
	MOCK_METHOD1(GetEffectByKey, IEffect* const(string));
};

#endif