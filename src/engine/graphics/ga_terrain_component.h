
#include "entity/ga_component.h"
#include "framework/ga_camera.h"

#include <random>

class ga_terrain_component : public ga_component
{
public:
	ga_terrain_component(class ga_entity* ent, const char* param_file, class ga_camera* cam);
	virtual ~ga_terrain_component();

	virtual void update(struct ga_frame_params* params) override;

private:
	static class ga_wireframe_material* _material;
	uint32_t _vao;
	uint32_t _vbos[4];
	uint32_t _index_count;

	// a reference to the player object so we can dynamically generate terrain
	ga_camera* _camera;

	// Terrain representation
	int _size;
	float _width;
	int _height;
	float _radius;
	float* _points;
	ga_vec2f _position;
	const char* _param_file;

	// storage for surrounding terrain tiles
	ga_terrain_component* _parent;
	ga_terrain_component* _neighbors[4];

	// some helper getters/setters
	float get_point(int x, int y);
	void set_point(int x, int y, float height);
	ga_vec2f point_to_position(int x, int y);

	// and methods to generate terrain / vbo objects
	void generate_terrain();

	// helper functions for Perlin noise implementation
	static float noise(float x, float y, float z);
	static float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
	static float lerp(float t, float a, float b) { return a + t * (b - a); }
	static float grad(int hash, float x, float y, float z);

	// permutation vector getters for Perlin noise
	static const int get_p(int i);
	static const int get_permutation (int i);

	void setup_vbos();
};