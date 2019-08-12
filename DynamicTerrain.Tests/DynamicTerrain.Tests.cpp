#include <Windows.h>
#include <gtest\gtest.h>
#include <gmock\gmock.h>

#include "Shared.h"

//void main(int argc, char* argv[]) {
//	testing::InitGoogleTest(&argc, argv);
//	RUN_ALL_TESTS();
//}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Shared::GetInstance().SetHInstance(hInstance);
	testing::InitGoogleTest(&__argc, __argv);
	testing::InitGoogleMock(&__argc, __argv);
	RUN_ALL_TESTS();
}