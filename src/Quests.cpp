#include "stdafx.h"
#include "Quests.h"

Quests::Quests(string p_quest_type, string p_quest_desc, int p_gold, int p_xp, int p_level_threshold, int p_item_threshold){
	quest_type = p_quest_type;
	quest_desc = p_quest_desc;
	gold = p_gold;
	xp = p_xp;
	level_threshold = p_level_threshold;
	item_threshold = p_item_threshold;
	quest_accepted = false;
	quest_display = true;
}

Quests::Quests(void){
	quest_accepted = false;
	quest_display = true;
}


Quests::~Quests(void){

}

void Quests::SetQuestXP (int p_xp){
	xp = p_xp;
}

void Quests::SetQuestGold(int p_gold){
	gold = p_gold;
}

void Quests::SetQuestDesc(string p_quest_desc){
	quest_desc = p_quest_desc;
}
void Quests::SetQuestAccept(bool accept){
	quest_accepted = accept;
}

int Quests::GetQuestGold(){
	return gold;
}

int Quests::GetQuestXP(){
	return xp;
}

string Quests::GetQuest(){
	return quest_desc;
}

bool Quests::GetQuestAccept(){
	return quest_accepted;
}

Text Quests::makeTextObj(int questRef, int x, int y){
	quest_text = new Text(x, y, GetQuest());
	return *quest_text;
}

//a file should be read to check the completion features of a quest, for example
//a file should read: kills:5, collect:sword_red, etc.
bool Quests::CheckQuestComplete(int questRef, BattleManager *battlemanager){
	if (questRef == 1){
		//Found out how many enemies have been killed.
		if (battlemanager->GetKillCount() >= 5 && quest_display == true){
			quest_accepted = false;
			//remove quest from questlist, display message saying you have completed a quest, 
			//make player stop and bring up dialog box that acknowledges the success!
			quest_complete = true;

			return quest_complete;
		}
		else quest_complete = false;
		return quest_complete;
	}
}

void Quests::QuestSuccess(vector<Quests*>quests, int quest_id, MainCharacter* player, CSprite* player_sprite, CSprite* dia_box_sprite, 
								CSprite* en_hp, CInteraction* interaction, CEnvironment* pub, CSprite* dialog_icon, vector<CEnemy*>enemies, EventManager* events){
									vector<Text>text;
									int temp_xp = quests[quest_id]->GetQuestXP();
									int temp_gold = quests[quest_id]->GetQuestXP();
									string quest_reward =+"You earned " + to_string(temp_xp) + " XP and " + to_string(temp_gold) + " gold";
									Text first_dialog(60, 400, "Quest complete!");
									Text second_dialog(60,420, quest_reward);

									Text third_dialog(60,440, "Press enter to continue.");
									text.push_back(first_dialog);
									text.push_back(second_dialog);
									
									//need an event that waits for an enter to be pressed, once done. Stop drawing this and stop this routine from being executed.
									//while interaction->WaitForEnter() draw the game and the quest success dialog, once user presses enter, set quest_display to false.
									while(!interaction->EnterPress()){
										events->DrawGame(player, player_sprite, en_hp, pub, enemies);
										events->QuestSuccessEvent(text, interaction, dialog_icon, dia_box_sprite, 10);
									}
									quest_display = false;
									
}

