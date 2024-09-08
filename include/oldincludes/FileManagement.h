#pragma once

#include <sstream>
#include <fstream>
#include <string>
#include "stdafx.h"
#include <vector>
#include "EntityManager.h"
#include "SDL_Setup.h"

using namespace std;

class FileManagement
{
public:
	FileManagement(CSDL_Setup* csdl_setup, float* p_camera_x, float* p_camera_y);
	~FileManagement(void);
	bool OpenFile(string file_loc);
	void HandleQuestFile(string file_loc);
	void HandleNPCFile(string file_loc);
	void HandleSpriteFile();
	EntityManager* GetEntities();
private:
	vector<Quests*>quests;
	Quests* quest;
	NPC* npc;
	EntityManager* entities;
	Sprite* sprite;
	CSDL_Setup* csdl_setup;
	float* camera_x;
	float* camera_y;
};

