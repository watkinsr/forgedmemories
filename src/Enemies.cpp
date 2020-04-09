#include "Enemies.h"

CEnemies::CEnemies(CSDL_Setup* passed_SDL_Setup, int passed_TempX, int passed_TempY, int *passed_MouseX, int *passed_MouseY,  float *passed_CameraX, float *passed_CameraY, int passed_ScreenWidth, int passed_ScreenHeight)
{
	cameraX = passed_CameraX;
	cameraY = passed_CameraY;

	csdl_setup = passed_SDL_Setup;

	mouseX = passed_MouseX;
	mouseY = passed_MouseY;

	count = 0;

	TempX = passed_TempX;
	TempY = passed_TempY;

	CheckMouseEvent();

	HP = 0;
	xInt = 0;
	yInt = 0;

	ScreenWidth = passed_ScreenWidth;
	ScreenHeight = passed_ScreenHeight;
	
}

CEnemies::CEnemies()
{

}

CEnemies::~CEnemies()
{
}

void CEnemies::setHP(int passed_HP)
{
	HP = passed_HP;
}

int CEnemies::getHP()
{
	return HP;
}

void CEnemies::SpawnEnemy(int amount, int Health, int Damage, int XP, CEnvironment* level)
{
	for (int i = 0; i < amount; i++)
	{
		//randomise x and y based on map size.
		xInt = rand() % (level->GetMapSizeX() * ScreenWidth);
		yInt = 60;
		cout << "Enemy " << i << "'s X Location is set to: " << xInt << endl;
		cout << "Enemy " << i << "'s Y Location is set to: " << yInt << endl;
		Enemies.push_back(new CEnemy(xInt, yInt,cameraX,cameraY,csdl_setup,Health,Damage, XP)); 
	}
}

vector<CEnemy*>* CEnemies::GetEnemies()
{
	return &Enemies;
}

void CEnemies::Draw()
{
	for (vector<CEnemy*>::iterator i = Enemies.begin(); i != Enemies.end(); ++i)
	{
		(*i)->Draw();
	}
	if (Enemies.size() > 0){
	Enemies[0]->getEnemyHPSprite()->Draw();
	}
	else 
		cout << "Error 001: Can't draw Enemy's HP Sprite as the vector size is 0" << endl;;
	//testcase
	
}

void CEnemies::PrintEnemyTestLoc(){
	cout << "EnemyX: " << Enemies[0]->GetEnemySprite()->GetSpriteCameraX() << endl;
	cout << "EnemyY: " << Enemies[0]->GetEnemySprite()->GetSpriteCameraY() << endl;
}

void CEnemies::DetectPlayer(Sprite* player){
	float range = 80;
	for (vector<CEnemy*>::iterator i = Enemies.begin(); i != Enemies.end(); ++i)
	{
		if ((*i)->GetEnemySprite()->IsSpriteClose(player, (*i)->GetEnemySprite(), 80)){
				//MoveSpriteTowardsEntity needs revision.
				(*i)->SetAttackAnim(true);
				(*i)->GetEnemySprite()->MoveSpriteTowardsEntity(player, (*i)->GetEnemySprite());
		}
	}
}

void CEnemies::CheckMouseEvent()
{
	tempMouseX = (float)*mouseX;
	tempMouseY = (float)*mouseY;
	if (csdl_setup->GetMainEvent()->type == SDL_MOUSEMOTION)
	{
		for (vector<CEnemy*>::iterator i = Enemies.begin(); i != Enemies.end(); ++i)
		{
			if (tempMouseX >= (*i)->GetSpriteCameraX() && tempMouseX <= (*i)->GetSpriteCameraX()+50 && tempMouseY >= (*i)->GetSpriteCameraY() && tempMouseY <= (*i)->GetSpriteCameraY()+50)
			{
				//En_Fight->Draw();
				cout << "You have scrolled over an enemy!" << endl;
			}

			//for clicking
			if (csdl_setup->GetMainEvent()->type == SDL_MOUSEBUTTONDOWN)
			{

				if (csdl_setup->GetMainEvent()->button.button == SDL_BUTTON_LEFT)
				{
					if (tempMouseX >= (*i)->GetSpriteCameraX() && tempMouseX <= (*i)->GetSpriteCameraX()+50 && tempMouseY >= (*i)->GetSpriteCameraY() && tempMouseY <= (*i)->GetSpriteCameraY()+50)
					{
						cout << "You have clicked the enemy!" << endl;
						cout << "HP = " << endl; //mc->getHP() << endl;
					}
				}
			}
		}
	}
}
