#pragma once
#ifndef GAMEMENU_H
#define GAMEMENU_H

#include "stdafx.h"
#include "Sprite.h"
#include "SDL_Setup.h"
#include "Interaction.h"
#include "TextManager.h"
#include "Point.h"
#include <map>
#include "Text.h"
#include "EventManager.h"
#include "Quests.h"
#include "EntityManager.h"

class GameMenu
{

public:
GameMenu(CSDL_Setup* passed_SDL_Setup, int *passed_MouseX, int *passed_MouseY,  float *passed_CameraX, float *passed_CameraY,
				 int ScreenWidth, int ScreenHeight);
~GameMenu();

void CheckForEsc(CSDL_Setup* passed_CSDL_Setup);
void DisplayGameMenu(MainCharacter* player, Sprite* playerSprite, Sprite* diaBoxSprite,
					 Sprite* enHP, CInteraction* interaction, CEnvironment* pub, Sprite* dialogIcon, vector<CEnemy*>Enemies,
					 EntityManager* entities);
void InitialiseGameMenu();
void RemoveQuest(BattleManager* battlemanager, EntityManager* entities);
void SetMenuGFX(Sprite* diaBoxSprite, Sprite* dialogIcon);
void ResetMenuGFX(Sprite* diaBoxSprite, Sprite* dialogIcon);
bool IsEscPressed();
int CheckUserPress(Sprite* menu_choice, vector<int>coords, EntityManager* entities);
void initialiseYValues();

vector<string>g_GameMenuText;

private:
	int w;
	int h;

	int rangeX;
	int rangeY;
	int count;

	static const int x = 263;
	int y;

	bool keyUp;
	bool keyPress;
	bool keyEsc;
	bool hasKeyBeenReleased;

	vector<int>GameMenuGFXVals;

	int prev_menu_choice_y;
	int opt_select;
	int screenWidth;
	int screenHeight;

	

	float *CameraX;
	float *CameraY;

	int *MouseX;
	int *MouseY;

	Quests* objOfQuests;

	Text* GameMenuTextObj;

	CSDL_Setup* csdl_setup;

	Point* point;



	CSDLFont_Setup* Dialog;



	vector <int>yValues;
};

#endif
