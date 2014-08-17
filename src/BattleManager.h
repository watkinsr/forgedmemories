#pragma once
#include "TextManager.h"
#include "SDL_Setup.h"
#include "SDL_Font_Setup.h"
#include "EventManager.h"
#include "Interaction.h"
#include "Enemies.h"
#include "SFXManager.h"

const int DIALOGXLOC = 50;
const int FIRSTLINEY = 400;
const int SECONDLINEY = 420;
const int THIRDLINEY = 440;
const string FONTFILE = "Fonts/Minecraftia.ttf";

class BattleManager
{
public:
	BattleManager(CSDL_Setup* passed_CSDL_Setup, int *passed_MouseX, int *passed_MouseY);
	~BattleManager(void);
	void HandleBattle(MainCharacter* player, CSprite* playerSprite, CSprite* diaBoxSprite, 
		CSprite* enHP, CInteraction* interaction, CEnvironment* pub, CSprite* dialogIcon, vector<CEnemy*>*Enemies, int enemyBattleNum);
	void InitBattleLog();
	void DoDamageToEnemy(MainCharacter* player, CSprite* playerSprite, CSprite* enHP, CEnvironment* pub, vector<CEnemy*>*enemies, int EnemyVectNum, SFXManager* sounds);
	void DoDamageToPlayer(MainCharacter* player, CSprite* playerSprite, CSprite* enHP, CEnvironment* pub, vector<CEnemy*>*enemies, int EnemyVectNum, SFXManager* sounds);
	void HandlePlayerAttack(MainCharacter* player, CSprite* playerSprite, CSprite* enHP, CEnvironment* pub, vector<CEnemy*> *enemies, SFXManager* sounds);
	void HandleEnemyMeleeAttack(MainCharacter* player, CSprite* playerSprite, CSprite* enHP, CEnvironment* pub, vector<CEnemy*> *enemies, SFXManager* sounds);
	void SetKillCount(int kill_amount);
	int GetKillCount();
	
private:
	TextManager* dialog;
	CSDL_Setup* csdl_setup;

	int *mouseX;
	int *mouseY;
	bool dialogSuccess;
	bool runOnce;
	bool fight;
	bool battle_dialogue;
	bool battleSuccessDialogue;
	bool EnterPressed;
	bool firstDialog;
	bool secondDialog;
	bool thirdDialog;
	bool DialogueEnd;
	bool allowedtoDraw;
	bool killSuccess;

	int enemynum;
	int count;
	int addedXPFromBattle;
	int HP;
	int xInt;
	int yInt;
	int kill_count;

	CSDLFont_Setup* text;
	vector<Text>dialogMsgs;
};

