#include "stdafx.h"
#include "SFXManager.h"


SFXManager::SFXManager(void)
{
	is_loop_sound = false;
	play_once = true;
}


SFXManager::~SFXManager(void)
{
}

void SFXManager::SwordAttack(){
	SDL_Sound *sounds = new SDL_Sound();
	is_loop_sound = false;
	sounds->LoadMedia("res/sfx/sword_attack_1.wav", is_loop_sound);
	sounds->PlayMedia(false);
	delete sounds;
}

void SFXManager::GetSceneMusic(){
	is_loop_sound = true;
	if (play_once){
	SDL_Sound *sounds = new SDL_Sound();
	sounds->LoadMedia("res/sfx/pub_01.wav", is_loop_sound);
	sounds->PlayMedia(true);
	play_once = false;
	delete sounds;
	}
}
