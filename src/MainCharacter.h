#pragma once

#ifndef MAINCHARACTER_H
#define MAINCHARACTER_H

#include "Sprite.h"
#include "SDL_Setup.h"
#include <math.h>
#include "stdafx.h"
#include "Environment.h"
#include "Enemy.h"

class MainCharacter{
public:
	MainCharacter(CSDL_Setup* passed_SDL_Setup, int *passed_MouseX, int *passed_MouseY, float *passed_CameraX, float *passed_CameraY, CEnvironment* passed_Environment);
	MainCharacter(void);
	~MainCharacter(void);
	double GetDistance(int X1, int Y1, int X2, int Y2);

	void Update(vector<CEnemy*>Enemies, vector<CMap*>Maps);
	void Draw();
	void StopMove();

	int GetHP();
	int GetDamage();
	int GetXP();
	int GetLevel();
	void SetHP(int p_hp);
	void SetDamage(int p_damage);
	void SetXP(int p_xp);
	void SetLevel(int p_level);
	void AddXP(int p_xp);
	void CheckLevelUp();
	int GetLevelStats();
	
	void DisplayAttackAnim();

	CSprite* GetPlayerSprite();

private:
	//player stats
	int hp;
	int xp;
	int damage;
	int level;
	int dex;


	//for checking level/setting stats
	int threshold;
protected:
	CEnvironment* Environment;

	void UpdateAnimation(CSprite* some_sprite);
	void UpdateControls(vector<CEnemy*>Enemies, vector<CMap*>Maps);
	float *CameraX;
	float *CameraY;

	int *MouseX;
	int *MouseY;
	CSDL_Setup* csdl_setup;

	CSprite* chest;
	CSprite* playerSprite;
	CSprite* playerStats;
	int timeCheck;
	bool keyDown;
	bool Follow;
	int Follow_Point_X;
	int Follow_Point_Y;

	float distance;
	bool stopAnimation;
	bool canPlayerMove;
};


#endif