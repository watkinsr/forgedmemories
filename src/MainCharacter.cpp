#include "MainCharacter.h"

MainCharacter::MainCharacter(){

}

MainCharacter::MainCharacter(CSDL_Setup* passed_SDL_Setup, int *passed_MouseX, int *passed_MouseY, float *passed_CameraX, float *passed_CameraY, CEnvironment* passed_Environment){
	Environment = passed_Environment;
	CameraX = passed_CameraX;
	CameraY = passed_CameraY;
	csdl_setup = passed_SDL_Setup;
	MouseX = passed_MouseX;
	MouseY = passed_MouseY;
	playerSprite = new CSprite(csdl_setup->GetRenderer(), "res/data/new_mc_spritesheet.png", 300, 250, 55,55, CameraX, CameraY, CCollisionRect(290,240,25,20));
	playerSprite->SetUpAnimation(4,2);
	playerSprite->SetOrgin((float)playerSprite->GetWidth() / 2.0f, (float)playerSprite->GetHeight());
	playerStats = new CSprite(csdl_setup->GetRenderer(), "res/data/stats_sm.png", 20, 20, 100,35, CameraX, CameraY, CCollisionRect());
	timeCheck = SDL_GetTicks();
	Follow = false;
	distance = 0;
	stopAnimation = false;

	//PLAYER STATS-------
	hp = 0;
	damage = 0;
	level = 1;
	dex = 0;
	xp = 0;
	keyDown = false;
	canPlayerMove = true;
}

MainCharacter::~MainCharacter(){
	delete playerStats;
	delete playerSprite;
}

// if level up then set hp higher, check if level up by setting level+1 if XP reaches a threshold, get threshold via checking current level.
int MainCharacter::GetLevelStats(){
	if (level == 1)
	{
		damage = 10;
		hp = 100;
		threshold = 100;
		return threshold;
	}
	if (level == 2)
	{
		damage = 15;
		hp = 150;
		threshold = 250;
		return threshold;
	}
	if (level == 3)
	{
		threshold = 400;
		return threshold;
	}
	if (level == 4)
	{
		threshold = 550;
		return threshold;
	}
	else
		return threshold;
}

int MainCharacter::GetXP(){

	return xp;
}

int MainCharacter::GetLevel(){
	return level;
}

int MainCharacter::GetHP(){
	return hp;
}

int MainCharacter:: GetDamage(){
	return damage;
}

void MainCharacter::SetHP(int p_hp){
	hp = hp + p_hp;
}

void MainCharacter::SetDamage(int p_damage){
	damage = p_damage;
}
void MainCharacter::SetXP(int p_xp){
	xp = p_xp;
}
void MainCharacter::SetLevel(int p_level){
	level = p_level;
}

void MainCharacter::AddXP(int p_xp){
	xp = xp + p_xp;
}

void MainCharacter::CheckLevelUp(){
	if (xp >= threshold){
		level++;
		GetLevelStats();
	}
}

void MainCharacter::Draw(){
	playerStats->DrawSteady();
	playerSprite->DrawSteady();
}

void MainCharacter::UpdateAnimation(CSprite* some_sprite){
	//gets angle between mouse and bob's position to determine animation in radians.
	float angle = atan2(Follow_Point_Y - *CameraY, Follow_Point_X - *CameraX);

	//conversion to degrees
	angle = (angle * (180.0f/3.14f) + 180.0f);

	if (!stopAnimation)
	{
		switch (csdl_setup->GetMainEvent()->type)
	{
	case SDL_KEYDOWN:
		if (!keyDown)
		{
			keyDown = true;
			switch (csdl_setup->GetMainEvent()->key.keysym.sym)
			{
			case SDLK_SPACE:
				some_sprite->PlayAnimation(0,3,1,10);
				break;

			default:
				break;
			}

			break;
		}


	case SDL_KEYUP:
		keyDown = false;
		switch (csdl_setup->GetMainEvent()->key.keysym.sym)
		{
		case SDLK_SPACE:
			some_sprite->PlayAnimation(0, 0, 0, 10);
			break;
		default:
			break;
		}

		break;
	default:
		break;
	}


		if (angle >= 0 && angle <= 360)
		{
			if (distance > 15)
				some_sprite->PlayAnimation(0, 2, 0, 300);
			else 
				some_sprite->PlayAnimation(1,1,0,300);
		}

	}
}

void MainCharacter::DisplayAttackAnim(){
	playerSprite->PlayAnimation(1,3,1,300);
}

