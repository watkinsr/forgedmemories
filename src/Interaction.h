#ifndef INTERACTION_H
#define INTERACTION_H

#pragma once
#include "SDL_Setup.h"
#include "Sprite.h"
#include "MainCharacter.h"
#include "stdafx.h"
#include "Environment.h"
#include "SDL_Font_Setup.h"



class CInteraction: public MainCharacter
{
public:
	CInteraction(CSDL_Setup* passed_SDL_Setup, int *passed_MouseX, int *passed_MouseY,  float *passed_CameraX, float *passed_CameraY);
	~CInteraction();

	void setRange(int x1, int y1, int x2, int y2);

	int checkEnter(CSprite* menu_select, int passed_screenWidth, int passed_screenHeight);

	bool checkInteraction(CSprite* some_sprite);

	int getRangeX();
	int getRangeY();
	bool CheckKeyPress();
	bool EnterPress();
	CSprite* getMenuArrow() {return Menu_Select;}
private:
	int w;
	int h;

	int rangeX;
	int rangeY;
	int count;

	bool click;

	bool keyDown;
	bool keyUp;
	bool keyPress;
	int opt_select;
	int screenWidth;
	int screenHeight;
	bool hasKeyBeenReleased;

	CSprite* Menu_Select;


};


#endif
