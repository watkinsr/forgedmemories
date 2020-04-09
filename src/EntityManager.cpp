#include "EntityManager.h"


EntityManager::EntityManager(void)
{
}


EntityManager::~EntityManager(void)
{
}

void EntityManager::AddQuests(Quests* quest){
	quests.push_back(quest);
}

void EntityManager::RemoveQuest(int quest_id){
	quests.erase(quests.begin()+quest_id);
}

bool EntityManager::AnyQuestAccepted(){
	for (unsigned int i = 0; i < quests.size(); i++){
		return quests[i]->GetQuestAccept();
	}

	return false;
}

void EntityManager::AddNPCs(NPC* npc){
	npcs.push_back(npc);
}

vector<Quests*> EntityManager::GetQuests(){
	return quests;
}

vector<NPC*> EntityManager::GetNPCs(){
	return npcs;
}

void EntityManager::AddSprites(Sprite* sprite){
	sprites.push_back(sprite);
}

vector<Sprite*> EntityManager::GetSprites(){
	return sprites;
}

void EntityManager::DrawAll(){
	for (vector<NPC*>::iterator i = npcs.begin(); i != npcs.end(); ++i){
		(*i)->Draw();
	}

}

bool EntityManager::CheckPlayerClose(Sprite* player){
	int count = 0;
	for (vector<NPC*>::iterator i = npcs.begin(); i != npcs.end(); ++i){
		count++;
		int player_x = player->GetSpriteCameraX();
		int player_y = player->GetSpriteCameraY();
		int npc_x = (*i)->GetNPCX();
		int npc_y = (*i)->GetNPCY();
		int diff_x = player_x - npc_x;
		int diff_y = player_y - npc_y;
		if (player_x - npc_x < 60 && player_x - npc_x > -60 
			&& player_y - npc_y < 60 && player_y - npc_y > -60){
				//get quest, pick NPC by identifying who is closest (current i identifies this!), then give user option to accept or decline the quest in a menu.
				//(*i)->InitialiseQuestMenu(count, quests[count-1]);
				return true;
		}
	}
	return false;
}
