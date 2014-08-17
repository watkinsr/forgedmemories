#pragma once
#include "stdafx.h"
#include "SDL_Font_Setup.h"
#include "CollissionRect.h"
 #include <windows.h>


class CSprite
{
public:
	CSprite(SDL_Renderer* passed_renderer, std::string FilePath, int x, int y, int w, int h, float *CameraX, float *CameraY, CCollisionRect passed_CollisionRect);
	CSprite();
	~CSprite(void);

	void DrawSteady();
	void Draw();

	void SetX(float X);
	void SetY(float Y);
	void SetPosition(float X, float Y);

	float GetX();
	float GetY();

	int GetSpriteCameraX();
	int GetSpriteCameraY();

	void SetWidth(int W);
	void SetHeight(int H);

	float GetXTest();
	float GetYTest();
	// 

	bool IsSpriteClose(CSprite* player, CSprite* enemy, float range);
	void MoveSpriteTowardsEntity(CSprite* player, CSprite* enemy);

	int GetWidth();
	int GetHeight();
	int GetCameraDrawWidth();

	void SetOrgin(float X, float Y);
	void PlayAnimation(int BeginFrame, int EndFrame, int Row, float Speed);
	void SetUpAnimation(int passed_Amount_X, int passed_Amount_Y);
	void TestWidthChange(int widthChange);

	void AnimateOnce(int BeginFrame, int EndFrame, int Row, float Speed);
	bool HasAnimationFinished();


	CCollisionRect GetCollisionRect() {return CollisionRect;}

	bool isColliding(CCollisionRect theCollider);
private:
	CCollisionRect CollisionRect;

	SDL_Rect Camera;
	float *CameraX;
	float *CameraY;

	float Orgin_X;
	float Orgin_Y;

	float X_pos;
	float Y_pos;

	SDL_Texture* CollisionImage;
	SDL_Texture* image;
	SDL_Rect rect;

	SDL_Rect crop;

	int img_width;
	int img_height;
	int CurrentFrame;
	int animationDelay;
	int previousTime;
	int Amount_Frame_X;
	int Amount_Frame_Y;

	bool animateFinish;
	string filepath;

	SDL_Renderer* renderer;

	
};


