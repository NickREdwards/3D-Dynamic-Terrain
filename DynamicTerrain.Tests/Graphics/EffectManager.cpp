#include <gtest\gtest.h>
#include <gmock\gmock.h>

#include <Graphics\EffectManager.h>
#include <Graphics\IEffect.h>

#include "..\Shared.h"
#include "..\Mocks\MockIEffect.h"
#include "..\Mocks\MockID3D11Device.h"

using namespace Engine::Graphics;
using Engine::Core::RVAL;

TEST(EffectManager, TestLoadingEffectsIntoManager) {
	MockID3D11Device mockD3DDevice;
	auto mockEffect = new MockIEffect;

	EffectManager fxMgr(&mockD3DDevice);

	ASSERT_EQ(RVAL::R_EM_KEY_BLANK,		fxMgr.AddEffect("", mockEffect));
	ASSERT_EQ(RVAL::R_EM_EFFECT_NULL,	fxMgr.AddEffect("MockEffect", nullptr));

	auto effectOne = fxMgr.GetEffectByKey("MockEffect");
	ASSERT_EQ(nullptr, effectOne);

	ASSERT_EQ(RVAL::R_OK,				fxMgr.AddEffect("MockEffect", mockEffect));
	ASSERT_EQ(RVAL::R_EM_KEY_EXISTS,	fxMgr.AddEffect("MockEffect", mockEffect));

	auto effectTwo = fxMgr.GetEffectByKey("MockEffect");
	ASSERT_NE(nullptr, effectTwo);
}