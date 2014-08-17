#include "stdafx.h"
#include "NPCManager.h"


NPCManager::NPCManager(void)
{
}


NPCManager::~NPCManager(void)
{
}

//idea here is to set the file location based on the npc id.
//so id 0 = npc_0.jpg
//id 1 = npc_1.jpg
//etc
void NPCManager::SetFileLocation(int npc_id){
	string npc_fileloc = "res/data/npcs/npc_" + to_string(npc_id) + ".png";
}


////void NPCManager::LoadNPCS(){
////	//FileManagement filemanagement;
////	if (filemanagement.OpenFile("res/data/npcs.txt")){
////		string start_line = "--BEGIN NPC LOAD--";
////		//getline(LoadedFile, line)
////
////	}
////
////
////}