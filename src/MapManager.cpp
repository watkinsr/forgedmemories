#include "stdafx.h"
#include "MapManager.h"


MapManager::MapManager(void)
{
}


MapManager::~MapManager(void)
{
}

//split into LoadQuestFile() & ReadQuestFile()
//read a file called quests.txt
//the file should take the npc's location, the spritesheet file location, the qeust type (kill, speak to, collect, go to , multiple)LoadQuests(){
void MapManager::LoadQuests(){
	ifstream LoadedFile ("data/quests.txt");
	string line;

	int currentType = TypeNone;
	if (LoadedFile.is_open())
	{
		while (LoadedFile.good() )
		{
			getline(LoadedFile, line);
			if (line == "---BEGIN QUESTS---")
			{
				currentType = TypeQuest;
			}

			else if (line == "---END QUESTS---")
			{
				currentType = TypeNone;
			}
			else {
				if (currentType == TypeQuest)
				{
					//start reading in the quest data.
					istringstream iss(line);

					int npc_x = 0; 
					int npc_y = 0; 
					string spritesheet_loc;
					int npc_id;
					//QuestType questType;
					int gold_earned;
					int xp_earned;
					string quest_text;
					string PreviousWord = ""; 
					//check for X/Y loc, spritesheet loc, npc num, quest type, gold earned and xp earned. get quest text.
					//store them in variables to init quest objects.
					while (iss) 
					{ 
						string word; 
						iss >> word; 

						if (PreviousWord == "x:") { 
							npc_x = atoi( word.c_str() ); 
						} 

						if (PreviousWord == "y:") { 
							npc_y = atoi( word.c_str() ); 
						} 
						if (PreviousWord == "spritesheet_loc:"){
							spritesheet_loc = word;
						}
						if (PreviousWord == "npc_id:"){
							npc_id = atoi( word.c_str() ); 
						}
						if (PreviousWord == "quest_text:"){
							quest_text = word;
						}
						if (PreviousWord == "quest_type:"){
							//TODO:
						}
						if (PreviousWord == "gold_earned:"){
							gold_earned = atoi( word.c_str() );
						}
						if (PreviousWord == "xp_earned:"){
							xp_earned = atoi( word.c_str() );
							// Quests *quest = new Quests(xp_earned, gold_earned, quest_text);
							//NPC *npc = new NPC(npc_x, npc_y, CameraX, CameraY, csdl_setup, *quest, npc_id, NPCAssets);
							//npcs.push_back(npc);
						}
						PreviousWord = word; 
					} 


				} 
			}

		}
	}
}

void MapManager::LoadNPCs(){
	
	

}
