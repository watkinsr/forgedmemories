#pragma once

using namespace std;

#include <string>
#include <vector>
#include "NPC.h"
#include "FileManagement.h"

//this class manages the vector of NPCs
//should have a load NPC's public routine that reads an npcs.txt file and creates all of the npc objects and pushes back into an vector of npcs.
class NPCManager
{
public:
	NPCManager(void);
	~NPCManager(void);
	void SetFileLocation(int npc_id);
	void LoadNPCS();
private:
	vector<NPC*>npcs;
};

