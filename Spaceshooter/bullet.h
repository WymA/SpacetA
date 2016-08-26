#ifndef C_BULLET_H
#define C_BULLET_H
#pragma once

#include <hge.h>
#include <hgesprite.h>
#include <hgerect.h>
#include <hgevector.h>

#define BULLET_FRICTION 0.99

class c_bullet
{
private:
	static HGE*		hge;
	hgeVector		Position;
	hgeVector		Velocity;
	hgeSprite*		Sprite;
	hgeRect			BoundingBox;
	float			Speed;
	short			Damage;

	float			CenterY;
	float			Radius;
	float			Angle;
	bool			bCenterYSet;
	bool			bOscillate;
	bool			bOscillateReverse;
public:
	c_bullet(hgeVector Position, hgeVector Velocity, HTEXTURE &Texture, short Damage);
	~c_bullet();

	void Update();
	void Render();

	void		SetDamage(short damage) { Damage = damage; };
	short		GetDamage() { return Damage; };
	void		SetOscillateReverse(bool Value) { bOscillateReverse = Value; };
	void		SetOscillate(bool Value) { bOscillate = Value; };
	hgeRect		GetBoundingBox() { return BoundingBox; };
	hgeVector	GetPosition() { return Position; };
};

#endif