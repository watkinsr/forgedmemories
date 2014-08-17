#include "stdafx.h"
#include "CollissionRect.h"

CCollisionRect::CCollisionRect()
{
	SetRectangle(0,0,0,0);
	OffsetX = 0;
	OffsetY = 0;
}

CCollisionRect::CCollisionRect(int x, int y, int w, int h)
{
	OffsetX = x;
	OffsetY = y;
	SetRectangle(0,0,w,h);
}


CCollisionRect::~CCollisionRect(void)
{
}

void CCollisionRect::SetRectangle(int x, int y, int w, int h)
{
	CollisionRectangle.x = x + OffsetX;
	CollisionRectangle.y = y + OffsetY;
	CollisionRectangle.w = w;
	CollisionRectangle.h = h;
}