// header
#include "game.h"
// std
#include <cassert>
#include <cmath>
#include <cstring> // memset
#ifndef NDEBUG
#include <iostream>
#endif
// For testing.
#define STARTING_LEVEL 1
namespace laserworp
{
// The virtual playing field is a 1000x1000 square.
constexpr int kArenaSize=1000;
constexpr int kEnemyMissileSpeed=10;
constexpr int kPlayerBulletSpeed=20;
constexpr int kPlayerFireDelay=7;
constexpr int kPlayerSpeed=20;
void operator+=(Event& a,Event b)
{
	a=static_cast<Event>(a|b);
}
bool Intersect(const Object& a,const Object& b)
{
	const int kRadius=48;
	return ((b.x-a.x)*(b.x-a.x)+(b.y-a.y)*(b.y-a.y))<kRadius*kRadius;
}
void Clamp(int& x,int& old)
{
	if (x>kArenaSize)
	{
		x=0;
		old=x;
	}
	if (x<0)
	{
		x=kArenaSize;
		old=x;
	}
}
void Bounce(Object& o)
{
	if (o.x>kArenaSize)
		o.vx=-abs(o.vx);
	if (o.x<0)
		o.vx=abs(o.vx);
	if (o.y>kArenaSize)
		o.vy=-abs(o.vy);
	if (o.y<0)
		o.vy=abs(o.vy);
}
constexpr int Game::kEnemyStartingSpeeds[8][2]= {{0,0},{8,0},{12,12},{14,0},{-16,0},{0,-18},{0,8},{0,0}};
// =============================================================================
Game::Game():_lives(),_level(),_score(),_player(),_bullets_count(),
	_enemies_count(),_missiles_count(),_player_visible(),_fire_delay(),
	_event_data(),_enemy_timer()
{
	assert((Object*)&_bullets==&_player+1);
	assert((Object*)&_enemies==&_player+1+kBulletsMax);
	// Movement functions.
	_enemy_functions[0]=Enemy1;
	_enemy_functions[1]=Enemy2;
	_enemy_functions[2]=Enemy3;
	_enemy_functions[3]=Enemy4;
	_enemy_functions[4]=Enemy5;
	_enemy_functions[5]=Enemy6;
	_enemy_functions[6]=Enemy7;
	_enemy_functions[7]=Enemy8;
}
bool Game::InputFire()
{
	if (_fire_delay)
	{
		--_fire_delay;
		return false;
	}
	_fire_delay=kPlayerFireDelay;
	if (_bullets_count==kBulletsMax)
		return false;
	_bullets[_bullets_count].x=_player.x;
	_bullets[_bullets_count].y=_player.y;
	_bullets[_bullets_count].x_old=_bullets[_bullets_count].x;
	_bullets[_bullets_count].y_old=_bullets[_bullets_count].y;
	_bullets_count++;
	return true;
}
void Game::InputDown() {}
void Game::InputLeft()
{
	if (_player.x>0)
		_player.x-=kPlayerSpeed;
}
void Game::InputRight()
{
	if (_player.x<kArenaSize)
		_player.x+=kPlayerSpeed;
}
void Game::InputUp() {}
Object Game::EventData() const
{
	return _event_data;
}
int Game::Level() const
{
	return _level;
}
void Game::LevelAdvance()
{
	++_level;
	if (_level>8)
		_level=1;
	LevelChange(_level);
}
void Game::LevelRestart()
{
	_player_visible=true;
	auto e=_enemies_count;
	LevelChange(_level);
	_enemies_count=e;
}
int Game::Lives() const
{
	return _lives;
}
void Game::RenderData(Object* p_out_array,size_t& p_out_size,size_t p_out_offsets[5]) const
{
	// Objects.
	p_out_array[0]=_player;
	memcpy(p_out_array+1,_bullets,sizeof(Object)*_bullets_count);
	memcpy(p_out_array+1+_bullets_count,_enemies,sizeof(Object)*_enemies_count);
	memcpy(p_out_array+1+_bullets_count+_enemies_count,_bombers,sizeof(Object)*2);
	memcpy(p_out_array+1+_bullets_count+_enemies_count+2,_missiles,sizeof(Object)*_missiles_count);
	p_out_size=ObjectsActiveCount();
	// Offsets.
	/*
	Values describes ranges in array. Index is the type.
	0 is player, 1 is bullets...
	*/
	p_out_offsets[0]=!_player_visible;
	p_out_offsets[1]=1;
	p_out_offsets[2]=1+_bullets_count;
	p_out_offsets[3]=1+_bullets_count+_enemies_count;
	p_out_offsets[4]=1+_bullets_count+_enemies_count+2;
}
int Game::Score() const
{
	return _score;
}
void Game::ScoreAdd(int s)
{
	_score+=s;
}
void Game::Start()
{
	_lives=3;
	_player.x=500;
	_player.y=800;
	_player_visible=true;
	_score=0;
	// Zeromem arrays and counts.
	memset(_bullets,0,(kObjectsMax-1)*sizeof(Object)+3*sizeof(size_t));
	LevelChange(STARTING_LEVEL);
}
bool Game::Started() const
{
	return _level;
}
void Game::Stop()
{
	_level=0;
	memset(_bullets,0,(kObjectsMax-1)*sizeof(Object)+3*sizeof(size_t));
}
Event Game::Update()
{
	Event result=NONE;
	if (__builtin_expect(!_level,0))
		return result;
	// Update positions for all objects.
	for (size_t i=0; i<kObjectsMax; ++i)
	{
		Object& o=(&_player)[i];
		o.x_old=o.x;
		o.y_old=o.y;
	}
	// Bullets.
	for (size_t i=0; i<_bullets_count; ++i)
	{
		Object& o=_bullets[i];
		o.y-=kPlayerBulletSpeed;
		// Check hits against enemies.
		for (size_t j=0; j<_enemies_count; ++j)
		{
			if (Intersect(o,_enemies[j]))
			{
				//std::cout<<"Bullet collided: "<<i<<" and "<<j<<'\n';
				o.y=-500;
				_score+=1;
				_event_data=_enemies[j];
				--_enemies_count;
				_enemies[j]=_enemies[_enemies_count];
				result+=ENEMY_HIT;
				if (!_enemies_count)
				{
					result+=LEVEL_COMPLETED;
				}
			}
		}
		// Check againt bombers.
		for (size_t j=0; j<2; ++j)
		{
			if (Intersect(o,_bombers[j]))
			{
				// Simply remove bullet.
				o.y=-500;
			}
		}
	}
	for (size_t i=0; i<_bullets_count; ++i)
	{
		Object& o=_bullets[i];
		if (o.y<0)
		{
			--_bullets_count;
			o=_bullets[_bullets_count];
		}
	}
	// Enemy level specific logic.
	(this->*_enemy_functions[_level-1])();
	// Enemy Movement.
	for (size_t i=0; i<_enemies_count; ++i)
	{
		Object& o=_enemies[i];
		o.x+=o.vx;
		o.y+=o.vy;
	}
	// Bombers.
	for (size_t i=0; i<2; ++i)
	{
		Object& o=_bombers[i];
		o.x+=o.vx;
		o.y+=o.vy;
		Clamp(o.x,o.x_old);
		Clamp(o.y,o.y_old);
		if (Intersect(o,_player))
		{
			result+=PLAYER_HIT;
		}
	}
	// Missiles.
	{
		// Spawn.
		if (_enemies_count && _missiles_count<kMissilesMax && rand()%5==0)
		{
			int dice=rand()%_enemies_count;
			Object& shooter=_enemies[dice];
			Object& m=_missiles[_missiles_count];
			m.x=m.x_old=shooter.x;
			m.y=m.y_old=shooter.y;
			++_missiles_count;
		}
		for (size_t i=0; i<_missiles_count; ++i)
		{
			Object& o=_missiles[i];
			o.y+=kEnemyMissileSpeed;
			if (Intersect(o,_player))
			{
				result+=PLAYER_HIT;
			}
			if (o.y>900)
			{
				--_missiles_count;
				_missiles[i]=_missiles[_missiles_count];
				--i;
			}
		}
	}
	if (result&PLAYER_HIT)
	{
		--_lives;
		_player_visible=false;
	}
	if (_lives<0)
	{
		result=GAME_OVER;
	}
	return result;
}
// =============================================================================
void Game::Enemy1()
{
	// Spiraling to the right.
	for (size_t i=0; i<_enemies_count; ++i, ++_enemy_timer)
	{
		Object& o=_enemies[i];
		o.vx=cos((_enemy_timer+i*2)/8.f)*16+4;
		o.vy=sin((_enemy_timer+i*2)/8.f)*15;
		Clamp(o.x,o.x_old);
	}
}
void Game::Enemy2()
{
	// Side to side.
	for (size_t i=0; i<_enemies_count; ++i)
	{
		Object& o=_enemies[i];
		o.vx=8-16*(i%2);
		o.vy=0;
		Clamp(o.x,o.x_old);
		Clamp(o.y,o.y_old);
	}
}
void Game::Enemy3()
{
	// Bounce around in diagonals.
	for (size_t i=0; i<_enemies_count; ++i)
	{
		Bounce(_enemies[i]);
	}
}
void Game::Enemy4()
{
	// Pogos.
	++_enemy_timer;
	for (size_t i=0; i<_enemies_count; ++i)
	{
		Object& o=_enemies[i];
		float a=-1.0f+(_enemy_timer%11)/5.0f;
		o.vy=15*tan(a);
		Bounce(_enemies[i]);
	}
}
void Game::Enemy5()
{
	// Space invaders type movement.
	for (size_t i=0; i<_enemies_count; ++i)
	{
		Object& o=_enemies[i];
		if (o.x>1000 || o.x<0)
			o.y+=60;
		if (o.y>800)
			o.y=60;
		Bounce(_enemies[i]);
	}
}
void Game::Enemy6()
{
	// Fly out from center of screen.
	for (size_t i=0; i<_enemies_count; ++i)
	{
		Object& o=_enemies[i];
		if (o.x>1000 || o.x<0 || o.y>1000 || o.y<0)
		{
			o.x_old=o.x=500;
			o.y_old=o.y=500;
			o.vx=rand()%20;
			o.vy=rand()%20;
			o.vx*=rand()%2?1:-1;
			o.vy*=rand()%2?1:-1;
		}
	}
}
void Game::Enemy7()
{
	// Move towards player.
	for (size_t i=0; i<_enemies_count; ++i)
	{
		Object& o=_enemies[i];
		o.vx=_player.x>o.x?5:-5;
		if (o.y>900)
			o.y_old=o.y=0;
	}
}
void Game::Enemy8()
{
	// Teleport around.
	++_enemy_timer;
	if (_enemy_timer==10)
	{
		_enemy_timer=0;
		for (size_t i=0; i<_enemies_count; ++i)
		{
			Object& o=_enemies[i];
			o.x=rand()%900+50;
			o.y=rand()%800+50;
			o.vx=rand()%10-5;
			o.vy=rand()%10-5;
		}
	}
}
void Game::LevelChange(int l)
{
	assert(l<9);
	_level=l;
	_enemies_count=kEnemiesMax;
	_enemy_timer=0;
	for (size_t i=0; i<_enemies_count; ++i)
	{
		Object& o=_enemies[i];
		o.x=rand()%900+100;
		o.y=rand()%400+200;
		o.vx=kEnemyStartingSpeeds[l-1][0];
		o.vy=kEnemyStartingSpeeds[l-1][1];
	}
	_bullets_count=0;
	for (size_t i=0; i<2; ++i)
	{
		Object& o=_bombers[i];
		o.x=500-i*300;
		o.y=0;
		o.vx=3;
		o.vy=8;
	}
	_missiles_count=0;
}
size_t Game::ObjectsActiveCount() const
{
	return 1+_bullets_count+_enemies_count+2+_missiles_count;
}
}
