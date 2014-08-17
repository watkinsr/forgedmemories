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

class CMainMenu:public Items
{
public:
	CMainMenu(CSDL_Setup* passed_SDL_Setup, int *passed_MouseX, int *passed_MouseY, int passed_ScreenWidth, int passed_ScreenHeight, float *passed_CameraX, float *passed_CameraY);
	CMainMenu();
	~CMainMenu();
	void Draw();
	int GetButtonClick();
	bool AboutMenu();
private:
	CSprite* MainMenu;
	CSprite* PlayButton;
	CSprite* PlayButton_Scroll;
	CSprite* OptionButton;
	CSprite* OptionButton_Scroll;
	CSprite* AboutButton;
	CSprite* AboutButton_Scroll;
	CSprite* AboutPage;
	CSprite* AboutPage_Scroll;
	CSDL_Setup* csdl_setup;


	int ScreenWidth;
	int ScreenHeight;

	bool start;
};

#endif