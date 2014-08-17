#include "Sprite.h"

CSprite::CSprite(){
	filepath = "";
}

CSprite::CSprite(SDL_Renderer* passed_renderer, std::string FilePath, int x, int y, int w, int h, float *passed_CameraX, float *passed_CameraY, CCollisionRect passed_CollisionRect){
	CollisionRect = passed_CollisionRect;

	renderer = passed_renderer;
	image = NULL;
	image = IMG_LoadTexture(renderer,FilePath.c_str());
	filepath = FilePath;
	if (image == NULL){
		std::cout << "Couldn't Load " << FilePath.c_str() << std::endl;
	}

	CollisionImage = NULL;

	CollisionImage = IMG_LoadTexture(renderer, "res/data/Debug/CollisionBox.png");

	if (CollisionImage == NULL){
		std::cout << "Couldn't Load " << "CollisionImage" << std::endl;
	}

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	SDL_QueryTexture(image,NULL,NULL, &img_width, &img_height);

	crop.x = 0;
	crop.y = 0;
	crop.w = img_width;
	crop.h = img_height;

	X_pos = (float)x;
	Y_pos = (float)y;

	Orgin_X = 0;
	Orgin_Y = 0;

	CurrentFrame = 0;
	Amount_Frame_X = 0;
	Amount_Frame_X = 0;

	CameraX = passed_CameraX;
	CameraY = passed_CameraY;

	Camera.x = (int)(rect.x + *CameraX);
	Camera.y = (int)(rect.y + *CameraY);
	Camera.w = rect.w;
	Camera.h = rect.h;

	previousTime = 0;
	animateFinish = false;
}

CSprite::~CSprite(void){
	SDL_DestroyTexture(image);
	SDL_DestroyTexture(CollisionImage);
}

void CSprite::SetUpAnimation(int passed_Amount_X, int passed_Amount_Y){
	Amount_Frame_X = passed_Amount_X;
	Amount_Frame_Y = passed_Amount_Y;
}

void CSprite::TestWidthChange(int widthChange){
	Camera.w = widthChange;
}

int CSprite::GetCameraDrawWidth(){
	return Camera.w;
}



void CSprite::PlayAnimation(int BeginFrame, int EndFrame, int Row, float Speed){
	if (animationDelay+Speed < SDL_GetTicks()){
		//if the endframe has not been reached, set the frame.
		if (EndFrame <= CurrentFrame)
			CurrentFrame = BeginFrame;
		else 
			CurrentFrame++;

		crop.x = CurrentFrame * (img_width/Amount_Frame_X);
		crop.y = Row * (img_height/Amount_Frame_Y);
		crop.w = img_width/Amount_Frame_X;
		crop.h = img_height/Amount_Frame_Y;

		animationDelay = SDL_GetTicks();
	}
}

//if the animation has not yet run, create a delay so that animation runs smoothly by specifying a speed. 
//if (the current time - previous update time > desired frame time) increment frame()
void CSprite::AnimateOnce(int BeginFrame, int EndFrame, int Row, float speed){
	if(!animateFinish){
		int currentTime = SDL_GetTicks();
		if (currentTime - previousTime > speed){
			//if currentframe exceeds endframe, animation has completed.
			if (EndFrame <= CurrentFrame)
				animateFinish = true;
			else 
				CurrentFrame++;

			crop.x = CurrentFrame * (img_width/Amount_Frame_X);
			crop.y = Row * (img_height/Amount_Frame_Y);
			crop.w = img_width/Amount_Frame_X;
			crop.h = img_height/Amount_Frame_Y;

			previousTime = SDL_GetTicks();
		}
	}
}

//if the animation has finished, return a true val.
bool CSprite::HasAnimationFinished(){
	return animateFinish;
}

bool CSprite::IsSpriteClose(CSprite* player, CSprite* enemy, float range){
	float enemyX = (float)enemy->GetSpriteCameraX();
	float enemyY = (float)enemy->GetSpriteCameraY();
	float playerX = (float)player->GetSpriteCameraX();
	float playerY = (float)player->GetSpriteCameraY();

	playerX = playerX - 30;
	playerY = playerY - 50;

	//if they are in range specified, move them towards the player.
	if ((enemyX - playerX < range) && (enemyX - playerX > -range) && (enemyY - playerY < range) && (enemyY - playerY > -range))
	{
		return true;
	}
	else
	{
		return false;
	}
		
}

