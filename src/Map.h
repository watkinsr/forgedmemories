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
	CSprite* GetMapTexture() {return map_texture;}
	CSprite* GetTable() {return table;}

	//return wall sprites
	CSprite* GetWallOutline_Left() {return wall_out_left;}
	CSprite* GetWallOutline_Right() {return wall_out_right;}
	CSprite* GetWallOutline_Top() {return wall_out_top;}
	CSprite* GetWallOutline_Bottom() {return wall_out_bottom;}


	int GetTableNum();
private:
	CSprite* map_texture;
	CSprite* table;
	CSprite* stool;
	CSprite* wall_out_left;
	CSprite* wall_out_right;
	CSprite* wall_out_top;
	CSprite* wall_out_bottom;

	int stool_y;
	int tile_size_x;
	int tile_size_y;
	int screen_width;
	int screen_height;
};

#endif