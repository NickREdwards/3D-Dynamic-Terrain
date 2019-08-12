#include <gtest\gtest.h>
#include <Input\BasicWindowsInput.h>

using namespace Engine::Input;

TEST(BasicWindowsInput, IsKeyDown) {
	BasicWindowsInput input(nullptr, 800, 600);

	ASSERT_FALSE(input.IsKeyDown(VK_LEFT));

	input.HandleMsg(WM_KEYDOWN, VK_LEFT, 0);
	input.Update();

	ASSERT_TRUE(input.IsKeyDown(VK_LEFT));

	input.HandleMsg(WM_KEYUP, VK_LEFT, 0);
	input.Update();

	ASSERT_FALSE(input.IsKeyDown(VK_LEFT));
}