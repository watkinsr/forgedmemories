#include "stdafx.h"
#include "NPC.h"

//need to determine what NPC png to load
//Not sure if we need cameraX/cameraY as these are set to 0 in gameloop.cpp..
NPC::NPC(int p_x, int p_y, int p_npc_id, string p_npc_sprite_loc, string p_name, float* p_camera_x, float* p_camera_y, CSDL_Setup* p_csdl_setup, vector<Quests*>quests){
	x = p_x;
	y = p_y;
	csdl_setup = p_csdl_setup;
	const int NPCSPRITEWIDTH = 50;
	const int NPCSPRITEHEIGHT = 50;
	const int NPCINTERACTSYMBOLX = x+40;
	const int NPCINTERACTSYMBOLY = y-20;
	CameraX = p_camera_x;
	CameraY = p_camera_y;
	file_loc = p_npc_sprite_loc;
	npcSprite = new CSprite(csdl_setup->GetRenderer(),file_loc,x,y,NPCSPRITEWIDTH,NPCSPRITEHEIGHT,CameraX, CameraY,CCollisionRect());
	npc_hover_interact = new CSprite(csdl_setup->GetRenderer(),"res/data/NPCS/npc_hover_interact.png",NPCINTERACTSYMBOLX,NPCINTERACTSYMBOLY,6,23,CameraX, CameraY,CCollisionRect());
	NPCTextObj = new Text();
	numNPC = p_npc_id;
	NPCQuest = *quests[numNPC];
	hasKeyBeenReleased = false;
	optSelect = 0;
	isQuestAdded = false;
	firstPress = false;
	isPlayerClose = false;
	exitNPCDialog = false;
	countEnterPress = 0;
}

NPC::NPC(int x_passed, int y_passed, int *passed_MouseX, int *passed_MouseY, float *passed_CameraX, float *passed_CameraY, int passed_screenWidth, int passed_screenHeight, CSDL_Setup* passed_csdl_setup, int NPCref){
	x = x_passed;
	y = y_passed;
	CameraX = passed_CameraX;
	CameraY = passed_CameraY;
	MouseX = passed_MouseX;
	MouseY = passed_MouseY;
	csdl_setup = passed_csdl_setup;
	screenWidth = passed_screenWidth;
	screenHeight = passed_screenHeight;
	numNPC = NPCref;
	GetNpcFileLocation();
	npcSprite = new CSprite(csdl_setup->GetRenderer(),NPCAssets[NPCref],300,350,50,50,CameraX,CameraY, CCollisionRect());

	hasKeyBeenReleased = false;
	optSelect = 0;
	isQuestAdded = false;
	firstPress = false;
	isPlayerClose = false;
	exitNPCDialog = false;
	countEnterPress = 0;
	NPCTextObj = new Text();
}

NPC::~NPC(void){
	delete npcSprite;
	delete npc_hover_interact;
}

int NPC::GetNPCX(){
	return npcSprite->GetSpriteCameraX();
}

int NPC::GetNPCY(){
	return npcSprite->GetSpriteCameraY();
}
string NPC::GetQuestDesc(){
	return NPCQuest.GetQuest();
}

Quests NPC::GetQuest(){
	return NPCQuest;
}

void NPC::Draw(){
	npcSprite->Draw();
	npc_hover_interact->Draw();
}

CSprite NPC::GetNPCSprite(){
	return *npcSprite;
}

bool NPC::isPlayerCloseToGetQuest(Quests *objOfQuests, vector<NPC> vectOfNPCS, CSprite* player){
	for (unsigned int i = 0; i < vectOfNPCS.size(); i++)
	{
		int playerLocX = player->GetSpriteCameraX();
		int playerLocY = player->GetSpriteCameraY();
		int NPCLocX = vectOfNPCS[i].npcSprite->GetSpriteCameraX();
		int NPCLocY = vectOfNPCS[i].npcSprite->GetSpriteCameraY();

		if (player->GetSpriteCameraX() - vectOfNPCS[i].npcSprite->GetSpriteCameraX() < 60 && player->GetSpriteCameraX() - vectOfNPCS[i].npcSprite->GetSpriteCameraX() > -60 
			&& player->GetSpriteCameraY() - vectOfNPCS[i].npcSprite->GetSpriteCameraY() < 60 && player->GetSpriteCameraY() - vectOfNPCS[i].npcSprite->GetSpriteCameraY() > -60 && !exitNPCDialog){
				//get quest, pick NPC by identifying who is closest (current i identifies this!), then give user option to accept or decline the quest in a menu.
				InitialiseQuestMenu(i, objOfQuests);
				isPlayerClose = true;
				return isPlayerClose;
		}

	}
	return isPlayerClose;
}