void MainCharacter::UpdateControls(vector<CEnemy*>Enemies, vector<CMap*>Maps){
	//code for making character follow mouse click
	if (csdl_setup->GetMainEvent()->type == SDL_MOUSEBUTTONDOWN 
		|| csdl_setup->GetMainEvent()->type == SDL_MOUSEMOTION)
	{
		if (csdl_setup->GetMainEvent()->button.button == SDL_BUTTON_LEFT)
		{
			//changes position based on camera aswell
			Follow_Point_X = (int)(*CameraX - *MouseX + 300);
			Follow_Point_Y = (int)(*CameraY - *MouseY + 250);
			Follow = true;
		}
	}

	if ((timeCheck+10 < SDL_GetTicks()) && Follow)
	{

		distance = (float)GetDistance((int)*CameraX, (int)*CameraY, Follow_Point_X, Follow_Point_Y);

		if (distance == 0)
			stopAnimation = true;
		else 
			stopAnimation = false;

		if (distance > 15)
		{

			bool colliding = false;

			for (unsigned int i = 0; i < Environment->GetTrees().size(); i++)
			{
				if (playerSprite->isColliding(Environment->GetTrees()[i]->GetTrunk()->GetCollisionRect()))
				{
					cout << "Player sprite is colliding with a tree!" << endl;
					colliding = true;
				}
			}

			for (unsigned int i = 0; i < Enemies.size(); i++)
			{
				if (playerSprite->isColliding(Enemies[i]->GetEnemySprite()->GetCollisionRect()))
				{
					cout << "Player sprite is colliding with a enemy sprite!" << endl;
					if (*CameraX > Follow_Point_X)
					{
						*CameraX = *CameraX + 5;
					}
					if (*CameraX < Follow_Point_X)
					{
						*CameraX = *CameraX - 5;
					}
					if (*CameraY > Follow_Point_Y)
					{
						*CameraY = *CameraY + 5;
					}
					if (*CameraY < Follow_Point_Y)
					{
						*CameraY = *CameraY - 5;
					}
					Follow_Point_X = (int)*CameraX;
					Follow_Point_Y = (int)*CameraY;
					distance = 0;
					Follow = false;

					colliding = true;
				}

			}

			for (int i = 0; i < Maps[i]->GetTableNum(); i++)
			{
				if (playerSprite->isColliding(Maps[i]->GetTable()->GetCollisionRect()))
				{
					cout << "Player sprite is colliding with a table!" << endl;
					if (*CameraX > Follow_Point_X)
					{
						*CameraX = *CameraX + 5;
					}
					if (*CameraX < Follow_Point_X)
					{
						*CameraX = *CameraX - 5;
					}
					if (*CameraY > Follow_Point_Y)
					{
						*CameraY = *CameraY + 5;
					}
					if (*CameraY < Follow_Point_Y)
					{
						*CameraY = *CameraY - 5;
					}
					Follow_Point_X = (int)*CameraX;
					Follow_Point_Y = (int)*CameraY;
					distance = 0;
					Follow = false;

					colliding = true;
				}
			}

			for (int i = 0; i < Maps[i]->GetTableNum(); i++)
			{
				if (playerSprite->isColliding(Maps[i]->GetWallOutline_Top()->GetCollisionRect()) || playerSprite->isColliding(Maps[i]->GetWallOutline_Left()->GetCollisionRect())
					|| playerSprite->isColliding(Maps[i]->GetWallOutline_Bottom()->GetCollisionRect()) || playerSprite->isColliding(Maps[i]->GetWallOutline_Right()->GetCollisionRect()))
				{
					cout << "Player sprite is colliding with outside wall" << endl;
					if (*CameraX > Follow_Point_X)
					{
						*CameraX = *CameraX + 5;
					}
					if (*CameraX < Follow_Point_X)
					{
						*CameraX = *CameraX - 5;
					}
					if (*CameraY > Follow_Point_Y)
					{
						*CameraY = *CameraY + 5;
					}
					if (*CameraY < Follow_Point_Y)
					{
						*CameraY = *CameraY - 5;
					}
					Follow_Point_X = (int)*CameraX;
					Follow_Point_Y = (int)*CameraY;
					distance = 0;
					Follow = false;

					colliding = true;
				}
			}

			if (!colliding)
			{
				if (*CameraX != Follow_Point_X)
				{
					*CameraX = *CameraX - (((*CameraX-Follow_Point_X)/distance) * 1.5f ) ;
				}

				if (*CameraY != Follow_Point_Y)
				{
					*CameraY = *CameraY - (((*CameraY-Follow_Point_Y)/distance) * 1.5f ) ;
				}
			}
		}
		else 
			Follow = false;

		timeCheck = SDL_GetTicks();
	}
}

void MainCharacter::Update(vector<CEnemy*>Enemies, vector<CMap*>Maps){
	UpdateAnimation(playerSprite);
	UpdateControls(Enemies, Maps);

	
}

CSprite* MainCharacter::GetPlayerSprite(){
	return playerSprite;
}

//Stops player at the current camera position as the follow point is where you have clicked.
void MainCharacter::StopMove(){
	Follow_Point_X = (int)*CameraX;
	Follow_Point_Y = (int)*CameraY;
}
double MainCharacter::GetDistance(int X1, int Y1, int X2, int Y2){
	double DifferenceX = X1 - X2;
	double DifferenceY = Y1 - Y2;
	double distance = sqrt((DifferenceX * DifferenceX) + (DifferenceY * DifferenceY));
	return distance;
}
