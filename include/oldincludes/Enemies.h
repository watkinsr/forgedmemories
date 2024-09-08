#ifndef ENEMIES_H
#define ENEMIES_H
#pragma once
#include "stdafx.h"
#include "Sprite.h"
#include "SDL_Setup.h"
#include <sstream>
#include <fstream>
#include "SDL_Font_Setup.h"
#include "Environment.h"
#include "Enemy.h"

class CEnemies
{
public:
	CEnemies(CSDL_Setup* passed_SDL_Setup, int passed_TempX, int passed_TempY, int *passed_MouseX, int *passed_MouseY,  float *passed_CameraX, float *passed_CameraY, int passed_ScreenWidth, int passed_ScreenHeight);
	CEnemies();
	~CEnemies();

	void Draw();
	void CheckMouseEvent();
	void setHP(int passed_HP);
	void DetectPlayer(Sprite* player);
	void PrintEnemyTestLoc();
	int getHP();
	vector<CEnemy*>* GetEnemies();

	void SpawnEnemy(int amount, int Health, int Damage, int XP, CEnvironment* level);
	
private:
	CSDL_Setup* csdl_setup;
	vector<CEnemy*>Enemies;
	Sprite* en_HP;
	float tempMouseX;
	float tempMouseY;
	float *cameraX;
	float *cameraY;
	int *mouseX;
	int *mouseY;
	int TempX;
	int TempY;
	int ScreenWidth;
	int ScreenHeight;

	int HP;
	int xInt;
	int yInt;
	int count; 

	bool fight;
};


#endif
