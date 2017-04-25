#version 400

uniform mat4 u_mvp;
uniform mat4 u_proj;

layout(location = 0) in vec3 in_vertex;

out float height;
out vec3 world_position;

void main(void)
{
	gl_Position = vec4(in_vertex, 1.0) * u_mvp;
	world_position = in_vertex;
}