#pragma once
#ifndef ENEMY_H
#define ENEMY_H

#include "stdafx.h"
#include "Sprite.h"
#include <vector>

class CEnemy {
public:
	CEnemy(int x_passed, int y_passed, float *CameraX, float *CameraY, CSDL_Setup* csdl_setup, int passed_HP, int passed_Damage, int passed_XPGive);
	CEnemy();
	~CEnemy();
	void Draw();
	void setHP(int passed_HP);
	void setStats(int val);
	void SetAttackAnim(bool p_attack);
	int getHP();
	int GetSpriteCameraX();
	int GetSpriteCameraY();
	int GetXP();
	
	int getDamage();
	void changeHP(vector<CEnemy*> EnemyVect, int EnemyVectNum, int PlayerDamage);
	void GetAIPathSquare();
	int getHPWidth();
	CSprite* getEnemyHPSprite();
	CSprite* GetEnemySprite() {return enemy_sprite;}

private:
	int x;
	int y;
	int HP;
	int Damage;
	enum Difficulty {
		Easy,
		Medium,
		Hard 
	};
	int XPGive;
	CSprite* enemy_sprite;
	CSprite* En_HP;
	bool goRight;
	bool goDown;
	bool goUp;
	bool goLeft;
	bool isTimeSet;
	bool attack_anim;
	float time;
	float DecreaseHPBy;
	float PercentageHPChange;
	float temp;
	int count;
	int HPWidth;
};

#endif