void CSprite::MoveSpriteTowardsEntity(CSprite* player, CSprite* enemy){
	float enemyX = (float)enemy->GetSpriteCameraX();
	float enemyY = (float)enemy->GetSpriteCameraY();
	float playerX = (float)(player->GetSpriteCameraX() - 30); //recenter playerX camera.
	float playerY = (float)(player->GetSpriteCameraY() - 30); //recenter playerY camera.
	float diffX = enemyX - playerX;
	float diffY = enemyY - playerY;	

	//move the enemy towards the player.
	//determine where player is in relation to enemy.
	if ((diffX > 0) && (enemyX > playerX)){
		//player is to the left of enemy.
		enemy->SetX(enemy->GetX() - 0.2f);
	}
	//player to the right of enemy.
	if (diffX <= 0) {
		enemy->SetX(enemy->GetX() + 0.2f);
	}
	//if player is below, move enemy down.
	if ((diffY > 0) && (enemyY > playerY)){
		enemy->SetY(enemy->GetY() - 0.2f);
	}
	//player above, move enemy up
	else{
		enemy->SetY(enemy->GetY() + 0.2f);
	}
}



void CSprite::Draw()
{

	//offsets camera based on image.
	Camera.x = (int)(rect.x + *CameraX);
	Camera.y = (int)(rect.y + *CameraY);

	CollisionRect.SetX(Camera.x);
	CollisionRect.SetY(Camera.y);

	SDL_RenderCopy(renderer,image, &crop, &Camera);

	/*SDL_RenderCopy(renderer,CollisionImage, NULL, &CollisionRect.GetRectangle());*/
}
//when  called, character won't be affected by camera.
void CSprite::DrawSteady()
{
	SDL_RenderCopy(renderer,image, &crop, &rect);
	SDL_RenderCopy(renderer,CollisionImage, NULL, &CollisionRect.GetRectangle());
}

void CSprite::SetX(float X)
{
	X_pos = X;

	rect.x = int(X_pos - Orgin_X);
}

void CSprite::SetY(float Y)
{
	Y_pos = Y;

	rect.y = int(Y_pos - Orgin_Y);
}

float CSprite::GetXTest(){
	return rect.x + *CameraX;
}

float CSprite::GetYTest(){
	return rect.y + *CameraY;
}



void CSprite::SetPosition(float X, float Y)
{
	X_pos = X;
	Y_pos = Y;

	rect.x = int(X_pos - Orgin_X);
	rect.y = int(Y_pos - Orgin_Y);
}

float CSprite::GetX()
{
	return X_pos;
}

float CSprite::GetY()
{
	return Y_pos;
}

void CSprite::SetOrgin(float X, float Y)
{
	Orgin_X = X;
	Orgin_Y = Y;

	SetPosition(GetX(), GetY());

}

void CSprite::SetWidth(int W)
{
	rect.w = W;
}

void CSprite::SetHeight(int H)
{
	rect.h = H;
}

int CSprite::GetWidth()
{
	return rect.w;
}

int CSprite::GetHeight()
{
	return rect.h;
}

int CSprite::GetSpriteCameraX()
{
	return Camera.x;
}
int CSprite::GetSpriteCameraY()
{
	return Camera.y;
}

bool CSprite::isColliding(CCollisionRect theCollider)
{
	//checks the four corners of collision rectangle.
	return !(CollisionRect.GetRectangle().x + CollisionRect.GetRectangle().w < theCollider.GetRectangle().x || 
		CollisionRect.GetRectangle().y + CollisionRect.GetRectangle().h < theCollider.GetRectangle().y ||
		CollisionRect.GetRectangle().x > theCollider.GetRectangle().x + theCollider.GetRectangle().w ||
		CollisionRect.GetRectangle().y > theCollider.GetRectangle().y + theCollider.GetRectangle().h);
}