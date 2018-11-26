/*******************************************************************************
Game
*******************************************************************************/
#pragma once
#include <cstddef> // size_t
#include <cstdint> // uint8_t
namespace laserworp
{
enum Event: uint8_t
{
	NONE=0,
	ENEMY_HIT=1<<0,
	LEVEL_COMPLETED=1<<1,
	PLAYER_HIT=1<<2,
	GAME_OVER=1<<3
};
// Game object.
struct Object
{
	int x,y;
	int x_old,y_old; // Used for interpolation.
	int vx,vy;
};
// Game logic.
class Game
{
	static constexpr size_t kBulletsMax=4,kEnemiesMax=8,kBombersMax=2,kMissilesMax=4;
public:
	static constexpr size_t kObjectsMax=1+kBulletsMax+kEnemiesMax+kBombersMax+kMissilesMax;
	Game();
	void InputDown();
	// Returns true when a new bullet is shot.
	bool InputFire();
	void InputLeft();
	void InputRight();
	void InputUp();
	// Retrieve data related to last event.
	Object EventData() const;
	int Level() const;
	void LevelAdvance();
	void LevelRestart();
	int Lives() const;
	// Get active objects to render.
	void RenderData(Object* out_array,size_t& out_count,size_t out_offsets[5]) const;
	int Score() const;
	void ScoreAdd(int);
	void Start();
	bool Started() const;
	void Stop();
	Event Update();
private:
	void Enemy1();
	void Enemy2();
	void Enemy3();
	void Enemy4();
	void Enemy5();
	void Enemy6();
	void Enemy7();
	void Enemy8();
	void LevelChange(int level);
	size_t ObjectsActiveCount() const;
	// Game data.
	int _lives;
	int _level;
	int _score;
	Object _player;
	Object _bullets[kBulletsMax];
	Object _enemies[kEnemiesMax];
	Object _bombers[kBombersMax];
	Object _missiles[kMissilesMax];
	size_t _bullets_count;
	size_t _enemies_count;
	size_t _missiles_count;
	// Player visible.
	bool _player_visible;
	// Player fire delay timer.
	int _fire_delay;
	// Enemy behaviors.
	typedef void (Game::* Function)();
	Function _enemy_functions[8];
	// Enemies inital vx,vy for each level.
	static const int kEnemyStartingSpeeds[8][2];
	// Most recent event data.
	Object _event_data;
	// Extra enemy behavior variables.
	int _enemy_timer;
};
}
