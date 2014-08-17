#pragma once
#include "stdafx.h"

class CCollisionRect
{
public:
	CCollisionRect();
	CCollisionRect(int x, int y, int w, int h);
	~CCollisionRect(void);

	void SetRectangle(int x, int y, int w, int h);
	SDL_Rect GetRectangle() { return CollisionRectangle; }

	void SetX(int x) { CollisionRectangle.x = x +OffsetX;	}
	void SetY(int y) { CollisionRectangle.y = y +OffsetY;	}
private:
	int OffsetX;
	int OffsetY;
	SDL_Rect CollisionRectangle;
};



