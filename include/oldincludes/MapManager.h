#pragma once

#include <sstream>
#include <fstream>
#include "Quests.h"
#include "NPC.h"



class MapManager
{
public:
	MapManager(void);
	~MapManager(void);
	enum Obj_Type {
		TypeNone,
		TypeQuest
	};

	void LoadQuests();
	void LoadNPCs();
};

