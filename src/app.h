/*******************************************************************************
Application
*******************************************************************************/
#pragma once
// std
#include <vector>
// libs
#include <SDL.h>
#include "gfx/window.h"
#include "gl/buffer.h"
#include "gl/program.h"
#include "gl/vao.h"
#include "smalltext/smalltext.h"
// local
#include "game.h"
#include "geometry.h"
#include "particles.h"
struct App
{
	App(Window&);
	App(const App&)=delete;
	void operator=(const App&)=delete;
	~App();
	void Init();
	void Render(float);
	bool Update();
private:
	void GameUpdate();
	// Main game HUD.
	void HudHide();
	void HudLivesDisplay();
	void HudShow();
	// Refresh lerp status at bottom.
	void LerpDisplay();
	// App/Menu states.
	void MenuCompletedEnter();
	void MenuCompletedExit();
	void MenuCompletedUpdate();
	void MenuLevelEnter();
	void MenuLevelExit();
	void MenuLevelUpdate();
	void MenuNothingUpdate();
	void MenuOverEnter();
	void MenuOverExit();
	void MenuOverUpdate();
	void MenuPlayerHitEnter();
	void MenuPlayerHitExit();
	void MenuPlayerHitUpdate();
	void MenuTitleEnter();
	void MenuTitleExit();
	void MenuTitleUpdate();
	// Update render from game and particle state.
	void RenderFrameUpdate();
	// Refresh score on the HUD.
	void ScoreDraw();
	// Build render matrix array.
	void UpdateMatrices(float);
	void UpdateMatricesNoLerp();
	// Returns true if we are waiting.
	bool Wait();
	// Constants.
	static const size_t kObjectsMax=laserworp::Game::kObjectsMax;
	static const size_t kParticlesMax=Particles::kParticlesMax;
	static const size_t kRenderablesMax=kObjectsMax+kParticlesMax;
	// Interpolated attributes of rendered meshes.
	struct RenderFrame
	{
		Point positions[kRenderablesMax];
		Point scales[kRenderablesMax];
	} _frame_new,_frame_old;
	const Uint8* _keyboard_state;
	Window& _window;
	SmallText _smalltext;
	// OpenGL stuff.
	gl::Program _prog;
	gl::Vao _vao;
	gl::Buffer _buffer_vertex,_buffer_elements,_buffer_matrices;
	Matrix _matrices[kRenderablesMax];
	// Number of object matrices in _matrices.
	size_t _objects_count;
	// Number of particle matrices in _matrices.
	size_t _particles_count;
	/*
	_instance_offsets is used to index in _matrices according to mesh type.
	0=Player
	1=Bullet
	2=Enemy
	3=Bombers
	4=Missiles
	5=Particles (=_objects_count)
	6=end (=_objects_count+_particles_count)
	*/
	size_t _instance_offsets[7];
	laserworp::Game _game;
	Particles _particles;
	bool _lerp_switch;
	bool _game_running;
	int _freeze;
	// Text handles.
	SmallText::Handle* _hud,*_title,*_completed,*_level,*_over,*_lerp;
	// Current menu update function.
	void (App::* _menu_state)();
	// Cached particle data, used in RenderFrameUpdate().
	std::vector<ParticleView> _particle_data;
	// Player stats.
	int _shots_fired,_shots_hit;
};
