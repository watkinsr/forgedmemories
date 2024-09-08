#pragma once
#include "Sprite.h"
#include "stdafx.h"
#include "SDL_Setup.h"
#include "Tree.h"
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include "Map.h"
#include "Loot.h"

using namespace std;

class CEnvironment
{
public:
	CEnvironment();
	CEnvironment(int ScreenWidth, int ScreenHeight, float *passed_CameraX, float *passed_CameraY, string mapLoc, CSDL_Setup* passed_csdl_setup, 
						   int passed_TileSizeX, int passed_TileSizeY, int passed_TexSizeX, int passed_TexSizeY, bool tables, bool trees, int passed_LevelNum);
	~CEnvironment(void);

	void DrawBack();
	void DrawFront();

	void Update();
	void LoadEnvFile();
	void FileSave();

	vector<Loot*> GetLootForMap();
	

	int GetMapSizeX();
	int GetMapSizeY();

	enum ModeType
	{
		GamePlay,
		LevelCreation
	};

	vector <Tree*> GetTrees() {return trees;};
	vector <CMap*> GetMapElements() {return maps;}
private:
	int GameMode;
	CSDL_Setup* csdl_setup;
	float* CameraX;
	float* CameraY;
	bool onePressed;
	Sprite* map[4][7];
	Sprite* Enemy;
	int TileSizeX;
	int TileSizeY;
	int StoolY;
	int levelNum;
	vector<CMap*> maps;
	vector <Tree*> trees;
	vector <Loot*> mapLoot;
};

