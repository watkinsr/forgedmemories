#include "stdafx.h"
#include "BattleManager.h"

const int ATTACKRANGE = 40;

BattleManager::BattleManager(CSDL_Setup* passed_CSDL_Setup, int *passed_MouseX, int *passed_MouseY){
	csdl_setup = passed_CSDL_Setup;
	mouseX = passed_MouseX;
	mouseY = passed_MouseY;

	dialogSuccess = false;
	runOnce = false;
	EnterPressed = false;
	firstDialog = false;
	secondDialog = false;
	thirdDialog = false;
	battleSuccessDialogue = false;
	battle_dialogue = false;
	DialogueEnd = false;
	allowedtoDraw = true;
	text = new CSDLFont_Setup(csdl_setup);
	dialog = new TextManager(csdl_setup);
	kill_count = 0;
	enemynum = 0;
	battleSuccessDialogue = true;
	killSuccess = false;
}


BattleManager::~BattleManager(void){
	delete text;
	delete dialog;
}

//should create a vector of Text, that has the first line of battle log, the second line and the third.
//If the first dialog is true, then add to the vector the first dialog, however, if the second dialog has been activated, 
//replace the first dialog with the text for the second dialog and do the same process for the next dialog.
void BattleManager::InitBattleLog(){
	if (firstDialog){
		dialogMsgs.clear();
		Text* firstLine = new Text(DIALOGXLOC, FIRSTLINEY, "A wild enemy appeared!");
		Text* secondLine = new Text(DIALOGXLOC, SECONDLINEY, "Press enter to continue.");
		dialogMsgs.push_back(*firstLine);
		dialogMsgs.push_back(*secondLine);
	}
	if (secondDialog){
		dialogMsgs.clear();
		Text* firstLine = new Text(DIALOGXLOC, FIRSTLINEY, "What would you like to do?");
		Text* secondLine = new Text(DIALOGXLOC, SECONDLINEY, "Attack");
		Text* thirdLine = new Text(DIALOGXLOC, THIRDLINEY, "Flee");
		dialogMsgs.push_back(*firstLine);
		dialogMsgs.push_back(*secondLine);
		dialogMsgs.push_back(*thirdLine);
	}
	if (thirdDialog){
		dialogMsgs.clear();
		Text* firstLine = new Text(DIALOGXLOC, FIRSTLINEY, "Your attack was successful!");
		Text* secondLine = new Text(DIALOGXLOC, SECONDLINEY, "Press enter to continue.");
		dialogMsgs.push_back(*firstLine);
		dialogMsgs.push_back(*secondLine);
	}
}

void BattleManager::SetKillCount(int p_kill_amount){
	kill_count = p_kill_amount;
}

int BattleManager::GetKillCount(){
	return kill_count;
}

void BattleManager::HandleBattle(MainCharacter* player, Sprite* playerSprite, Sprite* diaBoxSprite,
		Sprite* enHP, CInteraction* interaction, CEnvironment* pub, Sprite* dialogIcon, vector<CEnemy*>*enemies, int enemyBattleNum){
		enemynum = enemyBattleNum;
		firstDialog = true;
		DialogueEnd = false;
		
		int enemyVectSize = enemies->size();
		EventManager event(csdl_setup, mouseX, mouseY);
		InitBattleLog();

		while (firstDialog == true)
		{
			dialog->DrawDialog(dialogMsgs, FONTFILE, text, 10);
			event.DrawGame(player, playerSprite, enHP, pub, *enemies);
			event.BattleDialogEvent(dialogMsgs, interaction, dialogIcon, diaBoxSprite);
			//get user press
			if (interaction->checkEnter(dialogIcon, 640, 480) == 1 || interaction->checkEnter(dialogIcon, 640, 480) == 2){
			secondDialog = true;
			firstDialog = false;
			}
		}
		InitBattleLog();
		while (secondDialog == true)
		{
			dialog->DrawDialog(dialogMsgs, FONTFILE, text,10);
			event.DrawGame(player, playerSprite, enHP, pub, *enemies);
			event.BattleDialogEvent(dialogMsgs, interaction, dialogIcon, diaBoxSprite);

					if (interaction->checkEnter(dialogIcon, 640, 480) == 1)
					{
						//DoDamageToEnemy(player, player->GetPlayerSprite(), enHP, pub, enemies, enemyBattleNum, sounds);
					}
						//this tests to see if the vector has dropped by 1, aka the enemy has been killed.
						if ((*enemies).size() != enemyVectSize)
						{

							secondDialog = false;
							thirdDialog = false;
							battleSuccessDialogue = true;
						}
					
					else if (dialogIcon->GetY() >= 446 && dialogIcon->GetY() <= 466)
					{
						//open invent
					}
		

			else if (thirdDialog == true)
			{
				//if (result)
				//{
					DialogueEnd = true;
				//}
			}
 		}
			while (battleSuccessDialogue == true && DialogueEnd == false)
			{
				string xp_str =	to_string(addedXPFromBattle);
				xp_str.append(" XP gained");
				cout << "XP for Player: " << player->GetXP() << endl;		
				//if (result)
				{
				kill_count++;
				cout << "Kill count now: " << kill_count << endl;
				battleSuccessDialogue = false;
				DialogueEnd = true;
					
				}
			}
		}

