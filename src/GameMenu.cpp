#include "GameMenu.h"



GameMenu::GameMenu(CSDL_Setup* passed_SDL_Setup, int *passed_MouseX, int *passed_MouseY,  float *passed_CameraX, float *passed_CameraY, int ScreenWidth, int ScreenHeight) {
	CameraX = passed_CameraX;
	CameraY = passed_CameraY;

	csdl_setup = passed_SDL_Setup;

	MouseX = passed_MouseX;
	MouseY = passed_MouseY;

	w = 0;
	h = 0;
	rangeX = 0;
	rangeY = 0;
	count = 0;
	prev_menu_choice_y = 220;

	keyEsc = false;
	keyPress = false;
	hasKeyBeenReleased = false;

	screenWidth = 0;
	screenHeight = 0;
	opt_select = 0;

	Dialog = new CSDLFont_Setup(csdl_setup);
	point = new Point();

	GameMenuTextObj = new Text();
}

GameMenu::~GameMenu(){

	delete Dialog;
	delete point;
	delete GameMenuTextObj;
	delete objOfQuests;
}

void GameMenu::CheckForEsc(CSDL_Setup* passed_CSDL_Setup){
	//has the user pressed esc? If so, let's start the menu.
	csdl_setup = passed_CSDL_Setup;
	if (csdl_setup->GetMainEvent()->key.keysym.sym == SDLK_ESCAPE){
		keyEsc = true;
		point->setPointVect(6, 245, 220, 245, 235, 245, 250);
		InitialiseGameMenu();
	}
}


bool GameMenu::IsEscPressed(){
	return keyEsc;
}

void GameMenu::InitialiseGameMenu(){
	GameMenuTextObj->eraseAllMessagesInVector();
	Text menuItem1(263, 215, "Return to Game");
	Text menuItem2(263, 230, "Inventory");
	Text menuItem3(263, 245, "Quests");
	Text menuItem4(263, 260, "Load/Save Game");
	Text menuItem5(263, 275, "Options");
	
	GameMenuTextObj->addToTextVector(menuItem1);
	GameMenuTextObj->addToTextVector(menuItem2);
	GameMenuTextObj->addToTextVector(menuItem3);
	GameMenuTextObj->addToTextVector(menuItem4);
	GameMenuTextObj->addToTextVector(menuItem5);
}

void GameMenu::initialiseYValues(){
	yValues.push_back(y);
	yValues.push_back(y+15);
	yValues.push_back(y+30);
	yValues.push_back(y+45);
	yValues.push_back(y+60);
}

void GameMenu::SetMenuGFX(CSprite* diaBoxSprite, CSprite* dialogIcon){
	//store the previous values for later usage when we want to reset where these icons etc were previously.
	GameMenuGFXVals.clear();
	GameMenuGFXVals.push_back(diaBoxSprite->GetX());
	GameMenuGFXVals.push_back(diaBoxSprite->GetY());
	GameMenuGFXVals.push_back(diaBoxSprite->GetWidth());
	GameMenuGFXVals.push_back(diaBoxSprite->GetHeight());
	GameMenuGFXVals.push_back(dialogIcon->GetX());
	GameMenuGFXVals.push_back(dialogIcon->GetY());

	//set the sprites so the menu gfx are in the right place.
	diaBoxSprite->SetX(240);
	diaBoxSprite->SetWidth(170);
	diaBoxSprite->SetHeight(150);
	diaBoxSprite->SetY(170);
	dialogIcon->SetX(250);
	dialogIcon->SetY(220);
}

void GameMenu::ResetMenuGFX(CSprite* diaBoxSprite, CSprite* dialogIcon){
	if (GameMenuGFXVals.size() == 6){
	diaBoxSprite->SetX(GameMenuGFXVals[0]);
	diaBoxSprite->SetWidth(GameMenuGFXVals[1]);
	diaBoxSprite->SetHeight(GameMenuGFXVals[2]);
	diaBoxSprite->SetY(GameMenuGFXVals[3]);
	dialogIcon->SetX(GameMenuGFXVals[4]);
	dialogIcon->SetY(GameMenuGFXVals[5]);
	}
	else 
		cout << "Error 0: GameMenuGFXVector size incorrect, can't reset the vector" << endl;
}

