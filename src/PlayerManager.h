#pragma once

#include "Sprite.h"
#include "SDL_Setup.h"
#include "CollissionRect.h"

class PlayerManager
{
public:
	PlayerManager();
	PlayerManager(CSDL_Setup* csdl_setup, float *CameraX, float *CameraY);
	bool IsClose(Sprite* sprite, float rangeSpecified);
	~PlayerManager(void);

private:
	Sprite* player;
	CSDL_Setup* csdl_setup;
};

