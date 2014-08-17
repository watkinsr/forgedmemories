#pragma once
#ifndef NPC_H
#define NPC_H

#include "Sprite.h"
#include <vector>
#include "Quests.h"
#include "SDL_Setup.h"
#include "Text.h"
#include "EventManager.h"
#include "TextManager.h"
#include "Point.h"

using namespace std;

class NPC
{
public:
	NPC(int x, int y, int npc_id, string npc_sprite_loc, string name, float* p_camera_x, float* p_camera_y, CSDL_Setup* p_csdl_setup, vector<Quests*>quests);

	NPC(int x_passed, int y_passed, int *passed_MouseX, int *passed_MouseY, float *passed_CameraX, float *passed_CameraY, int passed_screenWidth, 
		int passed_screenHeight, CSDL_Setup* passed_csdl_setup, int NPCref);

	~NPC(void);

	void Draw();
	bool isPlayerCloseToGetQuest(Quests *objOfQuests, vector<NPC> vectOfNPCS, CSprite* player);
	void DisplayQuestMenu(int npc_id, vector<Quests*>quests, MainCharacter* player, CSprite* playerSprite, CSprite* diaBoxSprite, 
						CSprite* enHP, CInteraction* interaction, CEnvironment* pub, CSprite* dialogIcon, vector<CEnemy*>Enemies);
	void GetNpcFileLocation();
	void InitialiseQuestMenu(int npc_id, Quests *quest);
	int GetNPCX();
	int GetNPCY();
	bool CheckUserPress(CSprite* menu_arrow_sprite, int npc_id, Quests* quests);
	string GetQuestDesc();
	Quests GetQuest();

	CSprite GetNPCSprite();

	enum YPointsForMenuChoice{
		FIRSTCHOICE = 406,
		SECONDCHOICE = 426,
		THIRDCHOICE = 446
	};	

private:
	int numNPC;
	int level_num;
	int x;
	int y;
	int optSelect;

	bool hasKeyBeenReleased;
	bool isPlayerClose;
	bool isQuestAdded;
	bool firstPress;
	bool exitNPCDialog;

	int countEnterPress;
	int screenWidth;
	int screenHeight;

	int* MouseX;
	int* MouseY;

	float* CameraX;
	float* CameraY;

	string file_loc;

	CSprite* npcSprite;
	CSprite* npc_hover_interact;
	CSprite* mainPlayer;
	CSprite* Menu_Select;
	CSprite* DiaBox;

	MainCharacter* mc;

	vector<string>NPCAssets;


	Quests NPCQuest;

	Text* NPCTextObj;
	CSDL_Setup* csdl_setup;

	string questDesc;
	bool isQuestCompleted;

	TextManager* PrintText;
	
	Point* point;

};

#endif