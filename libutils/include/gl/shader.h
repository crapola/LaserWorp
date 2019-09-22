#pragma once
#include "../../src/gl/glid.hpp"
#include <string>
namespace gl
{
class Shader: public Identifiable
{
public:
	Shader(GLenum p_type);
	~Shader();
	void Compile(const char* source) const;
	void Compile(const std::string& source) const;
};
}