//if the player is in the right place to hit and space is pressed, then do damage.
void BattleManager::HandlePlayerAttack(MainCharacter* player, Sprite* playerSprite, Sprite* enHP, CEnvironment* pub, vector<CEnemy*> *enemies, SFXManager* sounds){
	count = 0;
	float playerX = (float)playerSprite->GetSpriteCameraX() - 20.0f;
	float playerY = (float)playerSprite->GetSpriteCameraY() - 40.0f;
	

	for (vector<CEnemy*>::iterator i = (*enemies).begin(); i != (*enemies).end(); ++i)
	{
		float enemyX = (float)(*i)->GetSpriteCameraX();
		float enemyY = (float)(*i)->GetSpriteCameraY();

		float rangeBetweenX = playerX - enemyX;
		float rangeBetweenY = playerY - enemyY;
		count++;
		if (enemyX-playerX < ATTACKRANGE && enemyX-playerX > 0 && enemyY-playerY < 40 && enemyY-playerY > -40){
			switch (csdl_setup->GetMainEvent()->type)
			{
			case SDL_KEYDOWN:

				switch(csdl_setup->GetMainEvent()->key.keysym.sym){
				case SDLK_SPACE:
					if(count!= 1){
						cout << "the EnemyVectNumber is wrong!" << endl;
					}
					if (!killSuccess){
						enemynum = count-1;
						DoDamageToEnemy(player, playerSprite, enHP, pub,enemies, enemynum, sounds);
					}
				}
			}
		}
		
		if (killSuccess){
			kill_count++;
			killSuccess = false;
			break;
		}
	}
}

//if the enemy is to the left of player, then take player damage.
void BattleManager::HandleEnemyMeleeAttack(MainCharacter* player, Sprite* playerSprite, Sprite* enHP, CEnvironment* pub, vector<CEnemy*> *enemies, SFXManager* sounds){
	count = 0;
	float player_x = (float)playerSprite->GetSpriteCameraX() - 20.0f;
	float player_y = (float)playerSprite->GetSpriteCameraY() - 40.0f;
	for (vector<CEnemy*>::iterator i = (*enemies).begin(); i != (*enemies).end(); ++i){
		float enemy_x = (float)(*i)->GetSpriteCameraX();
		float enemy_y = (float)(*i)->GetSpriteCameraY();

		float range_x = player_x - enemy_x;
		float range_y = player_y - enemy_y;
		count++;
		if (range_x < ATTACKRANGE && range_x > 0 && range_y < 40 && range_y > -40){
			DoDamageToPlayer(player, playerSprite, enHP, pub,enemies, enemynum, sounds);
		}
	}
}

void BattleManager::DoDamageToEnemy(MainCharacter* player, Sprite* playerSprite, Sprite* enHP, CEnvironment* pub, vector<CEnemy*>*enemies, int EnemyVectNum, SFXManager* sounds){
	player->DisplayAttackAnim();
	sounds->SwordAttack();
	player->GetLevelStats();
	int dmg = player->GetDamage();

 	if((*enemies)[EnemyVectNum]->getHP() >= 0){
		(*enemies)[EnemyVectNum]->setHP(-dmg);
		(*enemies)[EnemyVectNum]->changeHP((*enemies), EnemyVectNum, player->GetDamage());
	}

	if ((*enemies)[EnemyVectNum]->getHP() <= 0){
		allowedtoDraw = false;
		killSuccess = true;
	}
	if (!allowedtoDraw) {
		allowedtoDraw = true;
		player->AddXP((*enemies)[EnemyVectNum]->GetXP());
		addedXPFromBattle = (*enemies)[EnemyVectNum]->GetXP();
 		(*enemies).erase((*enemies).begin() + EnemyVectNum);
	}

	string dmg_str = std::to_string(dmg);
	Text dmgtext(250, 178, dmg_str);
	vector<Text>tempTextVect;
	tempTextVect.push_back(dmgtext);
	CSDLFont_Setup font(csdl_setup);
	float time = (float)SDL_GetTicks();
	do{
	EventManager* event = new EventManager(csdl_setup, mouseX, mouseY);
	event->DrawGame(player, playerSprite, enHP, pub, *enemies);
	dialog->DrawDialog(tempTextVect, FONT_FILE, &font, 15);
	csdl_setup->End();
	delete event;
	}while (SDL_GetTicks() < time + 300);
}

void BattleManager::DoDamageToPlayer(MainCharacter* player, Sprite* player_sprite, Sprite* en_hp, CEnvironment* pub, vector<CEnemy*>*enemies, int enemy_id, SFXManager* sounds){
	int damage = (*enemies)[enemy_id]->getDamage();
	if(player->GetHP() >= 0){
		player->SetHP(-damage);
		// TODO: EDIT PLAYERS HP SPRITE
	}
	if (player->GetHP() <= 0){
	//if player is dead, display game over, insert coins to play or load game.
	}

	string damage_str = std::to_string(damage);
	//moving the text further to the right for testing purposes, need to adjust.
	Text damage_text(400, 178, damage_str);
	vector<Text>temp_text_vect;
	temp_text_vect.push_back(damage_text);
	CSDLFont_Setup font(csdl_setup);
	float time = (float)SDL_GetTicks();
	//draw the game and the text for 300ms.
	do{
	EventManager* event = new EventManager(csdl_setup, mouseX, mouseY);
	event->DrawGame(player, player_sprite, en_hp, pub, *enemies);
	HandlePlayerAttack(player, player_sprite, en_hp, pub, enemies, sounds);
	dialog->DrawDialog(temp_text_vect, FONT_FILE, &font, 15);
	csdl_setup->End();
	delete event;
	}while (SDL_GetTicks() < time + 300);
}
