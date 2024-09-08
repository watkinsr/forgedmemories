#pragma once
//#include "Main.h"
using namespace std;
#include "stdafx.h"

class CSDL_Setup
{
public:
CSDL_Setup(bool* quit, int ScreenWidth, int ScreenHeight);
~CSDL_Setup(void);

SDL_Renderer* GetRenderer();
SDL_Event* GetMainEvent();
SDL_Window* GetWindow();

void Begin();
void End();



private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event* mainEvent;

};

