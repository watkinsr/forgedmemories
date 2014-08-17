#include "GameLoop.h"

GameLoop::GameLoop(int passed_ScreenWidth, int passed_ScreenHeight){
	CameraX = 0;
	CameraY = 0;
	MouseX = 0;
	MouseY = 0;
	ScreenWidth = passed_ScreenWidth;
	ScreenHeight = passed_ScreenHeight;
	
	csdl_setup = new CSDL_Setup(&quit, ScreenWidth, ScreenHeight);
	mainmenu = new CMainMenu(csdl_setup, &MouseX, &MouseY, ScreenWidth, ScreenHeight, &CameraX, &CameraY); //5mb
	pub = new CEnvironment(ScreenWidth, ScreenHeight, &CameraX, &CameraY, "data/environment/pub/floorbig.png", csdl_setup,2,2,640,480, true, false, 1);	//13mb
	player = new MainCharacter(csdl_setup, &MouseX, &MouseY, &CameraX, &CameraY, pub); //shouldn't even be taking in pub, it makes no sense.
	enemies = new CEnemies(csdl_setup,0,0, &MouseX, &MouseY, &CameraX, &CameraY, ScreenWidth, ScreenHeight); //60mb
	text = new CSDLFont_Setup(csdl_setup);
	quests = new Quests();
	battle = new CBattle(csdl_setup, &MouseX, &MouseY, &CameraX, &CameraY); 
	interaction = new CInteraction (csdl_setup,&MouseX, &MouseY, &CameraX, &CameraY);
	gamemenu = new GameMenu(csdl_setup ,&MouseX, &MouseY, &CameraX, &CameraY, ScreenWidth, ScreenHeight);
	npcs = new NPC(0, 0, &MouseX, &MouseY, &CameraX, &CameraY, ScreenWidth, ScreenHeight, csdl_setup, 0); 
	//testing this, really the values should be more clearcut as to what you are passing.
	loot = new Loot("chest", 0, 0, 0, csdl_setup, &CameraX, &CameraY);
	playermanager = new PlayerManager(csdl_setup,&CameraX, &CameraY);
	events = new EventManager(csdl_setup, &MouseX, &MouseY);
	battlemanager = new BattleManager(csdl_setup, &MouseX, &MouseY);
	sounds = new SFXManager();
	debug = new ConsoleDebug();
	filemanager = new FileManagement(csdl_setup, &CameraX, &CameraY);
	entities = new EntityManager();


	level_str = "LVL: ";
	hp_str = "HP: ";
	dmg_str = "DMG: ";
	xp_str = "XP: ";

	temp = "";

	quit = false;
	start = false;
}


GameLoop::~GameLoop(void){
	delete csdl_setup;
	delete player;
	delete mainmenu;
	delete pub;
	delete player;
	delete chest;
	delete enemies;
	delete text;
	delete battle;
	delete interaction;
	delete gamemenu;
	delete npcs;
	delete quests;
	delete playermanager;
	delete loot;
	delete events;
	delete debug;
	delete filemanager;
	delete entities;
}

void GameLoop::Begin(bool passed_start){
	enemies->SpawnEnemy(10, 30, 3, 100, pub);
	GetStats();
	LoadFiles();
	Play();	
}

//this routine takes all the objects created in the constructor and places 
//them individually into vectors for easy sprite management
void GameLoop::GroupEntities(){

}

//must load quests before npc's as npc's need a quest to be initialised.
void GameLoop::LoadFiles(){
filemanager->HandleQuestFile("data/quests.txt");
filemanager->HandleNPCFile("data/npcs.txt");
filemanager->HandleSpriteFile();
entities = filemanager->GetEntities();
}

void GameLoop::StartMenu(){
while (!quit && csdl_setup->GetMainEvent()->type != SDL_QUIT && start != true)
	{
		csdl_setup->Begin();
		SDL_GetMouseState(&MouseX,&MouseY);

		mainmenu->Draw();
		//if user presses start, start the game
		if (mainmenu->GetButtonClick() == 1)
		{
		start = true;
		}
		//if user presses options, get options
		else if (mainmenu->GetButtonClick() == 2)
		{

		}
		//if user presses about, get about
		else if (mainmenu->GetButtonClick() == 3)
		{
				while (mainmenu->AboutMenu() != true)
				{
					csdl_setup->Begin();
					SDL_GetMouseState(&MouseX,&MouseY);
					mainmenu->AboutMenu();
					csdl_setup->End();
				}
		}
		csdl_setup->End();
	}
}


