#version 400

uniform vec3 u_color;

in vec3 world_position;

void main(void)
{
	vec3 c = u_color;
	float grid_thickness = 0.03;

	bool on_grid = fract (world_position.x * 2.0) < grid_thickness ||
		fract (world_position.z * 2.0) < grid_thickness ||
		fract ((world_position.x - world_position.z) * 2.0) < grid_thickness;

	if (on_grid)
	{
		gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	}
	else {
		gl_FragColor = vec4(c, 1.0);
	}
}