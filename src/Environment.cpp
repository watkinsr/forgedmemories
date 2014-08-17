#include "Environment.h"

const int MAPONE = 1;
const int MAPTWO = 2;
const int MAPTHREE = 3;

CEnvironment::CEnvironment(){

}

CEnvironment::CEnvironment(int ScreenWidth, int ScreenHeight, float *passed_CameraX, float *passed_CameraY, string mapLoc, CSDL_Setup* passed_csdl_setup, 
						   int passed_TileSizeX, int passed_TileSizeY, int passed_TexSizeX, int passed_TexSizeY, bool tables, bool trees, int passed_LevelNum){
	csdl_setup = passed_csdl_setup;
	TileSizeX = passed_TileSizeX;
	TileSizeY = passed_TileSizeY;

	CameraX = passed_CameraX;
	CameraY = passed_CameraY;
	levelNum = passed_LevelNum;

	LoadEnvFile();
	StoolY = 350;

	for (int i = 0; i < TileSizeX; i++){
		for (int j = 0; j < TileSizeY; j++){
			maps.push_back(new CMap(i,j, TileSizeX, TileSizeY, passed_TexSizeX, passed_TexSizeY, mapLoc,CameraX, CameraY, csdl_setup, tables, trees, StoolY - 10));
		}
	}


	GameMode = GamePlay;


	onePressed = false;

}

CEnvironment::~CEnvironment(){
	for (vector<CMap*>::iterator i = maps.begin(); i != maps.end(); ++i)
	{
		delete (*i);
	}

	//clear all from memory
	for (vector<Tree*>::iterator i = trees.begin(); i != trees.end(); ++i)
	{
		delete (*i);
	}

	//then clear the vector
	trees.clear();
	maps.clear();
}

//if a specific level, we need to init a vector of loot objects, push back the loot for each level into that vector, then return that vector.
vector<Loot*> CEnvironment::GetLootForMap(){
	switch (levelNum)
	{
	case MAPONE:
		break;
	case MAPTWO:
		break;
	case MAPTHREE:
		break;
	}
	return mapLoot;
}

int CEnvironment::GetMapSizeX(){
	return TileSizeX;
}

int CEnvironment::GetMapSizeY(){
	return TileSizeY;
}

void CEnvironment::DrawBack(){
	for (vector<CMap*>::iterator i = maps.begin(); i != maps.end(); ++i){
		(*i)->DrawTexture();
		(*i)->DrawTable();
		(*i)->DrawStool();
	}
}

void CEnvironment::DrawFront(){
	for (vector<Tree*>::iterator i = trees.begin(); i != trees.end(); ++i){
		(*i)->DrawCrown();
	}
}

void CEnvironment::LoadEnvFile(){
	ifstream LoadedFile ("data/Map.txt");

	string line;

	enum Obj_Type {
		TypeNone,
		TypeTree,
		TypeLoot
	};

	int currentType = TypeNone;


	if (LoadedFile.is_open())
	{
		while (LoadedFile.good() )
		{
			getline(LoadedFile, line);
			if (line == "----====TREE SETTINGS====----")
			{
				currentType = TypeTree;
			}
			if (line == "----====LOOT SETTINGS====----")
			{
				currentType = TypeLoot;
			}
			if (line == "----====QUEST SETTINGS====----")
			{
				currentType = TypeTree;
			}
			else if (line == "----====END TREE====----")
			{
				currentType = TypeNone;
			}
			else {
				if (currentType == TypeTree)
				{

					istringstream iss(line);

					int TempX = 0; 
					int TempY = 0; 
					string PreviousWord = ""; 

					while (iss) 
					{ 
						string word; 
						iss >> word; 

						if (PreviousWord == "x:") 
						{ 
							TempX = atoi( word.c_str() ); 
						} 

						if (PreviousWord == "y:") 
						{ 
							TempY = atoi( word.c_str() ); 

							trees.push_back(new Tree(TempX, TempY,CameraX, CameraY, csdl_setup)); 
						} 

						PreviousWord = word; 
					} 


				} 

				//needs to check what type it is, where it is (x and y) and levelOfLoot.
				if (currentType == TypeLoot)
				{

					istringstream iss(line);

					int locationX = 0;
					int locationY = 0; 
					int rewardLevel = 0; 
					string type = "";

					string previousWord = ""; 

					while (iss) 
					{ 
						string word; 
						iss >> word; 

						//for instance, could be a chest, a sword, a shield, some gold, a scroll, a map, some player accessories.
						if (previousWord == "type:")
						{
							type = previousWord;

						}
						if (previousWord == "x:") 
						{ 
							locationX = atoi( word.c_str() ); 
						} 

						if (previousWord == "y:") 
						{ 
							locationY = atoi( word.c_str() ); 

							
						} 
						if (previousWord == "rewardLevel") 
						{
							//how good the loot is.
							rewardLevel = atoi(word.c_str() );

							//lets push back a loot object once we have reached the last 'item' or field for that piece of loot.
							mapLoot.push_back(new Loot(type, locationX, locationY, rewardLevel, csdl_setup, CameraX, CameraY)); 
						}
						previousWord = word; 
					} 

				}
			} 
		} 
	} 
	else{ 
		cout << "File Could Not Be Opened: Map.txt" << endl; 
	} 
} 

