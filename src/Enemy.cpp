#include "Enemy.h"

CEnemy::CEnemy(int x_passed, int y_passed, float *CameraX, float *CameraY, CSDL_Setup* csdl_setup, int passed_HP, int passed_Damage, int passed_XPGive){
	x = x_passed;
	y = y_passed;

	enemy_sprite = new CSprite(csdl_setup->GetRenderer(),"data/environment/enemy_spritesheet.png",x,y,50,50,CameraX,CameraY, CCollisionRect());
	enemy_sprite->SetUpAnimation(5,1);
	//enemy_sprite->SetOrgin(enemy_sprite->GetWidth()/2.0f, enemy_sprite->GetHeight());
	En_HP = new CSprite(csdl_setup->GetRenderer(),"data/hp_enem.png",x+10,y-10,24,4,CameraX,CameraY, CCollisionRect());

	Damage = passed_Damage;
	HP = passed_HP;
	XPGive = passed_XPGive;

	count = 0;
	DecreaseHPBy = 0;
	PercentageHPChange = 0;
	temp = 0;
	HPWidth = 0;
	isTimeSet = false;
	goDown = false;
	goUp = false;
	goRight = true;
	goLeft = false;
	attack_anim = false;
	time = 0;
}

CEnemy::CEnemy(){

}
CSprite* CEnemy::getEnemyHPSprite(){
	return En_HP;
}
CEnemy::~CEnemy(){
	delete enemy_sprite;
}

//if the enemy is not attacking, display the attack animation, otherwise, display attack animation.
void CEnemy::Draw(){
	if (!attack_anim)
		enemy_sprite->PlayAnimation(0,2,0,300);
	else
		enemy_sprite->PlayAnimation(0,4,0,200);

	enemy_sprite->Draw();
	En_HP->Draw();
}

void CEnemy::setHP(int passed_HP){
	HP = HP + passed_HP;
}

void CEnemy::SetAttackAnim(bool p_attack){
	attack_anim = p_attack;
}

int CEnemy::getHPWidth(){
	return En_HP->GetWidth();
}

void CEnemy::changeHP(vector<CEnemy*> EnemyVect, int EnemyVectNum, int PlayerDamage){
	if (count == 0){
		count ++;
	PercentageHPChange = PlayerDamage;
	temp = getHP() + PlayerDamage;
	PercentageHPChange = PercentageHPChange/temp;
	PercentageHPChange = PercentageHPChange*100;
	DecreaseHPBy = En_HP->GetWidth();
	DecreaseHPBy = DecreaseHPBy/100*PercentageHPChange;//100;
	HPWidth = En_HP->GetWidth();
	}
	HPWidth = HPWidth - DecreaseHPBy;
	EnemyVect[EnemyVectNum]->En_HP->TestWidthChange(HPWidth);
	float temp2 = En_HP->GetWidth();
}

int CEnemy::getHP(){
	return HP;
}

int CEnemy::getDamage(){
	return Damage;
}

int CEnemy::GetSpriteCameraX(){
	return enemy_sprite->GetSpriteCameraX();
}

int CEnemy::GetXP(){
	return XPGive;
}

int CEnemy::GetSpriteCameraY(){
	return enemy_sprite->GetSpriteCameraY();
}

//move the enemy 2 pixels every half a second, but stop when they have reached moving 30 pixels.
//we can accomplish this by checking that the original position of the enemy is now 30, and if it is, stop doing so and move downwards
//after we move downwards for 30 pixels, move left for 30 then up for 30.
void CEnemy::GetAIPathSquare(){
	float prevX = x;
	float prevY = y;
	float currentX = enemy_sprite->GetX();
	float currentY = enemy_sprite->GetY();
	
	if (!isTimeSet){
	time = SDL_GetTicks();
	}
	isTimeSet = true;

	//if sufficient time has elapsed, let's move our enemy sprite.

	//determine whether go right, down, left or up by checking enemy's coords.
	if (SDL_GetTicks() > time + 30){
		if (goRight){
			isTimeSet = false;
			enemy_sprite->SetPosition(currentX + 2, currentY);
			En_HP->SetPosition(En_HP->GetX() + 2, En_HP->GetY() );
			if (currentX - prevX == 30){
				goRight = false;
				goDown = true; 
			}
		}
		if (goDown){
			isTimeSet = false;
			enemy_sprite->SetPosition(currentX, currentY + 2);
			En_HP->SetPosition(En_HP->GetX(), En_HP->GetY() + 2);
			if (currentY - prevY == 30){
				goDown = false;
				goLeft = true;
			}
		}
		if (goLeft){
			isTimeSet = false;
			enemy_sprite->SetPosition(currentX - 2, currentY);
			En_HP->SetPosition(En_HP->GetX() - 2, En_HP->GetY());
			if (currentX - prevX <= -30){
				goLeft = false;
				goUp = true;
			}
		}
		if (goUp){
			isTimeSet = false;
			enemy_sprite->SetPosition(currentX, currentY - 2);
			En_HP->SetPosition(En_HP->GetX(), En_HP->GetY() - 2);
			if (currentY - prevY == -30){
				goUp = false;
				goRight = true;
			}
		}

	}
}
