#include "stdafx.h"
#include "CollissionRect.h"

CollisionRect::CollisionRect()
{
	SetRectangle(0,0,0,0);
	OffsetX = 0;
	OffsetY = 0;
}

CollisionRect::CollisionRect(int x, int y, int w, int h)
{
	OffsetX = x;
	OffsetY = y;
	SetRectangle(0,0,w,h);
}


CollisionRect::~CollisionRect(void)
{
}

void CollisionRect::SetRectangle(int x, int y, int w, int h)
{
	collisionRectangle.x = x + OffsetX;
	collisionRectangle.y = y + OffsetY;
	collisionRectangle.w = w;
	collisionRectangle.h = h;
}

SDL_Rect CollisionRect::GetRectangle() {
	return collisionRectangle;
}
