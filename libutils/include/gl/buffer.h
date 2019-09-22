#pragma once
#include "../../src/gl/glid.hpp"
namespace gl
{
class Buffer: public Identifiable
{
public:
	Buffer();
	~Buffer();
	void Bind(GLenum target) const;
};
}
