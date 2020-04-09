#pragma once
#ifndef MAP_H
#define MAP_H

#include "Sprite.h"
#include "stdafx.h"
#include "SDL_Setup.h"

class CMap
{
public:
	CMap(int xCount, int yCount, int passed_TileSizeX, int passed_TileSizeY, int TextureSizeX, int TextureSizeY, 
		   string MapFileLoc, float *CameraX, float *CameraY, CSDL_Setup* csdl_setup, bool tables, bool trees, int StoolY);
	~CMap();

	void DrawTable();
	void DrawTexture();
	void DrawStool();
	Sprite* GetMapTexture() {return map_texture;}
	Sprite* GetTable() {return table;}

	//return wall sprites
	Sprite* GetWallOutline_Left() {return wall_out_left;}
	Sprite* GetWallOutline_Right() {return wall_out_right;}
	Sprite* GetWallOutline_Top() {return wall_out_top;}
	Sprite* GetWallOutline_Bottom() {return wall_out_bottom;}


	int GetTableNum();
private:
	Sprite* map_texture;
	Sprite* table;
	Sprite* stool;
	Sprite* wall_out_left;
	Sprite* wall_out_right;
	Sprite* wall_out_top;
	Sprite* wall_out_bottom;

	int stool_y;
	int tile_size_x;
	int tile_size_y;
	int screen_width;
	int screen_height;
};

#endif
