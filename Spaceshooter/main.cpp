#include <hge.h>
#include <hgesprite.h>
#include <hgevector.h>
#include <list>

#include "c_player.h"
#include "c_bullet.h"
#include "c_enemy.h"

HGE*	hge	= NULL;

#define BACKGROUND_SCROLLING_SPEED	0.105f
#define BACKGROUND_ALPHA_MIN		50
#define BACKGROUND_ALPHA_MAX		255

//Enemies
std::list<c_enemy*> Enemies;

HTEXTURE	g_tEColors[5]		= { 0 };

//Player
c_player*	Player1				= NULL;

std::list<c_bullet*> Bullets;
HTEXTURE	g_tBullet			= 0;

//Background
HTEXTURE	g_tBackground		= 0;
HTEXTURE	g_tBGGapFix			= 0;
HTEXTURE	g_tStars			= 0;

hgeSprite*	g_sBackground		= NULL;
hgeSprite*	g_sBGGapFix			= NULL;
hgeSprite*	g_sStars			= NULL;

hgeVector	g_vBGPosition		= hgeVector(0,0);

bool		g_bBGFadeIn			= false;
bool		g_bBGFadeOut		= true;
short		g_nBGAlpha			= 255;

//Explosion
HTEXTURE	g_tExplosion		= 0;

struct explosion
{
	hgeAnimation*	Animation;
	hgeVector		Position;
};
std::list<explosion> Explosions;

void		CreateExplosion(hgeVector Position);

//Sound
HEFFECT		g_eBGMusic			= 0;
HEFFECT		g_eBGMusic2			= 0;
HEFFECT		g_eBGMusic3			= 0;
HEFFECT		g_eGunshot			= 0;
HEFFECT		g_eExplosion		= 0;

