#include "stdafx.h"
#include "Battle.h"

CBattle::CBattle(CSDL_Setup* passed_SDL_Setup, int *passed_MouseX, int *passed_MouseY, float *passed_CameraX, float *passed_CameraY)
{
	csdl_setup = passed_SDL_Setup;
	CameraX = passed_CameraX;
	CameraY = passed_CameraY;
	MouseX = passed_MouseX;
	MouseY = passed_MouseY;

	DiaBox = new CSprite(csdl_setup->GetRenderer(),"res/data/dia_box_norm.png",0,385,640,100,CameraX,CameraY, CCollisionRect());
	En_HP = new CSprite(csdl_setup->GetRenderer(),"res/data/hp_enem.png",10,-10,24,4,CameraX,CameraY, CCollisionRect());
	dialogIcon = new CSprite(csdl_setup->GetRenderer(),"res/data/menu_select.png",32,426,13,13,CameraX,CameraY, CCollisionRect());
	isBattle = false;
	count = 0;
}

CBattle::~CBattle()
{

}


bool CBattle::CheckBattle(vector <CEnemy*> *Enemies, MainCharacter* Player, CSprite* playerSprite)
{
	int diffX = 250;
	int diffY = 250;
	int EnemyVectSize = (*Enemies).size();
	
		for (vector<CEnemy*>::iterator i = (*Enemies).begin(); i != (*Enemies).end(); ++i)
		{
			count++;

			if (count == (*Enemies).size())
			{
				count = 0;
			}
			if (playerSprite->GetSpriteCameraX() - (*i)->GetSpriteCameraX() < 60 && playerSprite->GetSpriteCameraX() - (*i)->GetSpriteCameraX() > -60 
				&& playerSprite->GetSpriteCameraY() - (*i)->GetSpriteCameraY() < 60 && playerSprite->GetSpriteCameraY() - (*i)->GetSpriteCameraY() > -60)
			{
				int testEnemySpriteX = (*i)->GetSpriteCameraX();
				int testEnemySpriteY = (*i)->GetSpriteCameraY();
				int testPlayerSpriteX = playerSprite->GetSpriteCameraX();
				int testPlayerSpriteY = playerSprite->GetSpriteCameraY();

				cout << "Enemy sprite loc: " << (*i)->GetSpriteCameraX() << " " <<  (*i)->GetSpriteCameraY() << endl;
				cout << "Player sprite loc: " << playerSprite->GetSpriteCameraX() <<  " " << playerSprite->GetSpriteCameraY() << endl;

				enemyBattleNum = count-1;
				if (enemyBattleNum == -1)
				{
					enemyBattleNum = 0;
				}
				diffX = playerSprite->GetSpriteCameraX()-30 - (*i)->GetSpriteCameraX(); 
				diffY = playerSprite->GetSpriteCameraY()-50 - (*i)->GetSpriteCameraY();
			}
		}

	if (diffX < 30 && diffX > -30 && diffY < 30 && diffY > -30)
	{
		isBattle = true;
		return isBattle;
	}
	else
		isBattle = false;
		return isBattle;
}



void CBattle::DrawEnemies(vector <CEnemy*> Enemies)
{
	for (vector<CEnemy*>::iterator i = Enemies.begin(); i != Enemies.end(); ++i)
	{
		(*i)->Draw();
	}
	En_HP->Draw();
}