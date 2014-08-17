#include "stdafx.h"
#include "PlayerManager.h"


PlayerManager::PlayerManager(CSDL_Setup* passed_csdl_setup, float *CameraX, float *CameraY)
{
	csdl_setup = passed_csdl_setup;
	player = new CSprite(csdl_setup->GetRenderer(),"res/data/new_mc_spritesheet.png",300,250,50,50,CameraX,CameraY, CCollisionRect(0,0,50,50));
}

PlayerManager::PlayerManager(){

}
PlayerManager::~PlayerManager(void)
{
	delete player;
}

bool PlayerManager::IsClose(CSprite* sprite, float rangeSpecified){
	float playerX = (float)(player->GetSpriteCameraX() - 20);
	float playerY = (float)(player->GetSpriteCameraY() - 40);

	if (playerX - sprite->GetSpriteCameraX() >= -rangeSpecified && playerX - sprite->GetSpriteCameraX() <= rangeSpecified && 
		playerY - sprite->GetSpriteCameraY() >= -rangeSpecified && playerY - sprite->GetSpriteCameraY() <= rangeSpecified)
		return true;
	else 
		return false;
}

