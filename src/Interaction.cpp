#include "Interaction.h"

CInteraction::CInteraction(CSDL_Setup* passed_SDL_Setup, int *passed_MouseX, int *passed_MouseY,  float *passed_CameraX, float *passed_CameraY)
{

	CameraX = passed_CameraX;
	CameraY = passed_CameraY;

	csdl_setup = passed_SDL_Setup;

	MouseX = passed_MouseX;
	MouseY = passed_MouseY;

	w = 0;
	h = 0;
	rangeX = 0;
	rangeY = 0;
	count = 0;

	keyDown = false;
	click = false;
	keyPress = false;

	screenWidth = 0;
	screenHeight = 0;
	opt_select = 0;
	hasKeyBeenReleased = true;
	Menu_Select = new CSprite(csdl_setup->GetRenderer(),"res/data/menu_select.png",32,426,13,13,CameraX,CameraY, CCollisionRect());
}

CInteraction::~CInteraction()
{
	delete csdl_setup;
}

void CInteraction::setRange(int x1, int y1, int x2, int y2)
{
	rangeX = x2 - x1;
	rangeY = y2 - y1;
}

int CInteraction::getRangeX()
{
	return rangeX;
}

int CInteraction::getRangeY()
{
	return rangeY;
}

bool CInteraction::checkInteraction(CSprite* some_sprite)
{
	/*getchar();*/
	//use keyboard input to animate fight or not
	keyPress = false;
	keyDown = false;
	if (csdl_setup->GetMainEvent()->key.keysym.sym == SDLK_DOWN)
	{
		if (some_sprite->GetY() > 425 && some_sprite->GetY() <= 440)
		{
			some_sprite->SetY(some_sprite->GetY()+20);
			keyPress = true;
			keyDown = true;
			return keyPress;
		}
	}

	if (csdl_setup->GetMainEvent()->key.keysym.sym == SDLK_UP)
	{
		if (some_sprite->GetY() > 430 && some_sprite->GetY() <= 446)
		{
			some_sprite->SetY(some_sprite->GetY()-20);
			keyPress = true;
			keyUp = true;

			return keyPress;
		}

	}

	if (*MouseX > some_sprite->GetX() && *MouseX <= some_sprite->GetX()+some_sprite->GetWidth() && *MouseY > some_sprite->GetY() && *MouseY <= some_sprite->GetY()+some_sprite->GetHeight())
	{

		//get anim for mouseclick, run gameloop
		cout << "You have clicked on something" << endl;
		system("cls");
		click = true;
		return click;
	}

	return false;

}

bool CInteraction::EnterPress(){
	switch (csdl_setup->GetMainEvent()->type)
	{
	case SDL_KEYDOWN:
		switch(csdl_setup->GetMainEvent()->key.keysym.sym)
		{
		case SDLK_RETURN:
			return true;
		}
	}
	return false;
}

int CInteraction::checkEnter(CSprite* menu_select, int passed_screenWidth, int passed_screenHeight)
{
	opt_select = 0;
	switch (csdl_setup->GetMainEvent()->type)
	{
	case SDL_KEYDOWN:
		switch(csdl_setup->GetMainEvent()->key.keysym.sym)
		{
		case SDLK_RETURN:
			if (menu_select->GetY() > 420 && menu_select->GetY() <= 440)
			{
				if(hasKeyBeenReleased){
				cout << "You have pressed enter on first option" << endl;
				hasKeyBeenReleased = false;
				opt_select = 1;
				}

			}

			if (menu_select->GetY() > 440 && menu_select->GetY() <= 460)
			{
				if(hasKeyBeenReleased){
				cout << "You have pressed enter on second option" << endl;
				hasKeyBeenReleased = false;
				opt_select = 2;
				}

			}
			break;
		}
		break;
	case SDL_KEYUP:
		switch(csdl_setup->GetMainEvent()->key.keysym.sym)
		{
		case SDLK_RETURN:
			opt_select = 0;
			hasKeyBeenReleased = true;
			break;
		}
		break;
	}

	return opt_select;

}

bool CInteraction::CheckKeyPress()
{
	switch (csdl_setup->GetMainEvent()->type)
	{
	case SDL_KEYDOWN:
		if (!keyDown)
		{
			keyDown = true;
			switch (csdl_setup->GetMainEvent()->key.keysym.sym)
			{
			case SDLK_RETURN:
				return true;
				break;

			default:
				break;
			}

			break;
		}


	case SDL_KEYUP:
		keyDown = false;
		switch (csdl_setup->GetMainEvent()->key.keysym.sym)
		{
		case SDLK_RETURN:
			return false;
			break;
		default:
			break;
		}

		break;
	default:
		break;
	}
	return false;
}