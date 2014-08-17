#include "stdafx.h"
#include "EventManager.h"


EventManager::EventManager(CSDL_Setup* passed_CSDL_Setup, int *passed_MouseX, int *passed_MouseY){
	csdl_setup = passed_CSDL_Setup;
	mouseX = passed_MouseX;
	mouseY = passed_MouseY;
	show_once = false;
}


EventManager::~EventManager(void){
}

//might want to think about grouping entities such as sprites into a sprite struct.
void EventManager::ChestDialogEvent(MainCharacter* player, CSprite* playerSprite, CSprite* diaBoxSprite, 
						CSprite* enHP, CInteraction* interaction, CEnvironment* pub, CSprite* dialogIcon, vector<CEnemy*>Enemies){

						DrawGame(player, playerSprite, enHP, pub, Enemies);
						diaBoxSprite->DrawSteady();
						//dialogIcon->DrawSteady();
						TextManager dialog(csdl_setup);
						vector<Text>textVect;
						Text text1(DIALOGSTARTPOSX, DIALOGSTARTPOSY, "You have opened a chest!");
						textVect.push_back(text1);
						CSDLFont_Setup font(csdl_setup);
						dialog.DrawDialog(textVect, FONT_FILE, &font, 10);	
						//interaction->checkInteraction(dialogIcon);
						csdl_setup->End();

}

void EventManager::BattleDialogEvent(vector<Text>text, CInteraction* interaction, CSprite* dialogIcon, CSprite* diaBoxSprite){
	diaBoxSprite->DrawSteady();
	dialogIcon->DrawSteady();
	TextManager* dialog = new TextManager(csdl_setup);
	CSDLFont_Setup* font = new CSDLFont_Setup(csdl_setup);
	dialog->DrawDialog(text, FONT_FILE, font, 10);
	interaction->checkInteraction(dialogIcon);
	csdl_setup->End();
	delete dialog;
	delete font;
}

void EventManager::NPCDialogEvent(vector<Text>text, CInteraction* interaction, CSprite* dialogIcon, CSprite* diaBoxSprite, CSprite* NPC){
	diaBoxSprite->DrawSteady();
	dialogIcon->DrawSteady();
	NPC->Draw();
	TextManager* dialog = new TextManager(csdl_setup);
	CSDLFont_Setup* font = new CSDLFont_Setup(csdl_setup);
	dialog->DrawDialog(text, FONT_FILE, font, 10);
	interaction->checkInteraction(dialogIcon);
	csdl_setup->End();
	delete dialog;
	delete font;
}
//need to show quest success for either 2s and once or await user to press enter on (Press Enter to Continue) to quit the success log.
//may be worthwhile having a routine in interaction that has takes care of the enter part.


void EventManager::QuestSuccessEvent(vector<Text>text, CInteraction* interaction, CSprite* dialogIcon, CSprite* diaBoxSprite, int font_size){
	if (!show_once){
	show_once = true;
	diaBoxSprite->DrawSteady();
	dialogIcon->DrawSteady();
	TextManager* dialog = new TextManager(csdl_setup);
	CSDLFont_Setup* font = new CSDLFont_Setup(csdl_setup);
	dialog->DrawDialog(text, FONT_FILE, font, font_size);
	interaction->checkInteraction(dialogIcon);
	csdl_setup->End();
	delete dialog;
	delete font;
	}
}

void EventManager::GameMenuEvent(vector<Text>text, CInteraction* interaction, CSprite* dialogIcon, CSprite* diaBoxSprite, int font_size){
	diaBoxSprite->DrawSteady();
	dialogIcon->DrawSteady();
	TextManager* dialog = new TextManager(csdl_setup);
	CSDLFont_Setup* font = new CSDLFont_Setup(csdl_setup);
	dialog->DrawDialog(text, FONT_FILE, font, font_size);
	interaction->checkInteraction(dialogIcon);
	csdl_setup->End();
	delete dialog;
	delete font;
}

 //should draw all the loot also.
void EventManager::DrawGame(MainCharacter* player, CSprite* playerSprite, CSprite* enHP, CEnvironment* pub,vector<CEnemy*>enemies){
						csdl_setup->Begin();
						SDL_GetMouseState(mouseX,mouseY);
						pub->DrawBack();
						player->Draw();
						player->Update(enemies, pub->GetMapElements());
						//TODO: You may not want to stop the player when you draw all the game elements..
						//player->StopMove();
						pub->Update();
						pub->DrawFront();
						for (vector<CEnemy*>::iterator i = enemies.begin(); i != enemies.end(); ++i)
						{
							(*i)->Draw();
						}
						enHP->Draw();


}