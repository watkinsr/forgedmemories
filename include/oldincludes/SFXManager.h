#pragma once
#include "SDL_Sound.h"

class SFXManager
{
public:
	SFXManager(void);
	~SFXManager(void);
	void SwordAttack();
	void GetSceneMusic();

private:
	bool play_once;
	bool is_loop_sound;
};

