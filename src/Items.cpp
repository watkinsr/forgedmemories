#include "Items.h"

Items::Items()
{

}

Items::Items(CSDL_Setup* passed_SDL_Setup, int *passed_MouseX, int *passed_MouseY, float *passed_CameraX, float *passed_CameraY)
{
	CameraX = passed_CameraX;
	CameraY = passed_CameraY;

	csdl_setup = passed_SDL_Setup;
	MouseX = passed_MouseX;
	MouseY = passed_MouseY;


	chest = new Sprite(csdl_setup->GetRenderer(), "res/data/misc/tchest.png", 400, 100, 47,47, CameraX, CameraY, CollisionRect(0,0,47,47));
	chest->SetUpAnimation(12,8);
	chest->SetOrgin((float)chest->GetWidth()/2.0f, (float)chest->GetHeight());


	timeCheck = SDL_GetTicks();Follow = false;
	distance = 0;
	stopAnimation = false;
}
Items::~Items()
{
	delete chest;
}

void Items::Draw()
{
	chest->Draw();
}

void Items::Update()
{
	UpdateAnimation();
	UpdateControls();
}

void Items::UpdateAnimation()
{
	//gets angle between mouse and bob's position to determine animation in radians.
	float angle = atan2(Follow_Point_Y - *CameraY, Follow_Point_X - *CameraX);

	//conversion to degrees
	angle = (angle * (180.0f/3.14f) + 180.0f);

	if (!stopAnimation)
	{
		if (angle >= 0 && angle <= 360)
		{
			cout << distance << endl;
			if (distance > 15)
				chest->PlayAnimation(0, 2, 0, 300);
			else 
				chest->PlayAnimation(1,1,2,300);
		}
	}
}
void Items::UpdateControls()
{
	if (csdl_setup->GetMainEvent()->button.button == SDL_BUTTON_LEFT)
	{
		//changes position based on camera aswell
		Follow_Point_X = (int)(*CameraX - *MouseX + 300);
		Follow_Point_Y = (int)(*CameraY - *MouseY + 250);
		Follow = true;
	}


	if ((timeCheck + 10 < SDL_GetTicks()) && Follow)
	{

		distance = (float)GetDistance((int)*CameraX, (int)*CameraY, Follow_Point_X, Follow_Point_Y);

		if (distance == 0.0f)
			stopAnimation = true;
		else
			stopAnimation = false;

		if (distance > 15)
		{
			if (*CameraX != Follow_Point_X)
			{
				*CameraX = *CameraX - (((*CameraX - Follow_Point_X) / distance) * 1.5f);
			}

			if (*CameraY != Follow_Point_Y)
			{
				*CameraY = *CameraY - (((*CameraY - Follow_Point_Y) / distance) * 1.5f);
			}
		}
		else
			Follow = false;

		timeCheck = SDL_GetTicks();
	}
}
