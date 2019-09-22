// header
#include "app.h"
// std
#include <iostream>
#include <sstream>
// libs
#include "gl/shader.h"
#include "misc/loadstring.h"
// local
#include "app_strings.h"
#include "audio/audio.h"
using namespace std;
// Convert coordinates from game space to NDC.
void Convert(float& __restrict__ x,float& __restrict__ y)
{
	x=x/500.0f-1.0f;
	y=1.0f-y/500.0f;
}
App::App(Window& p_w):_frame_new(),_frame_old(),_keyboard_state(),_window(p_w),_smalltext(),_prog(),_vao(),
	_buffer_vertex(),_buffer_elements(),_buffer_matrices(),_objects_count(),_particles_count(),_game(),_particles(),
	_lerp_switch(true),_game_running(false),_freeze(),_hud(),_title(),_completed(),_level(),_over(),_lerp(),_menu_state(),_particle_data(kParticlesMax),
	_shots_fired(),_shots_hit()
{
}
App::~App()
{
	audio::DeviceClose();
}
void App::Init()
{
	// Enable color blending.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Enable backface culling.
	// Reminder: verts have to be counter clockwise.
	glEnable(GL_CULL_FACE);
	// 9 possible points around origin.
	Point p1(-1, 1, 0),p2(0, 1, 0),p3(1, 1, 0);
	Point p4(-1, 0, 0),p5(0, 0, 0),p6(1, 0, 0);
	Point p7(-1,-1, 0),p8(0,-1, 0),p9(1,-1, 0);
	// Colors.
	Point c_red(1,0,0),c_green(0,1,0),c_blue(0,0,1),
		  c_yellow(1,1,0),c_purple(1,0,1);
	// Define meshes from those.
	Vertex verts[]=
	{
		{p7,c_red},{p2,c_green},{p5,c_red},{p9,c_blue},// Player ship
		{p7,c_red},{p2,c_red},{p8,c_yellow},{p9,c_red},// Player bullet
		{p4,c_red},{p2,c_green},{p8,c_blue},{p6,c_green},// Enemy
		{p1,c_red},{p5,c_green},{p8,c_blue},{p3,c_green},// Bomber
		{p1,c_red},{p2,c_yellow},{p8,c_green},{p3,c_red},// Enemy bullet
		{p4,c_red},{p2,c_red},{p8,c_yellow},{p6,c_red}// Particle
	};
	// Meshes all share the same elements array.
	GLushort elements[]= {0,2,1,1,2,3};
	{
		// Log some extra info.
		cout<<"Vertex stuct size:"<<sizeof(Vertex)<<'\n';
		std::cout<<"Shader version: "<<glGetString(GL_SHADING_LANGUAGE_VERSION)<<'\n';
		int m=0;
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE,&m);
		cout<<"Max uniform block size: "<< (m>>10) <<" KB.\n";
	}
	// Compile shaders.
	gl::Shader vs(GL_VERTEX_SHADER),fs(GL_FRAGMENT_SHADER);
	vs.Compile(LoadString("data/vertex.glsl"));
	fs.Compile(LoadString("data/fragment.glsl"));
	_prog.Attach(vs);
	_prog.Attach(fs);
	_prog.Link();
	// Upload data.
	// Vertices.
	_buffer_vertex.Bind(GL_ARRAY_BUFFER);
	glBufferData(GL_ARRAY_BUFFER,sizeof(verts),verts,GL_STATIC_DRAW);
	// Elements.
	_buffer_elements.Bind(GL_ELEMENT_ARRAY_BUFFER);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(elements),elements,GL_STATIC_DRAW);
	// Matrices.
	_buffer_matrices.Bind(GL_ARRAY_BUFFER);
	glBufferData(GL_ARRAY_BUFFER,sizeof(_matrices),_matrices,GL_DYNAMIC_DRAW);
	// Attributes.
	// VAOs are required in Core context.
	_vao.Bind();
	_buffer_vertex.Bind(GL_ARRAY_BUFFER);
	_buffer_elements.Bind(GL_ELEMENT_ARRAY_BUFFER);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),reinterpret_cast<GLvoid*>(offsetof(Vertex,color)));
	// Matrix attribs 2, 3, 4, 5
	const GLsizei size_of_vec4=sizeof(glm::vec4);
	_buffer_matrices.Bind(GL_ARRAY_BUFFER);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(2,4,GL_FLOAT,GL_FALSE,4*size_of_vec4,reinterpret_cast<GLvoid*>(0*size_of_vec4));
	glVertexAttribPointer(3,4,GL_FLOAT,GL_FALSE,4*size_of_vec4,reinterpret_cast<GLvoid*>(1*size_of_vec4));
	glVertexAttribPointer(4,4,GL_FLOAT,GL_FALSE,4*size_of_vec4,reinterpret_cast<GLvoid*>(2*size_of_vec4));
	glVertexAttribPointer(5,4,GL_FLOAT,GL_FALSE,4*size_of_vec4,reinterpret_cast<GLvoid*>(3*size_of_vec4));
	// Make attributes instanced. Divisor=1 per instance.
	glVertexAttribDivisor(2,1);
	glVertexAttribDivisor(3,1);
	glVertexAttribDivisor(4,1);
	glVertexAttribDivisor(5,1);
	// Text.
	_smalltext.Resolution(200,300);
	_lerp=&_smalltext.Add("Interpolation:ON ");
	_smalltext.Paragraph(*_lerp,0,280,17);
	_smalltext.SetColor(*_lerp,0,17,Color2b(2,2,2,3),0);
	LerpDisplay();
	// Init keyboard.
	_keyboard_state=SDL_GetKeyboardState(nullptr);
	// Init audio.
	if (!audio::DeviceOpen())
		std::cout<<"Audio isn't working.";
	// Go to state Title.
	MenuTitleEnter();
}
void App::Render(float p_lerpFactor)
{
	// Build and interpolate matrices.
	if (_lerp_switch)
		UpdateMatrices(p_lerpFactor);
	else
		UpdateMatricesNoLerp();
	// Bind our VAO and shader program.
	_vao.Bind();
	_prog.Bind();
	// Upload the whole matrices array.
	_buffer_matrices.Bind(GL_ARRAY_BUFFER);
	glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(_matrices),_matrices);
	// Clear background.
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	if (_objects_count)
	{
		//std::cout<<"Drawing "<<_matrices_count<<" matrices\n";
		GLsizei draw_count;
		_instance_offsets[5]=_objects_count;
		_instance_offsets[6]=_objects_count+_particles_count;
		for (size_t mesh_id=0; mesh_id<6; ++mesh_id)
		{
			GLint base_vert=mesh_id*4; // Each mesh has four vertices in the buffer.
			GLuint base_instance=_instance_offsets[mesh_id];
			draw_count=_instance_offsets[mesh_id+1]-_instance_offsets[mesh_id];
			glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,0,draw_count,base_vert,base_instance);
			//std::cout<<"Draw Mesh ID:"<<mesh_id<<" Count:"<<draw_count<<" Index:"<<base_instance<<'\n';
		}
	}
	// Not necessary.
	_vao.Unbind();
	_prog.Unbind();
	// Draw text.
	_smalltext.Draw();
	// Swap.
	SDL_GL_SwapWindow(_window);
}
bool App::Update()
{
	// Window events.
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			return false;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				return false;
				break;
			case SDLK_LEFT:
				break;
			case SDLK_RIGHT:
				break;
			case SDLK_UP:
				break;
			case SDLK_DOWN:
				break;
			case SDLK_F1:
				break;
			case SDLK_f:
				_particles.ExplosionSpawn(500,500);
				break;
			case SDLK_n:
				MenuCompletedEnter();
				break;
			case SDLK_SPACE:
				break;
			default:
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_i:
				_lerp_switch=!_lerp_switch;
				LerpDisplay();
				break;
			case SDLK_RIGHT:
				break;
			case SDLK_UP:
				break;
			case SDLK_DOWN:
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
	if (Wait())
		return true;
	(this->*_menu_state)();
	if (_game_running)
	{
		GameUpdate();
		_particles.Update();
		RenderFrameUpdate();
	}
	return true;
}
// =============================================================================
void App::GameUpdate()
{
	// Run game logic.
	laserworp::Event result=_game.Update();
	// Handle game events. The order of if blocks matters.
	if (result&laserworp::ENEMY_HIT)
	{
		std::cout<<"Enemy hit!\n";
		ScoreDraw();
		audio::Beep(250);
		auto o=_game.EventData();
		_particles.ExplosionSpawn(o.x,o.y);
		++_shots_hit;
	}
	if (result&laserworp::PLAYER_HIT)
	{
		std::cout<<"Player was hit.\n";
		MenuPlayerHitEnter();
	}
	if (result&laserworp::LEVEL_COMPLETED)
	{
		std::cout<<"Level completed!\n";
		MenuCompletedEnter();
	}
	if (result&laserworp::GAME_OVER)
	{
		std::cout<<"It's over!\n";
		MenuOverEnter();
	}
	// Input.
	if (_keyboard_state[SDL_SCANCODE_LEFT])
	{
		_game.InputLeft();
	}
	if (_keyboard_state[SDL_SCANCODE_RIGHT])
	{
		_game.InputRight();
	}
	if (_keyboard_state[SDL_SCANCODE_SPACE])
	{
		if (_game.InputFire())
		{
			audio::Beep(440);
			++_shots_fired;
		}
	}
}
void App::HudHide()
{
	_smalltext.Delete(*_hud);
}
void App::HudLivesDisplay()
{
	_smalltext.SetColor(*_hud,20+16,3,Color2b(1,0,0,3),2);
	_smalltext.SetColor(*_hud,20+16,_game.Lives(),Color2b(3,0,0,3),2);
}
void App::HudShow()
{
	_hud=&_smalltext.Add(kHud);
	_smalltext.Paragraph(*_hud,0,0,20);
	_smalltext.SetColor(*_hud,0,20*4,Color2b(3,3,3,3),2);
	_smalltext.SetColor(*_hud,20+2,6,Color2b(0,3,0,3),2);// SCORE= green.
	_smalltext.SetColor(*_hud,40+2,8,Color2b(3,0,2,3),2);// SECTION= purple.
	_smalltext.Write(*_hud,20+16,"\xf4\xf4\xf4");
	HudLivesDisplay();
}
void App::LerpDisplay()
{
	static const char* captions[2]= {"OFF","ON "};
	static const uint8_t colors[2]= {Color2b(2,0,0,3),Color2b(0,2,0,3)};
	size_t i=+_lerp_switch;
	_smalltext.Write(*_lerp,14,captions[i]);
	_smalltext.SetColor(*_lerp,14,3,colors[i],0);
}
void App::MenuCompletedEnter()
{
	assert(_game.Level()>0 && _game.Level()<9 && _shots_fired>0);
	_completed=&_smalltext.Add(kLevelCompleted);
	_smalltext.Paragraph(*_completed,0,128,25);
	std::stringstream ss;
	ss<<"Sheet "<<kNumbers[_game.Level()-1]<<" completed";
	_smalltext.Write(*_completed,1*25+2,ss.str().c_str());
	// Colorize all blue.
	_smalltext.SetColor(*_completed,0,25*11,Color2b(1,2,3,3),2);
	// White title.
	_smalltext.SetColor(*_completed,1*25+2,16+5,Color2b(3,3,3,3),2);
	_smalltext.SetColor(*_completed,8*25+1,22,Color2b(3,3,3,3),2);
	// Lines in darker blue.
	_smalltext.SetColor(*_completed,0,25,Color2b(0,1,2,3),2);
	_smalltext.SetColor(*_completed,2*25,25,Color2b(0,1,2,3),2);
	_smalltext.SetColor(*_completed,9*25,2*25,Color2b(0,1,2,3),2);
	// Green numbers.
	_smalltext.SetColor(*_completed,3*25+14,4,Color2b(0,3,0,3),2);
	_smalltext.SetColor(*_completed,4*25+14,4,Color2b(0,3,0,3),2);
	_smalltext.SetColor(*_completed,6*25+14,4,Color2b(0,3,0,3),2);
	// Level bonus.
	ss.str(std::string());
	int level_bonus=_game.Level()*10;
	ss<<level_bonus;
	_smalltext.Write(*_completed,3*25+15,ss.str().c_str());
	// Accuracy bonus.
	ss.str(std::string());
	int accuracy=100*_shots_hit/_shots_fired;
	ss<<accuracy;
	_smalltext.Write(*_completed,4*25+15,ss.str().c_str());
	// Total.
	ss.str(std::string());
	int total=level_bonus+accuracy;
	ss<<total;
	_smalltext.Write(*_completed,6*25+15,ss.str().c_str());
	// Apply and refresh score.
	_game.ScoreAdd(total);
	ScoreDraw();
	_shots_fired=_shots_hit=0;
	_freeze=30;
	_menu_state=MenuCompletedUpdate;
	_game_running=false;
}
void App::MenuCompletedExit()
{
	_smalltext.Delete(*_completed);
	_completed=nullptr;
	_game.LevelAdvance();
	_game_running=true;
	MenuLevelEnter();
}
void App::MenuCompletedUpdate()
{
	if (!_freeze)
	{
		if (_keyboard_state[SDL_SCANCODE_SPACE])
		{
			MenuCompletedExit();
		}
	}
}
void App::MenuLevelEnter()
{
	int l=_game.Level();
	std::stringstream ss;
	ss<<"Section "<<kNumbers[_game.Level()-1];
	_level=&_smalltext.Add(ss.str().c_str());
	_smalltext.Paragraph(*_level,8*6,128,16);
	// Update section.
	ss.str(std::string());
	ss<<l;
	_smalltext.Write(*_hud,40+10,ss.str().c_str());
	_freeze=20;
	_objects_count=0;
	_particles.Clear();
	_menu_state=MenuLevelUpdate;
	_game_running=false;
}
void App::MenuLevelExit()
{
	_smalltext.Delete(*_level);
	_level=nullptr;
	_menu_state=MenuNothingUpdate;
	_game_running=true;
}
void App::MenuLevelUpdate()
{
	if (!_freeze)
	{
		MenuLevelExit();
	}
}
void App::MenuNothingUpdate()
{
}
void App::MenuOverEnter()
{
	audio::Beep(110);
	_over=&_smalltext.Add("Game Over");
	_smalltext.Paragraph(*_over,8*(25/2-9/2),128,10);
	_freeze=30;
	_game.Stop();
	_menu_state=MenuOverUpdate;
}
void App::MenuOverExit()
{
	_smalltext.Delete(*_over);
	MenuTitleEnter();
}
void App::MenuOverUpdate()
{
	if (!_freeze)
		MenuOverExit();
}
void App::MenuPlayerHitEnter()
{
	audio::Beep(220);
	HudLivesDisplay();
	_freeze=30;
	_menu_state=MenuPlayerHitUpdate;
}
void App::MenuPlayerHitExit()
{
	_game.LevelRestart();
	_particles.Clear();
	_menu_state=MenuNothingUpdate;
}
void App::MenuPlayerHitUpdate()
{
	if (!_freeze)
	{
		MenuPlayerHitExit();
	}
}
void App::MenuTitleEnter()
{
	_title=&_smalltext.Add(kTitle);
	_smalltext.Paragraph(*_title,0,64,25);
	_smalltext.SetColor(*_title,2*25,25*2,Color2b(3,2,0,3),0);
	_smalltext.SetColor(*_title,4*25,25*3,Color2b(1,2,0,3),0);
	_smalltext.SetColor(*_title,4*25+1,1,Color2b(0,3,3,3),Color2b(0,0,3,3));
	_smalltext.SetColor(*_title,4*25+3,1,Color2b(0,3,3,3),Color2b(0,0,3,3));
	_smalltext.SetColor(*_title,5*25+1,1,Color2b(0,3,3,3),Color2b(0,0,3,3));
	_smalltext.SetColor(*_title,6*25+1,1,Color2b(0,3,3,3),Color2b(0,0,3,3));
	HudHide();
	_menu_state=MenuTitleUpdate;
	_objects_count=0;
	_particles_count=0;
	_game_running=false;
}
void App::MenuTitleExit()
{
	_smalltext.Delete(*_title);
	_title=nullptr;
	HudShow();
	MenuLevelEnter();
}
void App::MenuTitleUpdate()
{
	// Flash.
	int c=rand()|0x3;
	_smalltext.SetColor(*_title,8*25+1,19,c,0);
	if (_keyboard_state[SDL_SCANCODE_SPACE])
	{
		_game.Start();
		//audio::Beep(698); // 698 Hz=CPC beep.
		MenuTitleExit();
	}
}
void App::RenderFrameUpdate()
{
	static_assert(kObjectsMax>=laserworp::Game::kObjectsMax);
	// Game objects
	// ------------
	laserworp::Object active_objects[kObjectsMax];
	size_t active_objects_count=0;
	size_t offsets[5];
	_game.RenderData(active_objects,active_objects_count,offsets);
	constexpr Point sizes[5]=
	{
		{0.125f,0.125f,0.0f},
		{1.0f/16.0f,1.0f/16.0f,0.0f},
		{0.125f,0.125f,0.0f},
		{0.125f,0.125f,0.0f},
		{1.0f/16.0f,1.0f/16.0f,0.0f}
	};
	memcpy(_instance_offsets,offsets,sizeof(offsets));
	// Build arrays for new attributes.
	size_t current_thing=0;
	for (size_t i=0; i<active_objects_count; ++i)
	{
		float x=active_objects[i].x;
		float y=active_objects[i].y;
		Convert(x,y);
		Point pos(x,y,0.f);
		/*
		Determine what kind of object it is.
		If we don't test for current_thing<4 here, we get undefined behavior,
		even if we know it won't be higher.
		*/
		while (current_thing<4 && i>=offsets[current_thing+1])
		{
			++current_thing;
		}
		Point scale=sizes[current_thing];
		_frame_new.positions[i]=pos;
		_frame_new.scales[i]=scale;
		x=active_objects[i].x_old;
		y=active_objects[i].y_old;
		Convert(x,y);
		pos= {x,y,0.0f};
		_frame_old.positions[i]=pos;
	}
	_objects_count=active_objects_count;
	// Particles
	// ---------
	_particles.RenderData(_particle_data);
	_particles_count=_particle_data.size();
	for (size_t i=0; i<_particles_count; ++i)
	{
		Point p(_particle_data.at(i).x,_particle_data.at(i).y,0);
		Point p_old(_particle_data.at(i).x_old,_particle_data.at(i).y_old,0);
		Convert(p.x,p.y);
		Convert(p_old.x,p_old.y);
		_frame_new.positions[_objects_count+i]=p;
		_frame_old.positions[_objects_count+i]=p_old;
		_frame_new.scales[_objects_count+i]=Point(1.0f/32.0f,1.0f/32.0f,0.0f);
	}
}
void App::ScoreDraw()
{
	std::stringstream ss;
	ss<<_game.Score();
	_smalltext.Write(*_hud,20+8,ss.str().c_str());
}
void App::UpdateMatrices(float p)
{
	for (size_t i=0; i<_objects_count+_particles_count; ++i)
	{
		Matrix m;
		Point pos=glm::mix(_frame_old.positions[i],_frame_new.positions[i],p);
		//Point scale=glm::mix(_scales_old[i],_scales_new[i],p);
		Point scale=_frame_new.scales[i];
		m=glm::translate(m,pos);
		m=glm::scale(m,scale);
		_matrices[i]=m;
	}
}
void App::UpdateMatricesNoLerp()
{
	for (size_t i=0; i<_objects_count+_particles_count; ++i)
	{
		Matrix m;
		m=glm::translate(m,_frame_new.positions[i]);
		m=glm::scale(m,_frame_new.scales[i]);
		_matrices[i]=m;
	}
}
bool App::Wait()
{
	if (_freeze)
	{
		--_freeze;
		return true;
	}
	return false;
}
