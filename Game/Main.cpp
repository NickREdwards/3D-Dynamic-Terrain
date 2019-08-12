#include <Windows.h>
#include <Core\BaseGame.h>

using Engine::Core::BaseGame;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	BaseGame game;
	if (game.Init(hInstance) != RVAL::R_OK) {
		MessageBox(nullptr, L"Unable to initialise game", L"Fatal Error", MB_OK);
		return 0;
	} else {
		return game.Run();
	}
}