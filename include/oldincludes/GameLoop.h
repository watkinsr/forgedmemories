#pragma once

#include <stdlib.h>


#include "SDL_Setup.h"
#include "Sprite.h"
#include "MainCharacter.h"
#include <math.h>
#include "stdafx.h"
#include "Environment.h"
#include "MainMenu.h"
#include "Items.h"
#include "Interaction.h"
#include "Enemies.h"
#include "SDL_Font_Setup.h"
#include "Battle.h"
#include "GameMenu.h"
#include "NPC.h"
#include "Quests.h"
#include "GameMenu.h"
#include "Loot.h"
#include "PlayerManager.h"
#include "EventManager.h"
#include "SFXManager.h"
#include "ConsoleDebug.h"
#include "FileManagement.h"
#include "EntityManager.h"

class GameLoop
{
public:
	GameLoop(int passed_ScreenWidth, int passed_ScreenHeight);
	~GameLoop(void);
	void Begin();
	void StartMenu();
	void Play();
	void GetStats();
	void LoadFiles();
	void GroupEntities();
	void DrawPlayerDebugStats();
private:
	int ScreenWidth;
	int ScreenHeight;

	float CameraX;
	float CameraY;

	MainCharacter* player;
	CEnvironment* pub;
	Items *chest;
	MainMenu* mainmenu;
	NPC* npcs;
	Quests* quests;
	CEnemies* enemies;
	CSDLFont_Setup* text;
	CInteraction* interaction;
	CSDL_Setup* csdl_setup;
	CBattle* battle;
	GameMenu* gamemenu;
	Loot* loot;
	PlayerManager* playermanager;
	EventManager* events;
	BattleManager* battlemanager;
	SFXManager* sounds;
	ConsoleDebug* debug;
	FileManagement* filemanager;
	EntityManager* entities;

	string level_str;
	string xp_str;
	string hp_str;
	string dmg_str;

	bool start;
	bool quit;

	int damage;
	int hp;
	int xp;
	int level;

	string temp;
	int MouseX;
	int MouseY;
};

