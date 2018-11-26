#version 450

layout(location=0) in vec3 in_position;
layout(location=1) in vec3 in_color;
layout(location=2) in mat4 in_matrix; // Instanced

//layout(location=0) uniform mat4 transform;

out vec3 vert_color;

void main()
{
	//gl_Position=transform*vec4(in_position,1.0f);
	gl_Position=in_matrix*vec4(in_position,1.0f);
	vert_color=in_color;
}