void CEnvironment::FileSave(){
	ofstream LoadedFile;
	LoadedFile.open("data/Map.txt");
	LoadedFile << "----====TREE SETTINGS====----" << endl;
	for (vector<Tree*>::iterator i = trees.begin(); i != trees.end(); ++i){
		LoadedFile << "x: " << (*i)->GetX() << "\ty: " << (*i)->GetY() << endl;
		(*i)->DrawCrown();
	}
	LoadedFile << "----====TREE END====----" << endl;
	LoadedFile.close();


	cout << "Level Saved!" << endl;


}

void CEnvironment::Update(){
	if (GameMode == LevelCreation){
		if (csdl_setup->GetMainEvent()->type == SDL_KEYDOWN){
			if (!onePressed && csdl_setup->GetMainEvent()->key.keysym.sym == SDLK_F11){
				FileSave();
				onePressed = true;
			}
		}

		if (csdl_setup->GetMainEvent()->type == SDL_KEYUP){
			if (onePressed && csdl_setup->GetMainEvent()->key.keysym.sym == SDLK_F11)
			{
				onePressed = false;
			}
		}

		if (csdl_setup->GetMainEvent()->type == SDL_KEYDOWN){
			if (!onePressed && csdl_setup->GetMainEvent()->key.keysym.sym == SDLK_F1)
			{
				trees.push_back(new Tree(-*CameraX+275,-*CameraY+107, CameraX, CameraY, csdl_setup));
				onePressed = true;
			}
		}

		if (csdl_setup->GetMainEvent()->type == SDL_KEYUP){
			if (onePressed && csdl_setup->GetMainEvent()->key.keysym.sym == SDLK_F1)
			{
				onePressed = false;
			}
		}
	}

	if (csdl_setup->GetMainEvent()->type == SDL_KEYDOWN){
		if (!onePressed && csdl_setup->GetMainEvent()->key.keysym.sym == SDLK_F2)
		{
			if (trees.size() > 0)
			{

				delete trees[trees.size()-1];

				trees.pop_back();

			}

			onePressed = true;
		}
	}

	if (csdl_setup->GetMainEvent()->type == SDL_KEYUP){
		if (onePressed && csdl_setup->GetMainEvent()->key.keysym.sym == SDLK_F2){
			onePressed = false;
		}
	}

	if (csdl_setup->GetMainEvent()->type == SDL_KEYDOWN){
		if (!onePressed && csdl_setup->GetMainEvent()->key.keysym.sym == SDLK_F12){
			if (GameMode == LevelCreation){
				cout << "Level Creation: OFF" << endl;
				GameMode = GamePlay;
			}
			else if (GameMode == GamePlay){
				cout << "Level Creation: ON" << endl;
				GameMode = LevelCreation;
			}
			onePressed = true;
		}
	}

	if (csdl_setup->GetMainEvent()->type == SDL_KEYUP){
		if (onePressed && csdl_setup->GetMainEvent()->key.keysym.sym == SDLK_F12){
			onePressed = false;
		}
	}

}