/////////////////////////////////////////////////
// Frame function
// Which handles logical process in each frame
/////////////////////////////////////////////////
bool FrameFunc()
{
	float delta = hge->Timer_GetDelta();
	
	//Background
	if(g_vBGPosition.x < 800) g_vBGPosition.x += BACKGROUND_SCROLLING_SPEED;
	else g_vBGPosition	= hgeVector(-982,0);

	if(g_bBGFadeOut)
	{
		g_nBGAlpha -= 2;

		if(g_nBGAlpha <= BACKGROUND_ALPHA_MIN)
		{
			g_bBGFadeOut = false;
			g_bBGFadeIn	 = true;
		}
	}
	else if(g_bBGFadeIn)
	{
		g_nBGAlpha += 2;

		if(g_nBGAlpha >= BACKGROUND_ALPHA_MAX)
		{
			g_bBGFadeOut = true;
			g_bBGFadeIn	 = false;
		}
	}
	g_sStars->SetColor(ARGB(g_nBGAlpha,255,255,255));
	
	//Explosions
	for(auto i = Explosions.begin(); i != Explosions.end(); /**/)
	{
		if((*i).Animation->GetFrame() == 4)
		{
			delete (*i).Animation;
			i = Explosions.erase(i);
		}
		else
		{
			(*i).Animation->Update(delta);
			i++;
		}
	}

	//Enemies
	if(Enemies.size() < 5)
	{
		short Health = hge->Random_Int(50, 100);

		c_enemy* Enemy = new c_enemy( hgeVector(830, hge->Random_Int(50,550)), 
			hgeVector(-hge->Random_Int(2,8), hge->Random_Int(-4,4)), 
			Health, 
			g_tEColors[hge->Random_Int(0,4)]);

		Enemies.push_back(Enemy);
	}

	for(auto i = Enemies.begin(); i != Enemies.end(); /**/)
	{
		if((*i)->GetPosition().x < 0 || (*i)->GetPosition().y > 580 || (*i)->GetPosition().y < 20)
		{
			delete (*i);
			i = Enemies.erase(i);
		}
		else
		{
			(*i)->Update(delta);
			i++;
		}
	}
	
	//Player
	Player1->Update(delta);

	if(hge->Input_KeyDown(HGEK_SPACE))
	{
		//Single shot
		c_bullet* Bullet = new c_bullet(Player1->GetPosition() + hgeVector(16,0), hgeVector(15,0), g_tBullet, 100);
						
		Bullets.push_back(Bullet);

		hge->Effect_PlayEx(g_eGunshot,35,0,hge->Random_Float(1,1.5));
	}

	for(auto i = Bullets.begin(); i != Bullets.end(); /**/)
	{
		(*i)->Update();

		if((*i)->GetPosition().x > 800 || (*i)->GetPosition().x < 0 || (*i)->GetPosition().y < 0 || (*i)->GetPosition().y > 600) i = Bullets.erase(i);
		else i++;
	}

	//Collision Bullet vs Enemy
	if(!Bullets.empty() && !Enemies.empty())
	{
		for(auto i = Bullets.begin(); i != Bullets.end(); /**/)
		{
			bool bHit = false;

			for(auto x = Enemies.begin(); x != Enemies.end(); x++)
			{
				if((*x)->GetBoundingBox().Intersect(&(*i)->GetBoundingBox()))
				{
					(*x)->SetHealth((*x)->GetHealth() - (*i)->GetDamage());

					delete (*i);
					i	 = Bullets.erase(i);
					bHit = true;

					break;
				}
			}
			if(!bHit) i++;
		}
	}

	/////////////////////////////////////////////////////////
	// Enemy's health
	// Iteratively check the health of every enemy unit
	/////////////////////////////////////////////////////////
	for(auto i = Enemies.begin(); i != Enemies.end(); /**/)
	{
		if((*i)->GetHealth() <= 0)
		{
			CreateExplosion((*i)->GetPosition());

			delete (*i);
			i = Enemies.erase(i);
		}
		else i++;
	}

	// Enemy vs Player
	/////////////////////////////////////////////////////////
	// Physical collision detact
	// Iteratively check the collision status of every enemy unit
	/////////////////////////////////////////////////////////
	for(auto i = Enemies.begin(); i != Enemies.end(); /**/)
	{
		if((*i)->GetBoundingBox().Intersect(&Player1->GetBoundingBox()))
		{
			// Create explosion
			CreateExplosion((*i)->GetPosition());

			// Delete the 'i' enemy object
			delete (*i);
			// Erase the 'i' enemy image
			i = Enemies.erase(i);

			// Reset player status
			Player1->SetPosition(hgeVector(10,268));
			Player1->SetVelocity(hgeVector(0,0));
		}
		else i++;
	}

	if (hge->Input_GetKeyState(HGEK_ESCAPE)) return true;
	return false;
}

