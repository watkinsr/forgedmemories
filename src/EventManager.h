#pragma once
#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include "Sprite.h"
#include "MainCharacter.h"
#include "PlayerManager.h"
#include "TextManager.h"
#include "Interaction.h"
#include "Text.h"
#include "SDL_Setup.h"

const string FONT_FILE = "res/fonts/Minecraftia.ttf";
const int DIALOGSTARTPOSX = 50;
const int DIALOGSTARTPOSY = 400;

class EventManager
{
public:
	EventManager(CSDL_Setup* passed_CSDL_Setup, int *passed_MouseX, int *passed_MouseY);
	~EventManager(void);
	void ChestDialogEvent(MainCharacter* player, CSprite* playerSprite, CSprite* diaBoxSprite, 
		CSprite* enHP, CInteraction* interaction, CEnvironment* pub, CSprite* dialogIcon, vector<CEnemy*>Enemies);
	void DrawGame(MainCharacter* player, CSprite* playerSprite, CSprite* enHP, CEnvironment* pub,vector<CEnemy*>Enemies);
	void BattleDialogEvent(vector<Text>text, CInteraction* interaction, CSprite* dialogIcon, CSprite* diaBoxSprite);
	void NPCDialogEvent(vector<Text>text, CInteraction* interaction, CSprite* dialogIcon, CSprite* diaBoxSprite, CSprite* NPC);
	void QuestSuccessEvent(vector<Text>text, CInteraction* interaction, CSprite* dialogIcon, CSprite* diaBoxSprite, int font_size);
	void GameMenuEvent(vector<Text>text, CInteraction* interaction, CSprite* dialogIcon, CSprite* diaBoxSprite, int font_size);
private:
	int *mouseX;
	int *mouseY;
	bool show_once;
	CSDL_Setup* csdl_setup;
};

#endif