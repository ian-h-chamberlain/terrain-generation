/*
** RPI Game Architecture 2017
** Final project - Ian Chamberlain
** 
** Terrain generator component
*/

#include "ga_terrain_component.h"
#include "ga_material.h"


ga_terrain_component::ga_terrain_component(ga_entity* ent, const char* param_file) : ga_component(ent)
{
	_material = new ga_constant_color_material();
	_material->init();
	_material->set_color({0.6f, 0.6f, 0.6f});

	// TODO actually generate the terrain
	static GLfloat vertices[] = {
		-1.0, -1.0, -1.0,
		 1.0, -1.0, -1.0,
		 1.0, -1.0,  1.0,
		-1.0, -1.0,  1.0,
	};

	static GLushort indices[] = {
		0,  1,  2,
		2,  3,  0,
	}

	// TODO setup VBOs for drawing

	// possible implementation for infinite: keep a list of neighbors and generate them as needed
}

ga_terrain_component::~ga_terrain_component()
{
	// TODO delete VBOs

	delete _material;
}

void ga_terrain_component::update(ga_frame_params * params)
{
	// TODO draw terrain

	// TODO generate further terrain based on camera position?
}