bool RenderFunc()
{
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);

	//Background
	g_sBackground->Render(g_vBGPosition.x,g_vBGPosition.y);
	if(g_vBGPosition.x > -1) 	g_sBackground->Render(g_vBGPosition.x - 982 - 800, 0);
	g_sBGGapFix->Render(g_vBGPosition.x - 28, 0);

	g_sStars->Render(0,0);

	//Player
	Player1->Render();

	for(auto i = Bullets.begin(); i!= Bullets.end(); i++)
	{
		(*i)->Render();
	}

	//Enemies
	for(auto i = Enemies.begin(); i != Enemies.end(); i++)
	{
		(*i)->Render();
	}

	//Explosions
	for(auto i = Explosions.begin(); i != Explosions.end(); i++)
	{
		(*i).Animation->Render((*i).Position.x, (*i).Position.y);
	}


	hge->Gfx_EndScene();
	return false;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	hge = hgeCreate(HGE_VERSION);
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_USESOUND, true);
	hge->System_SetState(HGE_FPS, HGEFPS_VSYNC);
	hge->System_SetState(HGE_TITLE, "Spaceshooter Tutorial");

	if(hge->System_Initiate())
	{
		Bullets.clear();

		//Background
		g_tBackground	= hge->Texture_Load("images/farback.jpg");
		g_tBGGapFix		= hge->Texture_Load("images/bg_gapfix.jpg");
		g_tStars		= hge->Texture_Load("images/starfield.png");

		g_sBackground	= new hgeSprite(g_tBackground, 0, 0, 1782, 600);
		g_sBGGapFix		= new hgeSprite(g_tBGGapFix, 0, 0, 64, 600);
		g_sStars		= new hgeSprite(g_tStars, 0, 0, 800, 600);

		g_vBGPosition	= hgeVector(-982, 0);

		//Player
		Player1			= new c_player(hgeVector(10, 268), hgeVector(5,0));

		g_tBullet		= hge->Texture_Load("images/bullet.png");

		//Enemies
		g_tEColors[0]	= hge->Texture_Load("images/eSpritesheet_40x30.png");
		g_tEColors[1]	= hge->Texture_Load("images/eSpritesheet_40x30_hue1.png");
		g_tEColors[2]	= hge->Texture_Load("images/eSpritesheet_40x30_hue2.png");
		g_tEColors[3]	= hge->Texture_Load("images/eSpritesheet_40x30_hue3.png");
		g_tEColors[4]	= hge->Texture_Load("images/eSpritesheet_40x30_hue4.png");

		//Explosion
		g_tExplosion	= hge->Texture_Load("images/Explosion-Sprite-Sheet.png");

		//Sounds
		g_eBGMusic		= hge->Effect_Load("sounds/134707__atanasiu__deepspace-sonification.ogg");
		g_eBGMusic2		= hge->Effect_Load("sounds/158733__panzertank15__spaceambient.ogg");
		g_eBGMusic3		= hge->Effect_Load("sounds/108929__soulman-90__ambience-space.ogg");
		g_eGunshot		= hge->Effect_Load("sounds/111047__cyberkineticfilms__gunshot-2-laser.ogg");
		g_eExplosion	= hge->Effect_Load("sounds/21410__heigh-hoo__blow.aif");


		hge->Effect_PlayEx(g_eBGMusic, 40, 0, 0, true);
		hge->Effect_PlayEx(g_eBGMusic2, 100, 0, 0, true);
		hge->Effect_PlayEx(g_eBGMusic3, 10, 0, 0, true);

		hge->System_Start();

		//Cleanup..
		hge->Texture_Free(g_tBackground);
		hge->Texture_Free(g_tExplosion);
		hge->Texture_Free(g_tBGGapFix);
		hge->Texture_Free(g_tBullet);
		hge->Texture_Free(g_tStars);
		
		hge->Effect_Free(g_eExplosion);
		hge->Effect_Free(g_eBGMusic);
		hge->Effect_Free(g_eBGMusic2);
		hge->Effect_Free(g_eBGMusic3);
		hge->Effect_Free(g_eGunshot);

		delete g_sBackground;
		delete g_sBGGapFix;
		delete g_sStars;
		delete Player1;
		
		for(auto i = Bullets.begin(); i != Bullets.end(); /**/)
		{
			delete (*i);
			i = Bullets.erase(i);
		}

		for(auto i = Enemies.begin(); i != Enemies.end(); /**/)
		{
			delete (*i);
			i = Enemies.erase(i);
		}

		for(short i = 0; i < 5; i++) hge->Texture_Free(g_tEColors[i]);

		for(auto i = Explosions.begin(); i != Explosions.end(); /**/)
		{
			delete (*i).Animation;
			i = Explosions.erase(i);
		}

	}
	else
	{
	  MessageBox(NULL, hge->System_GetErrorMessage(), "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
	}
	
	hge->System_Shutdown();
	hge->Release();
 
	return 0;
}

void CreateExplosion(hgeVector Position)
{
	explosion exp;
	exp.Animation = new hgeAnimation(g_tExplosion,5,10,0,0,118,118);
	exp.Animation->SetHotSpot(59,59);
	exp.Animation->Play();
	exp.Position = Position;
	Explosions.push_back(exp);

	hge->Effect_PlayEx(g_eExplosion,100,0,hge->Random_Float(1,3));
}