void NPC::GetNpcFileLocation(){
	//set for testing purposes.
	level_num = 1;
	if (level_num == 1 && numNPC == 0){
		NPCAssets.push_back("res/data/environment/pub/NPCs/NPC1.png");

		//meant for another function
		///*Quests quest01;
		//quest01.setQuestDesc("Kill 5 enemies");
		//NPC Joe_Pub(200,200,CameraX, CameraY, csdl_setup, quest01,  NPCAssets, 0);
		//vectOfNPCS.push_back(Joe_Pub); */


	}
	if (NPCAssets.size() == 0){
		cout << "Failed to add NPC File location, vector size is 0" << endl;
	}
}

//this function should inititalise a generic quest menu.
void NPC::InitialiseQuestMenu(int npc_id, Quests *quest){
		NPCTextObj->eraseAllMessagesInVector();
		Text menuItem1(50, 400, quest->GetQuest());
		Text menuItem2(50, 420, "Accept Quest");
		Text menuItem3(50, 440, "Decline Quest");
		NPCTextObj->addToTextVector(menuItem1);
		NPCTextObj->addToTextVector(menuItem2);
		NPCTextObj->addToTextVector(menuItem3);
	}

//take a group of sprites to draw later along with npc id/quest vector to get the menu dialog.
void NPC::DisplayQuestMenu(int npc_id, vector<Quests*>quests, MainCharacter* player, CSprite* playerSprite, CSprite* diaBoxSprite, 
						   CSprite* enHP, CInteraction* interaction, CEnvironment* pub, CSprite* dialogIcon, vector<CEnemy*>Enemies){
							   if (quests.size() != 0){
								   InitialiseQuestMenu(npc_id, quests[npc_id-1]);

								   //while the quest has not been added, display the quest menu.
								   while (!exitNPCDialog){

									   EventManager event(csdl_setup, MouseX, MouseY);
									   event.DrawGame(player, playerSprite, enHP, pub, Enemies);

									   //displays the text.
									   event.NPCDialogEvent(NPCTextObj->getTextVect(), interaction, dialogIcon, diaBoxSprite, npcSprite);	

									   //get option
									   CheckUserPress(dialogIcon, npc_id, quests[npc_id-1]);
								   }
							   }
}

//check the user input, i.e. has the user pressed down or up through the menus, enter to get through the selections.
int NPC::CheckUserPress(CSprite* menu_arrow_sprite, int NPCRefNumber, Quests* quest){
	//disable the option to move up/down dependent on whether you have pressed accept for second opt.
	optSelect = 0;
	switch (csdl_setup->GetMainEvent()->type)
	{
	case SDL_KEYDOWN:

		switch(csdl_setup->GetMainEvent()->key.keysym.sym){
		case SDLK_RETURN:

			cout << "You have pressed enter : " << countEnterPress << " times" << endl;
			if (menu_arrow_sprite->GetY() == FIRSTCHOICE)
			{
				cout << "You have pressed enter on first option" << endl;
				optSelect = 1;
			}

			if (menu_arrow_sprite->GetY() == SECONDCHOICE && countEnterPress == 0)// && !hasKeyBeenReleased)
			{				
				cout << "You have pressed enter on second option" << endl;
				Text questAccepted(50, 430, "Quest was added to list of quests.");
				Text pressEnterToContinue(50, 445, "Press enter to continue.");
				NPCTextObj->eraseAllMessagesInVector();
				NPCTextObj->addToTextVector(questAccepted);
				NPCTextObj->addToTextVector(pressEnterToContinue);
				isQuestAdded = true;
				//Edit the vector that contains all quests saying a quest has been accepted from an NPC.
				quest->SetQuestAccept(true);
				optSelect = 2;
			}
			if (hasKeyBeenReleased){
				countEnterPress++;}
			//While the user has not pressed enter to close menu, keep it open. Set a flag to make sure user has to press enter to close.
			if (csdl_setup->GetMainEvent()->key.keysym.sym == SDLK_RETURN && countEnterPress == 1)
			{
				exitNPCDialog = true;
			}
			if (menu_arrow_sprite->GetY() == THIRDCHOICE && countEnterPress == 0)
			{
				cout << "You have pressed enter on third option" << endl;
				Text questDeclined(50, 430, "You declined the quest");
				Text pressEnterToContinue(50, 445, "Press enter to continue.");;
				NPCTextObj->eraseAllMessagesInVector();
				NPCTextObj->addToTextVector(questDeclined);
				NPCTextObj->addToTextVector(pressEnterToContinue);
				optSelect = 3;
			}
			break;
		}



	case SDL_KEYUP:
		switch(csdl_setup->GetMainEvent()->key.keysym.sym)
		{
		case SDLK_RETURN:
			hasKeyBeenReleased = true;
			break;
		case SDLK_DOWN:
			hasKeyBeenReleased = true;
			break;
		case SDLK_UP:
			hasKeyBeenReleased = true;
			break;
		}
		break;
	}

	return optSelect;
}
