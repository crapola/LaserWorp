// header
#include "particles.h"
// std
#include <algorithm> // for_each
#include <functional> // bind,mem_fn
#ifndef NDEBUG
#include <iostream>
#endif
const float kGravity=1.0f;
// Used for sorting.
bool Particles::Particle::operator<(Particle& b)
{
	return y<b.y;
}
Particles::Particle::Particle():x(500),y(1000),x_old(),y_old(),vx(),vy()
{
}
Particles::Particles():_array(),_end(),_offset(),_rand()
{
	std::random_device r;
	_rand.seed(r());
	_end=_array.begin();
}
Particles::~Particles()
{
}
void Particles::Clear()
{
	_array.fill(Particle());
	_offset=0;
}
void Particles::ExplosionSpawn(float x,float y)
{
	for (size_t i=_offset; i<_offset+kExplosionSize; ++i)
	{
		Particle& o=_array.at(i);
		o.x=x;
		o.y=y;
		RandomizeSpeed(o);
	}
	if (_offset<kParticlesMax-kExplosionSize)
	{
		_offset+=kExplosionSize;
	}
	else
	{
		_offset=0;
	}
}
void Particles::Update()
{
	// Sort by y.
	std::sort(_array.begin(),_array.end());
	// Active part stops at first y>900.
	_end=std::find_if(_array.begin(),_array.end(),[](Particle& p)
	{
		return p.y>900;
	});
	// Move active part.
	std::for_each(_array.begin(),_end,std::bind(std::mem_fn(&Particles::Move),this,std::placeholders::_1));
}
void Particles::RenderData(std::vector<ParticleView>& out_vector) const
{
	ptrdiff_t active_count=_end-_array.begin();
	out_vector.resize(active_count);
	for (ptrdiff_t i=0; i<active_count; ++i)
	{
		out_vector[i].x=_array.at(i).x;
		out_vector[i].y=_array.at(i).y;
		out_vector[i].x_old=_array.at(i).x_old;
		out_vector[i].y_old=_array.at(i).y_old;
	}
}
// =============================================================================
void Particles::Move(Particle& p)
{
	p.x_old=p.x;
	p.y_old=p.y;
	p.x+=p.vx;
	p.y+=p.vy;
	p.vy+=kGravity;
}
void Particles::RandomizeSpeed(Particle& p)
{
	std::uniform_real_distribution<float> uniform_dist(-10.0f,10.0f);
	p.vx=uniform_dist(_rand);
	p.vy=uniform_dist(_rand);
}