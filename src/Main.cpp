#include "stdafx.h"
//#include <vld.h>
#include "GameLoop.h"

const string g_FONT_FILE = "res/fonts/Minecraftia.ttf";

using namespace std;

int main()
{
	GameLoop*  mainGame = new GameLoop(640, 480);
	// mainGame->Begin(false);

	delete mainGame;

	return 0;
}

