#include "Map.h"

CMap::CMap(int x_count, int y_count, int p_tile_size_x, int p_tile_size_y, int texture_size_x, int texture_size_y, 
		   string map_file_loc, float *camera_x, float *camera_y, CSDL_Setup* csdl_setup, bool tables, bool trees, int p_stool_y){
	tile_size_x = p_tile_size_x;
	tile_size_y = p_tile_size_y;
	stool_y = p_stool_y;
	map_texture = new CSprite(csdl_setup->GetRenderer(),map_file_loc, texture_size_x*x_count, texture_size_y*y_count, texture_size_x, texture_size_y, camera_x, camera_y, CCollisionRect(0,0,0,0));
	if (tables == true){
		tables = false;
		table = new CSprite(csdl_setup->GetRenderer(),"data/environment/pub/table.png", 200, 300, 100, 300, camera_x, camera_y, CCollisionRect(0,0,100,300));

		stool = new CSprite(csdl_setup->GetRenderer(),"data/environment/pub/stool.png", 300,stool_y, 30,30, camera_x, camera_y, CCollisionRect(0,0,30,30));
	}
		//make objects for outside walls to get drawn by SDL
		for (int i = 0; i < 5; i++){
			wall_out_left = new CSprite(csdl_setup->GetRenderer(),"data/environment/pub/wall_outline.png", -30,0, 30,480*i, camera_x, camera_y, CCollisionRect(0,0,30,480*i));
			wall_out_right = new CSprite(csdl_setup->GetRenderer(),"data/environment/pub/wall_outline.png", texture_size_x*tile_size_x,0, 30,480*i, camera_x, camera_y, CCollisionRect(0,0,30,480*i));
		}
		for (int i = 0; i < 5; i++){
			wall_out_top = new CSprite(csdl_setup->GetRenderer(),"data/environment/pub/wall_outline_w.png", -30,0, 640*i,30, camera_x, camera_y, CCollisionRect(0,0,640*i,30));
			wall_out_bottom = new CSprite(csdl_setup->GetRenderer(),"data/environment/pub/wall_outline_w.png", -30,texture_size_y*tile_size_y, 640*i,30, camera_x, camera_y, CCollisionRect(0,0,640*i,30));
		}

}

CMap::~CMap(){
	for (int i = 0; i < tile_size_x; i++){
		for (int j = 0; j < tile_size_y; j++){
			delete map_texture;
		}
	}
	for (int i = 0; i < 5; i++){
		delete wall_out_bottom;
		delete wall_out_left;
		delete wall_out_right;
		delete wall_out_top;
	}
	delete map_texture;
	delete table;
	delete stool;
}

void CMap::DrawTexture(){
	//draw floor
	map_texture->Draw();
	//draw walls
	wall_out_bottom->Draw();
	wall_out_top->Draw();
	wall_out_left->Draw();
	wall_out_right->Draw();
}

void CMap::DrawTable(){
	table->Draw();
}

void CMap::DrawStool(){
	stool->Draw();
}

int CMap::GetTableNum(){
	return 1;
}

