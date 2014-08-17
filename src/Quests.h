#pragma once
#ifndef QUESTS_H
#define QUESTS_H

#include <vector>
#include "Text.h"
#include "BattleManager.h"
#include "EventManager.h"

using namespace std;

class Quests
{
public:
	Quests(string p_quest_type, string p_quest_desc, int p_gold, int p_xp, int p_level_threshold, int p_item_threshold);
	Quests(void);
	~Quests(void);
	

	void SetQuestXP (int p_xp);
	void SetQuestGold(int p_gold);
	void SetQuestDesc(string p_quest_desc);
	void SetQuestAccept(bool accept);
	int GetQuestGold();
	int GetQuestXP();
	bool GetQuestAccept();
	string GetQuest();
	Text makeTextObj(int questRef, int x, int y);
	bool CheckQuestComplete(int questRef, BattleManager *battlemanager);
	void QuestSuccess(vector<Quests*>quests, int quest_id, MainCharacter* player, CSprite* player_sprite, CSprite* dia_box_sprite, 
								CSprite* en_hp, CInteraction* interaction, CEnvironment* pub, CSprite* dialog_icon, vector<CEnemy*>enemies, EventManager* events);

private:
	int xp;
	int gold;
	int level_threshold;
	int item_threshold;
	string quest_type;

	bool quest_complete;
	bool quest_accepted;
	bool quest_display;
	bool questAlreadyCompleted;

	Text* quest_text;
	string quest_desc;
};

#endif