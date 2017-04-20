/*
** RPI Game Architecture 2017
** Final project - Ian Chamberlain
** 
** Terrain generator component
*/
#include <fstream>
#include <cassert>
#include <iostream>
#include <random>
#include <ctime>

#include "ga_terrain_component.h"
#include "ga_material.h"

#include "entity/ga_entity.h"

ga_terrain_component::ga_terrain_component(ga_entity* ent, const char* param_file) : ga_component(ent)
{
	// use unlit material for simplicity
	_material = new ga_directional_light_material();
	_material->init();
	_material->set_color({0.3f, 0.3f, 0.3f});

	// load the input file
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += param_file;

	std::ifstream file(fullpath);

	assert(file.is_open());

	// now assign parameters based on the input
	std::string cmd;
	while (file.peek() != EOF)
	{
		file >> cmd;
		if (cmd == "size")
		{
			file >> _size;
			// for convenience use 2^x + 1
			_size = std::pow(2, _size) + 1;
		}
		else if (cmd == "height")
		{
			file >> _height;
		}
		else {
			// Unknown input, error
			std::cerr << "Error parsing terrain file: '" << cmd <<
				"' not recognized" << std::endl;
			assert(false);
		}
	}

	// seed the random number generator
	std::srand((unsigned int) std::time(NULL));

	// and finally, generate a heightmap from our parameters
	_points = new float[_size * _size];
	generate_terrain();

	// use the newly generated _points to setup vbos for drawing
	setup_vbos();

	// possible implementation for infinite: keep a list of neighbors and generate them as needed
}

void ga_terrain_component::generate_terrain()
{
	// first initialize all the points to halfway

	for (int i = 0; i < _size; i++)
	{
		for (int j = 0; j < _size; j++)
		{
			float r = (float)std::rand() / (float)RAND_MAX;
			set_point(i, j, 0.5f);
		}
	}

	// recursively modify the terrain randomly
	subdivide_terrain(_size - 1);
}

void ga_terrain_component::subdivide_terrain(int size) {
	int x, y;
	int half = size / 2;

	// base case for recursive division
	if (half < 1)
		return;

	// first subdivide by squares
	for (y = half; y < _size - 2; y += size)
	{
		for (x = half; x < _size - 2; x += size)
		{
			square_offset(x, y, half);
		}
	}

	// then by diamonds
	for (y = 0; y <= _size - 2; y += half)
	{
		for (x = (y + half) % size; x <= _size - 2; x += size)
		{
			diamond_offset(x, y, half);
		}
	}

	// finally, the recursive step
	subdivide_terrain(half);
}

void ga_terrain_component::square_offset(int x, int y, int size)
{
	// first get the average of the square points
	float avg = get_point(x - size, y - size) +
		get_point(x + size, y - size) +
		get_point(x - size, y + size) +
		get_point(x + size, y + size);

	avg /= 4.0f;

	float offset = (float)(std::rand()) / (float) RAND_MAX - 0.5f;
	set_point(x, y, avg + offset);
}

void ga_terrain_component::diamond_offset(int x, int y, int size)
{
	float avg = get_point(x, y - size) +
		get_point(x - size, y) +
		get_point(x + size, y) +
		get_point(x, y + size);

	avg /= 4.0f;

	float offset = (float) (std::rand()) / (float) RAND_MAX - 0.5f;
	set_point(x, y, avg + offset);
}

void ga_terrain_component::setup_vbos()
{
	// setup indices and vertices for drawing
	int vertex_count = 3 * _size * _size;
	GLfloat* vertices = new GLfloat[vertex_count];

	std::cout << "vertices " << vertex_count / 3 << std::endl;
	
	// calculate x, y, z from _points data
	for (int i = 0; i < _size; i++)
	{
		for (int j = 0; j < _size; j++)
		{
			float x = (float) i - (_size / 2.0f);
			float y = get_point(i, j) * _height - _height / 2.0f;
			float z = (float) j - (_size / 2.0f);

			vertices[3 * (i + _size * j)] = x;
			vertices[3 * (i + _size * j) + 1] = y;
			vertices[3 * (i + _size * j) + 2] = z;
		}
	}

	// assign indices based on position in vertex array
	_index_count = 6 * (_size - 1) * (_size - 1);
	GLushort* indices = new GLushort[_index_count];
	std::cout << "indices " << _index_count << std::endl;

	int x = 0;
	int y = 0;
	for (int i = 0; i < _index_count; i += 6)
	{
		// assign one quad at a time
		indices[i] = x + _size * y;
		indices[i + 1] = x + 1 + _size * y;
		indices[i + 2] = x + 1 + _size * (y + 1);
		indices[i + 3] = x + 1 + _size * (y + 1);
		indices[i + 4] = x + _size * (y + 1);
		indices[i + 5] = x + _size * y;

		// advance coordinates to keep up
		x = (x + 1);
		if (x >= _size - 1)
		{
			x = 0;
			y++;
		}
	}

	// and use that data for the arrays
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// only need 2 buffers since we have constant color
	glGenBuffers(2, _vbos);

	glBindBuffer(GL_ARRAY_BUFFER, _vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_count, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * _index_count, indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	// clean up our temporary arrays
	delete[] indices;
	delete[] vertices;
}

ga_terrain_component::~ga_terrain_component()
{
	// delete VBOs
	glDeleteBuffers(2, _vbos);
	glDeleteVertexArrays(1, &_vao);

	delete _material;
	delete[] _points;
}

// getter/setter for _points
float ga_terrain_component::get_point(int x, int y)
{
	if (y * _size + x < _size * _size)
		return _points[y * _size + x];
	else
		return 0.5f;
}

void ga_terrain_component::set_point(int x, int y, float height)
{
	assert(y * _size + x < _size * _size);
	_points[y * _size + x] = height;
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