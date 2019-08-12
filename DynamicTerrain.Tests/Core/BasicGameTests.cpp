#include <gtest\gtest.h>
#include <Core\BaseGame.h>
#include <Core\MethodHelpers.h>

using namespace Engine::Core;

TEST(BasicGameTests, Init) {
	BaseGame game;
	ASSERT_TRUE(game.Init(nullptr) != RVAL::R_OK);
}