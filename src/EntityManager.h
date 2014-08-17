#pragma once

#include "Quests.h"
#include "NPC.h"
#include "Sprite.h"
#include "Environment.h"
#include <vector>

//this class will have vectors of NPCs, quests, sprites, maps and will be able to add onto/remove from them
//there should be a GetQuests() and CreateQuests() function for example.
class EntityManager
{
public:
	EntityManager(void);
	~EntityManager(void);
	void AddQuests(Quests* quest);
	void RemoveQuest(int quest_id);
	bool AnyQuestAccepted();
	vector<Quests*>GetQuests();
	void AddNPCs(NPC* npc);
	vector<NPC*>GetNPCs();
	void AddSprites(CSprite* sprite);
	vector<CSprite*> GetSprites();
	void DrawAll();
	bool CheckPlayerClose(CSprite* player);
	vector<CSprite*>sprites;

private:
	vector<Quests*>quests;
	vector<NPC*>npcs;
	vector<CEnvironment*>maps;
};

