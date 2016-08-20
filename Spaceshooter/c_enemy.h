#ifndef C_ENEMY_H
#define C_ENEMY_H
#pragma once

#include <hge.h>
#include <hgevector.h>
#include <hgeanim.h>

#define ENEMY_FRICTION 0.95

class c_enemy
{
private:
	static HGE*		hge;
	hgeVector		Position;
	hgeVector		Velocity;
	hgeAnimation*	Sprite;
	hgeRect			BoundingBox;
	float			Speed;

	float			CenterY;
	float			Radius;
	float			Angle;
	bool			bCenterYSet;
	bool			bOscillate;
	short			Health;
public:
	c_enemy(hgeVector Position, hgeVector Velocity, short Health, HTEXTURE &Texture);
	~c_enemy();

	bool Update(float delta);
	void Render();

	short		GetHealth() { return Health; };
	void		SetHealth(short health) { Health = health; };
	void		SetOscillate(bool Value) { bOscillate = Value; };
	float		GetSpeed() { return Speed; };
	void		SetSpeed(float speed) { Speed = speed; };
	hgeRect		GetBoundingBox() { return BoundingBox; };
	hgeVector	GetPosition() { return Position; };
};

#endif