#pragma once
#include <GL/glew.h>
namespace gl
{
class Identifiable
{
public:
	operator GLuint() const
	{
		return _id;
	}
protected:
	Identifiable():_id(0){}
	explicit Identifiable(const GLuint id):_id(id){}
	Identifiable(const Identifiable&)=delete;
	Identifiable& operator=(const Identifiable&)=delete;
	~Identifiable(){}
	GLuint _id;
};
}
