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
		else
		{
			// Unknown input, error
			std::cerr << "Error parsing terrain file: '" << cmd <<
				"' not recognized" << std::endl;
			assert(false);
		}
	}

	// seed the random number generator
	std::srand((unsigned int) std::time(NULL));

	// setup Perlin noise implementation

	// and finally, generate a heightmap from our parameters
	_points = new float[_size * _size];
	generate_terrain();

	// use the newly generated _points to setup vbos for drawing
	setup_vbos();

	// possible implementation for infinite: keep a list of neighbors and generate them as needed
}

void ga_terrain_component::generate_terrain()
{
	// initialize points pseudarandomly with Perlin noise
	for (int i = 0; i < _size; i++)
	{
		for (int j = 0; j < _size; j++)
		{
			float x = (float) i - (_size / 2.0f);
			float y = (float) j - (_size / 2.0f);

			set_point(i, j, noise(x, y, 0.5f));
		}
	}
}

// Perlin noise generator - based on the reference implementation at 
// http://mrl.nyu.edu/~perlin/noise/
float ga_terrain_component::noise(float x, float y, float z)
{
	int X = (int)std::floor(x) & 255,                  // FIND UNIT CUBE THAT
		Y = (int)std::floor(y) & 255,                  // CONTAINS POINT.
		Z = (int)std::floor(z) & 255;

	x -= std::floor(x);                                // FIND RELATIVE X,Y,Z
	y -= std::floor(y);                                // OF POINT IN CUBE.
	z -= std::floor(z);

	double  u = fade(x),                                // COMPUTE FADE CURVES
			v = fade(y),                                // FOR EACH OF X,Y,Z.
			w = fade(z);

	int A = get_p(X  ) + Y, AA = get_p(A) + Z, AB = get_p(A + 1) + Z,      // HASH COORDINATES OF
		B = get_p(X+1) + Y, BA = get_p(B) + Z, BB = get_p(B + 1) + Z;      // THE 8 CUBE CORNERS,

	return lerp(w, lerp(v, lerp(u,  grad(get_p(AA  ), x  , y  , z  ),  // AND ADD
									grad(get_p(BA  ), x-1, y  , z  )), // BLENDED
							lerp(u, grad(get_p(AB  ), x  , y-1, z  ),  // RESULTS
									grad(get_p(BB  ), x-1, y-1, z  ))),// FROM  8
					lerp(v, lerp(u, grad(get_p(AA+1), x  , y  , z-1),  // CORNERS
									grad(get_p(BA+1), x-1, y  , z-1)), // OF CUBE
							lerp(u, grad(get_p(AB+1), x  , y-1, z-1),
									grad(get_p(BB+1), x-1, y-1, z-1))));
}

float ga_terrain_component::grad(int hash, float x, float y, float z)
{
	int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
	double u = h<8 ? x : y,                 // INTO 12 GRADIENT DIRECTIONS.
		v = h<4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

// initialization vector for Perlin noise
const int ga_terrain_component::get_permutation(int i)
{
	static const int permutation[] = 
	{
		151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};

	return permutation[i];
}

// Helper for Perlin noise initialization vector
const int ga_terrain_component::get_p(int i)
{
	if (i < 256)
	{
		return get_permutation(i);
	}
	return get_permutation(i - 256);
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
