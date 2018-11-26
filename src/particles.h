/*******************************************************************************
Particles
*******************************************************************************/
#pragma once
#include <array>
#include <random>
#include <vector>
struct ParticleView
{
	float x,y;
	float x_old,y_old;
};
class Particles
{
	struct Particle
	{
		Particle();
		bool operator<(Particle&);
		float x,y;
		float x_old,y_old;
		float vx,vy;
	};
public:
	static constexpr size_t kExplosionSize=32;
	static constexpr size_t kParticlesMax=4*kExplosionSize;
	Particles();
	Particles(const Particles&)=delete;
	void operator=(const Particles&)=delete;
	~Particles();
	void Clear();
	void ExplosionSpawn(float x,float y);
	void Update();
	void RenderData(std::vector<ParticleView>& out_vector) const;
private:
	void Move(Particle& p);
	void RandomizeSpeed(Particle&);
	std::array<Particle,kParticlesMax> _array;
	std::array<Particle,kParticlesMax>::iterator _end;
	size_t _offset;
	std::default_random_engine _rand;
};