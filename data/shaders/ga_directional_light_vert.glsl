#version 400

uniform mat4 u_mvp;
uniform mat4 u_proj;

layout(location = 0) in vec3 in_vertex;

out float height;

void main(void)
{
	gl_Position = vec4(in_vertex, 1.0) * u_mvp;
	height = clamp(in_vertex.y, 0.3, 1.0);
}