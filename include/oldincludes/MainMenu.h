#pragma once
#ifndef MAINMENU_H
#define MAINMENU_H

#include "Items.h"
#include "Sprite.h"
#include "SDL_Setup.h"
#include <math.h>
#include "stdafx.h"
//#include "Main.h"
#include "CollissionRect.h"

class MainMenu:public Items
{
public:
	MainMenu(CSDL_Setup* passed_SDL_Setup, int *passed_MouseX, int *passed_MouseY, int passed_ScreenWidth,
				 int passed_ScreenHeight, float *passed_CameraX, float *passed_CameraY);
	MainMenu();
	~MainMenu();
	void Draw();
	int GetButtonClick();
	bool AboutMenu();
private:
	Sprite* mainMenu;
	Sprite* PlayButton;
	Sprite* PlayButton_Scroll;
	Sprite* OptionButton;
	Sprite* OptionButton_Scroll;
	Sprite* AboutButton;
	Sprite* AboutButton_Scroll;
	Sprite* AboutPage;
	Sprite* AboutPage_Scroll;
	CSDL_Setup* csdl_setup;


	int ScreenWidth;
	int ScreenHeight;

	bool start;
};

#endif
