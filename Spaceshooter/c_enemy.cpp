#include "c_enemy.h"

HGE* c_enemy::hge = 0;

c_enemy::c_enemy(hgeVector position, hgeVector velocity, short health, HTEXTURE &Texture) 
	: Position(position), Velocity(velocity), Health(health)
{
	hge = hgeCreate(HGE_VERSION);

	Sprite	= new hgeAnimation(Texture,6,6,0,0,40,30);
	Sprite->SetHotSpot(20,15);
	Sprite->Play();

	Speed		= 0.030;

	CenterY		= 0;
	Radius		= hge->Random_Float(50.0f,80.0f);
	Angle		= 0.0f;
	bCenterYSet	= false;
	bOscillate	= false;
}

c_enemy::~c_enemy()
{
	delete Sprite;

	hge->Release();
}

bool c_enemy::Update(float delta)
{
	Velocity.x *= ENEMY_FRICTION;
	Velocity.y *= ENEMY_FRICTION;

	Position.x += Velocity.x;
	if(!bOscillate) Position.y += Velocity.y;
	else
	{
		if(!bCenterYSet)
		{
			if(Velocity.y > -0.000001f && Velocity.y < 0.000001f)
			{
				CenterY = Position.y;
				bCenterYSet = true;
			}
		}

		Position.y = CenterY + sin(Angle) * Radius;
		Angle += 2 * delta;
	}

	Sprite->Update(delta);
	
	Velocity.x -= Speed;
	
	Sprite->GetBoundingBox(Position.x, Position.y, &BoundingBox);
	return false;
}

void c_enemy::Render()
{
	Sprite->Render(Position.x, Position.y);
}