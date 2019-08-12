#include <gtest\gtest.h>
#include <gmock\gmock.h>

#include <Core\MethodHelpers.h>
#include <Graphics\BaseEffect.h>

#include "..\Mocks\MockID3D11Device.h"
#include "..\Mocks\MockID3DX11Effect.h"

using Engine::Core::RVAL;
using namespace Engine::Graphics;

TEST(BaseEffect, InitWithInvalidFile) {
	MockID3D11Device mockD3DDevice;
	// Initialise with invalid extension (only .fx and .fxo allowed)
	ASSERT_ANY_THROW(BaseEffect effect(L"Effect.test", &mockD3DDevice));
}

TEST(BaseEffect, BuildFromFXFile) {
	MockID3D11Device mockD3DDevice;
	BaseEffect effect;
	ASSERT_TRUE(RFAILURE(effect.BuildFromFXFile(L"Effect.test", &mockD3DDevice)));
	ASSERT_TRUE(RFAILURE(effect.BuildFromFXFile(L"Effect.fxo", &mockD3DDevice)));
}

TEST(BaseEffect, LoadFromFXOFile) {
	MockID3D11Device mockD3DDevice;
	BaseEffect effect;
	ASSERT_TRUE(RFAILURE(effect.LoadFromFXOFile(L"Effect.test", &mockD3DDevice)));
	ASSERT_TRUE(RFAILURE(effect.LoadFromFXOFile(L"Effect.fx", &mockD3DDevice)));
}

TEST(BaseEffect, BuildInputLayout) {
	MockID3D11Device mockD3DDevice;
	BaseEffect effect;

	// _technique will be null, so BuildInputLayout should fail
	ASSERT_TRUE(RFAILURE(effect.BuildInputLayout(&mockD3DDevice)));
}