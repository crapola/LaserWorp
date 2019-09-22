#pragma once
#include "../../src/gl/glid.hpp"
namespace gl
{
class Vao: public Identifiable
{
public:
	Vao();
	~Vao();
	void Bind() const;
	static void Unbind();
};
}
