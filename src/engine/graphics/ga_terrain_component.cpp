/*
** RPI Game Architecture 2017
** Final project - Ian Chamberlain
** 
** Terrain generator component
*/

#include "ga_terrain_component.h"
#include "ga_material.h"

#include "entity/ga_entity.h"

ga_terrain_component::ga_terrain_component(ga_entity* ent, const char* param_file) : ga_component(ent)
{
	// use unlit material for simplicity
	_material = new ga_constant_color_material();
	_material->init();
	_material->set_color({0.6f, 0.6f, 0.6f});

	// TODO actually generate the terrain
	static GLfloat vertices[] = {
		-10.0, -1.0, -10.0,
		 10.0, -1.0, -10.0,
		 10.0, -1.0,  10.0,
		-10.0, -1.0,  10.0,
	};

	static GLushort indices[] = {
		0,  1,  2,
		2,  3,  0,
	};

	// setup VBOs for drawing
	_index_count = uint32_t(sizeof(indices) / sizeof(*indices));

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// only need 2 buffers since we have constant color
	glGenBuffers(2, _vbos);

	glBindBuffer(GL_ARRAY_BUFFER, _vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	// possible implementation for infinite: keep a list of neighbors and generate them as needed
}

ga_terrain_component::~ga_terrain_component()
{
	// delete VBOs
	glDeleteBuffers(2, _vbos);
	glDeleteVertexArrays(1, &_vao);

	delete _material;
}

void ga_terrain_component::update(ga_frame_params * params)
{
	// draw the terrain each frame
	ga_static_drawcall draw;
	draw._name = "ga_terrain_component";
	draw._vao = _vao;
	draw._index_count = _index_count;
	draw._transform = get_entity()->get_transform();
	draw._draw_mode = GL_TRIANGLES;
	draw._material = _material;

	while (params->_static_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_static_drawcalls.push_back(draw);
	params->_static_drawcall_lock.clear(std::memory_order_release);
	
	// TODO generate further terrain based on camera position
}