void GameMenu::DisplayGameMenu(MainCharacter* player, CSprite* playerSprite, CSprite* diaBoxSprite, 
							   CSprite* enHP, CInteraction* interaction, CEnvironment* pub, CSprite* dialogIcon, vector<CEnemy*>Enemies, EntityManager* entities){

								   SetMenuGFX(diaBoxSprite, dialogIcon);
								   //Constantly draw menu for SDL
								   EventManager event(csdl_setup, MouseX, MouseY);
								   event.DrawGame(player, playerSprite, enHP, pub, Enemies);

								   //get option
								   CheckUserPress(dialogIcon, point->getPointVect(), entities);

								   //displays the text.
								   event.GameMenuEvent(GameMenuTextObj->getTextVect(), interaction, dialogIcon, diaBoxSprite, 9);

								   

								   csdl_setup->End();

								   //reset sprites back to previous location
								   //this routine will take the previous values it had and put them back in.
								   ResetMenuGFX(diaBoxSprite, dialogIcon);
}


bool GameMenu::CheckUserPress(CSprite* menu_choice, vector<int>coords, EntityManager* entities){
	vector<Quests*>quests = entities->GetQuests();

	opt_select = 0;

	menu_choice->SetY(prev_menu_choice_y);

	switch (csdl_setup->GetMainEvent()->type)
	{
	case SDL_KEYDOWN:

		switch(csdl_setup->GetMainEvent()->key.keysym.sym){
		case SDLK_DOWN:
			switch((int)menu_choice->GetY()){
			case 220:
				menu_choice->SetY(menu_choice->GetY() +15);
				hasKeyBeenReleased = false;
				break;
			case 235:
				if (hasKeyBeenReleased == true)
					menu_choice->SetY(menu_choice->GetY() +15); 
				hasKeyBeenReleased = false;
				break;
			case 250:
				if (hasKeyBeenReleased == true)
					menu_choice->SetY(menu_choice->GetY() +15); 
				hasKeyBeenReleased = false;
				break;
			case 265:
				if (hasKeyBeenReleased == true)
					menu_choice->SetY(menu_choice->GetY() +15); 
				hasKeyBeenReleased = false;
				break;
			}
		case SDLK_UP:
			switch((int)menu_choice->GetY()){
			case 280:
				if (hasKeyBeenReleased == true)
					menu_choice->SetY(menu_choice->GetY() -15);
				hasKeyBeenReleased = false;
				break;
			case 265:
				if (hasKeyBeenReleased == true)
					menu_choice->SetY(menu_choice->GetY() -15);
				hasKeyBeenReleased = false;
				break;
			case 250:
				if (hasKeyBeenReleased == true)
					menu_choice->SetY(menu_choice->GetY() -15);
				hasKeyBeenReleased = false;
				break;
			case 235:
				if (hasKeyBeenReleased == true)
					menu_choice->SetY(menu_choice->GetY() -15); 
				hasKeyBeenReleased = false;
				break;
			}


			break;
		case SDLK_RETURN:
			if (menu_choice->GetY() == coords[1])
			{
				cout << "You have pressed enter on first option" << endl;
				keyEsc = false;
				opt_select = 1;
			}

			if (menu_choice->GetY() == coords[3])
			{
				cout << "You have pressed enter on second option" << endl;
				for (int i = 0; i < 5; i++){
				g_GameMenuText.push_back("Inventory");
				}
				GameMenuTextObj->editAllMessagesInVector(5, g_GameMenuText, coords);
				opt_select = 2;
			}
			if (menu_choice->GetY() == coords[5])
			{
				cout << "You have pressed enter on third option" << endl;
				for (int i = 0; i < quests.size(); i++){
					if (entities->AnyQuestAccepted()){
						g_GameMenuText.push_back(quests[i]->GetQuest());
						GameMenuTextObj->eraseAllMessagesInVector();
						GameMenuTextObj->addToTextVector(quests[i]->makeTextObj(i, 245, 215));
					}
					else{
						Text noQuests(250, 215, "No Quests Available");
						GameMenuTextObj->eraseAllMessagesInVector();
						GameMenuTextObj->addToTextVector(noQuests);
					}
				}
				opt_select = 3;
			}
			break;
		case SDLK_ESCAPE:
				initialiseYValues();
				GameMenuTextObj->ResizeVector(5);
				g_GameMenuText.push_back("Return to Game");
				g_GameMenuText.push_back("Inventory");
				g_GameMenuText.push_back("Quests");
				g_GameMenuText.push_back("Load/Save Game");
				g_GameMenuText.push_back("Options");
				
				GameMenuTextObj->editAllMessagesInVector(5, g_GameMenuText, yValues);
				break;
			}
			break;
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
	prev_menu_choice_y = menu_choice->GetY();
	return opt_select;
}

void GameMenu::RemoveQuest(BattleManager* battlemanager, EntityManager* entities){
	vector<Quests*>quests = entities->GetQuests();
	for (int i = 0; i < quests.size(); i++){
		int quest_id = i;
		if (quests[i]->CheckQuestComplete(quest_id, battlemanager)){
			//TODO:
			entities->RemoveQuest(quest_id);
		}
	}
}