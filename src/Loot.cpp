#include "stdafx.h"
#include "Loot.h"

Loot::Loot()
{

}

Loot::Loot(string type, int passed_X, int passed_Y, int passed_RewardLevel, CSDL_Setup* passed_SDL_Setup, float *passed_CameraX, float *passed_CameraY)
{
	csdl_setup = passed_SDL_Setup;
	CameraX = passed_CameraX;
	CameraY = passed_CameraY;
	x = passed_X;
	y = passed_Y;

	distance = 0;

	if (type == "chest"){
	InitChest();
	}

}

Loot::~Loot(void)
{
	delete chest;
}

void Loot::DrawChest(){
	if (!CHEST_OPEN){
		GetLoot()->PlayAnimation(1,1,0,200);
		GetLoot()->Draw();
	}
	else 
		GetLoot()->Draw();
}

//Play chest open animation when the chest opens then stop the player.
void Loot::OpenChest(){
	//in the future, get the chest ID num from map, depending on that ID, give dialog.
	if (chest->HasAnimationFinished() == false){
			chest->AnimateOnce(1,3,0,200);
			CHEST_OPEN = true;
	}
}



//get distance by passing in 4 vars, 1 pair of x/y locs's for player, 1 pair for obj.
//get the difference between the x and y values and then perform equation that gets the distance.
double Loot::GetDistanceFromObj(int player_x, int player_y, int obj_x, int obj_y){
	double DifferenceX = player_x - obj_x;
	double DifferenceY = player_y - obj_y;
	distance = sqrt((DifferenceX * DifferenceX) + (DifferenceY * DifferenceY));
	return distance;
}

//create player sprite object, setup animation and size.
void Loot::SetPlayerSprite(){
	player = new CSprite(csdl_setup->GetRenderer(), "res/data/new_mc_spritesheet.png", 300, 250, 55,55, CameraX, CameraY, CCollisionRect(290,240,25,20));
	player->SetUpAnimation(4,2);
	player->SetOrgin((float)player->GetWidth()/2.0f, (float)player->GetHeight());
}

CSprite* Loot::GetPlayerSprite(){
	if (player != NULL)
		return player;

	return nullptr;
}
CSprite* Loot::GetLoot(){
	return chest;
}

void Loot::InitChest(){
	chest = new CSprite(csdl_setup->GetRenderer(), "res/data/environment/red-chest.png", 400, 200, 45, 45, CameraX, CameraY, CCollisionRect(0, 0, 46, 46));
	chest->SetUpAnimation(4, 1);
	chest->SetOrgin((float)chest->GetWidth(), (float)chest->GetHeight());
	CHEST_OPEN = false;
}

