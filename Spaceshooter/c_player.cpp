#include "c_player.h"

HGE* c_player::hge = 0;

c_player::c_player(hgeVector position, hgeVector velocity) : Position(position), Velocity(velocity)
{
	hge = hgeCreate(HGE_VERSION);

	Texture = hge->Texture_Load("images/Spritesheet_64x29.png");
	Sprite	= new hgeAnimation(Texture,4,4,0,0,64,29);
	Sprite->SetHotSpot(32,14.5);
	Sprite->Play();

	Speed		= 0.15;
}

c_player::~c_player()
{
	hge->Texture_Free(Texture);
	delete Sprite;

	hge->Release();
}

bool c_player::Update(float delta)
{
	Velocity.x *= PLAYER_FRICTION;
	Velocity.y *= PLAYER_FRICTION;

	Position.x += Velocity.x;
	Position.y += Velocity.y;

	if(hge->Input_GetKeyState(HGEK_A) || hge->Input_GetKeyState(HGEK_LEFT)	 && Position.x > 32) Velocity.x -= (Speed * M_PI) /2;
	if(hge->Input_GetKeyState(HGEK_D) || hge->Input_GetKeyState(HGEK_RIGHT)  && Position.x < 800) Velocity.x += (Speed * M_PI) /2;
	if(hge->Input_GetKeyState(HGEK_W) || hge->Input_GetKeyState(HGEK_UP)	 && Position.y > 0) Velocity.y -= (Speed * M_PI) /2;
	if(hge->Input_GetKeyState(HGEK_S) || hge->Input_GetKeyState(HGEK_DOWN)   && Position.y < 600) Velocity.y += (Speed * M_PI) /2;

	if(Position.x > 800) { Position.x -= 1; Velocity.x = -Velocity.x; };
	if(Position.x < 0)   { Position.x += 1; Velocity.x = -Velocity.x; };
	if(Position.y < 0)	 { Position.y += 1; Velocity.y = -Velocity.y; };
	if(Position.y > 600) { Position.y -= 1; Velocity.y = -Velocity.y; };

	Sprite->Update(delta);

	Sprite->GetBoundingBox(Position.x, Position.y, &BoundingBox);
	return false;
}

void c_player::Render()
{
	Sprite->Render(Position.x,Position.y);
}