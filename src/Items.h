#pragma once

#include "MainCharacter.h"
#include "Sprite.h"
#include "SDL_Setup.h"
#include <math.h>
#include "stdafx.h"

class Items:public MainCharacter
{
public:
	Items(CSDL_Setup* passed_SDL_Setup, int *passed_MouseX, int *passed_MouseY, float *passed_CameraX, float *passed_CameraY);
	Items();
	~Items();
	void Draw();

	void Update();
private:
	void UpdateAnimation();
	void UpdateControls();

	CSprite* chest;
};

