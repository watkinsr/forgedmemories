#include "FileManagement.h"


FileManagement::FileManagement(CSDL_Setup* p_csdl_setup, float* p_camera_x, float* p_camera_y){
	csdl_setup = p_csdl_setup;
	camera_x = p_camera_x;
	camera_y = p_camera_y;
	entities = new EntityManager();
}


FileManagement::~FileManagement(void){
	delete entities;
	delete npc;
	delete sprite;
	delete quest;
}

bool FileManagement::OpenFile(string file_loc){
	ifstream LoadedFile (file_loc);
	bool file_open = LoadedFile.good();
	if (!LoadedFile.good()){
		cout << "ERROR: Could not load file: " + file_loc << endl;
	}
	return file_open;
}

//file type refers to NPC/Quest etc, the type of file we would like to read.
//read in a file from file location
void FileManagement::HandleQuestFile(string file_loc){
	string quest_type;
	string quest_desc;
	int gold;
	int xp;
	int level_threshold;
	int item_threshold;
	string previous_word = ""; 
	

	ifstream LoadedFile (file_loc);
	string line;
	while (LoadedFile.good() ){
		getline(LoadedFile, line);
		istringstream iss(line);
		while (iss) { 
			string word; 
			iss >> word; 
			if (previous_word == "quest_type:") { 
				quest_type = word;
			}
			if (previous_word == "quest_desc:") { 
				quest_desc = quest_desc.append(line, 12, line.size());
			}
			if (previous_word == "gold:"){
				gold = atoi(word.c_str());
			}
			if (previous_word == "xp:"){
				xp = atoi(word.c_str());
			}
			if (previous_word == "level_threshold:"){
				level_threshold = atoi(word.c_str());
			}
			if (previous_word == "item_threshold:"){
				item_threshold = atoi(word.c_str());
				//Quests quest(quest_type, quest_desc, gold, xp, level_threshold, item_threshold);
				quest = new Quests(quest_type, quest_desc, gold, xp, level_threshold, item_threshold);
				entities->AddQuests(quest);
			}
			previous_word = word; 
		}
	}
}

void FileManagement::HandleNPCFile(string file_loc){
	int x;
	int y;
	int npc_id;
	string npc_sprite_loc;
	string name;
	string previous_word = ""; 

	ifstream LoadedFile (file_loc);
	string line;
	while (LoadedFile.good() ){
		getline(LoadedFile, line);
		istringstream iss(line);
		while (iss) { 
			string word; 
			iss >> word; 
			if (previous_word == "x:") { 
				x = atoi(word.c_str());
			}
			if (previous_word == "y:") { 
				y = atoi(word.c_str());
			}
			if (previous_word == "npc_id:") { 
				npc_id = atoi(word.c_str());
			}
			if (previous_word == "npc_sprite_loc:") { 
				npc_sprite_loc = word;
			}
			if (previous_word == "name:") { 
				name = word;
				npc = new NPC(x,y,npc_id,npc_sprite_loc,name,camera_x,camera_y,csdl_setup,entities->GetQuests());
				entities->AddNPCs(npc);
			}
			previous_word = word; 
		}
	}
}

void FileManagement::HandleSpriteFile(){
	string sprite_loc;
	string file_loc = "res/data/sprites.txt";
	int x;
	int y;
	bool animation;
	int w;
	int h;
	int column_amount;
	int row_amount;
	string previous_word = ""; 

	ifstream LoadedFile (file_loc);
	string line;
	while (LoadedFile.good() ){
		getline(LoadedFile, line);
		istringstream iss(line);
		while (iss) { 
			string word; 
			iss >> word; 
			if (previous_word == "sprite_loc:") { 
				sprite_loc = word;
			}
			if (previous_word == "x:") { 
				x = atoi(word.c_str());
			}
			if (previous_word == "y:") { 
				y = atoi(word.c_str());
			}
			if (previous_word == "animation:") { 
				if (word == "yes")
					animation = true;
				else
					animation = false;
			}
			if (previous_word == "spritesheet_column:") { 
				column_amount = atoi(word.c_str());
			}
			if (previous_word == "spritesheet_row:") { 
				row_amount = atoi(word.c_str());
			}
			if (previous_word == "w:") { 
				w = atoi(word.c_str());
			}
			if (previous_word == "h:") { 
				h = atoi(word.c_str());
				//TODO:
				CollisionRect collision(0,0,0,0);
				sprite = new Sprite(csdl_setup->GetRenderer(), sprite_loc, x, y, w, h, camera_x, camera_y, collision);
				if (animation){
					sprite->SetUpAnimation(row_amount, column_amount);
				}
				entities->AddSprites(sprite);
			}
			previous_word = word;	
		}

	}
}

EntityManager* FileManagement::GetEntities(){
	return entities;
}