//really, this should have loadMap() which inits vector of npcs, chests, other various entities such as enemies and so on.
void GameLoop::Play(){
	while (!quit && csdl_setup->GetMainEvent()->type != SDL_QUIT){
		csdl_setup->Begin();
		SDL_GetMouseState(&MouseX,&MouseY);
		enemies->PrintEnemyTestLoc();
		cout << "PlayerX: " << player->GetPlayerSprite()->GetSpriteCameraX() << endl;
		cout << "PlayerY: " << player->GetPlayerSprite()->GetSpriteCameraY() << endl;
		pub->DrawBack();
		player->Draw();
		player->Update(*enemies->GetEnemies(), pub->GetMapElements());
		pub->Update();
		pub->DrawFront();
		sounds->GetSceneMusic();

		//draws NPC's for the time being, should draw every form of sprite however.
		entities->DrawAll();

		//just to enable quest success.
		battlemanager->SetKillCount(5);

		const float LOOTRANGE = 60;
		if (playermanager->IsClose(loot->GetLoot(), LOOTRANGE)){
			loot->OpenChest();
			player->StopMove();
			//Sprites shouldn't come from battle, this is just temporary testing usage. There instead should be a class that deals with DialogSprites and returns them.
			events->ChestDialogEvent(player, player->GetPlayerSprite(), battle->GetDiaBoxSprite(), 
				battle->GetEnHP(), interaction, pub, battle->GetDialogIcon(), *enemies->GetEnemies());
		}

		if (entities->CheckPlayerClose(player->GetPlayerSprite())){
			npcs->DisplayQuestMenu(1, entities->GetQuests(), player, player->GetPlayerSprite(), 
				battle->GetDiaBoxSprite(), battle->GetEnHP(), interaction, pub, battle->GetDialogIcon(), *enemies->GetEnemies());
		}

		//should be draw all loot objects.
		loot->DrawChest();

		enemies->Draw();
		enemies->GetEnemies();
		enemies->DetectPlayer(player->GetPlayerSprite());

		battlemanager->HandlePlayerAttack(player, player->GetPlayerSprite(), battle->GetEnHP(), pub, enemies->GetEnemies(), sounds);
		battlemanager->HandleEnemyMeleeAttack(player, player->GetPlayerSprite(), battle->GetEnHP(), pub, enemies->GetEnemies(), sounds);

		//if quest finished. Get gamemenu object to remove the quest from the quests.
		if (quests->CheckQuestComplete(1, battlemanager)){
			quests->QuestSuccess(entities->GetQuests(), 0, player, player->GetPlayerSprite(), battle->GetDiaBoxSprite(), 
				battle->GetEnHP(), interaction, pub, battle->GetDialogIcon(), *enemies->GetEnemies(), events);
			gamemenu->RemoveQuest(battlemanager, entities);

		}

		DrawPlayerDebugStats();

		gamemenu->CheckForEsc(csdl_setup);
		if (gamemenu->IsEscPressed()){
			gamemenu->DisplayGameMenu(player, player->GetPlayerSprite(), battle->GetDiaBoxSprite(), 
				battle->GetEnHP(), interaction, pub, battle->GetDialogIcon(), *enemies->GetEnemies(), entities);
		}

		GetStats();
		csdl_setup->End();	
	}
}

void GameLoop::DrawPlayerDebugStats(){
	text->getText(xp_str, FONT_FILE, 300, 10, text->getColor(), 6, false, true, csdl_setup->GetRenderer());
	text->getText(level_str, FONT_FILE, 380, 10, text->getColor(), 6, false, true, csdl_setup->GetRenderer());
	text->getText(hp_str,	FONT_FILE, 460, 10, text->getColor(), 6, false, true, csdl_setup->GetRenderer());
	text->getText(dmg_str, FONT_FILE, 540, 10, text->getColor(), 6, false, true, csdl_setup->GetRenderer());
}

void GameLoop::GetStats(){
		//player->GetLevelStats();
		player->CheckLevelUp();
		damage = player->GetDamage();
		temp = to_string(damage);
		dmg_str.replace(4,5,temp);

		hp = player->GetHP();
		temp = to_string(hp);
		hp_str.replace(4,5,temp);

		level = player->GetLevel();
		temp = to_string(level);
		level_str.replace(4,5,temp);

		xp = player->GetXP();
		temp = to_string(xp);
		xp_str.replace(4,5,temp);
}