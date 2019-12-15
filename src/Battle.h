#pragma once

#ifndef BATTLE_H
#define BATTLE_H

#include "stdafx.h"
#include "Sprite.h"
#include "SDL_Font_Setup.h"
#include "SDL_Setup.h"
#include "Interaction.h"
#include "MainCharacter.h"
#include "TextManager.h"
#include "EventManager.h"

class CBattle
{
public:
	CBattle(CSDL_Setup* passed_SDL_Setup, int *passed_MouseX, int *passed_MouseY, float *passed_CameraX, float *passed_CameraY);
	~CBattle();
	bool CheckBattle(vector <CEnemy*> *Enemies, MainCharacter* Player, CSprite* playerSprite);
	void DrawEnemies(vector <CEnemy*> Enemies);

	CSprite* GetDiaBoxSprite(){return DiaBox;};
	CSprite* GetEnHP(){return En_HP;};
	CSprite* GetDialogIcon(){return dialogIcon;};
	int GetEnemyNum(){return enemyBattleNum;};

private:
	CSprite* DiaBox;
	CSprite* En_HP;
	CSprite* dialogIcon;
	CSDL_Setup* csdl_setup;

	int *MouseX;
	int *MouseY;
	float *CameraX;
	float *CameraY;
	int count;
	int enemyBattleNum;

	bool isBattle;
	bool x;
	bool keyDown;
	
	bool stopAnimation;

};

#endif
