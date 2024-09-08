#pragma once
#include "stdafx.h"

class CollisionRect
{
public:
	CollisionRect();
	CollisionRect(int x, int y, int w, int h);
	~CollisionRect(void);

	void SetRectangle(int x, int y, int w, int h);
	SDL_Rect GetRectangle();

	void SetX(int x) { collisionRectangle.x = x +OffsetX;	}
	void SetY(int y) { collisionRectangle.y = y +OffsetY;	}
private:
	int OffsetX;
	int OffsetY;
	SDL_Rect collisionRectangle;
};



