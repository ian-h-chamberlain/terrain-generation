#version 400

uniform vec3 u_color;
uniform vec3 u_light_direction;

in float height;

void main(void)
{
	vec3 c = u_color * height;
	gl_FragColor = vec4(c, 1.0);
}