
#include "entity/ga_component.h"

class ga_terrain_component : public ga_component
{
public:
	ga_terrain_component(class ga_entity* ent, const char* param_file);
	virtual ~ga_terrain_component();

	virtual void update(struct ga_frame_params* params) override;

private:
	class ga_directional_light_material* _material;
	uint32_t _vao;
	uint32_t _vbos[4];
	uint32_t _index_count;

	// Terrain representation
	int _size;
	int _height;
	float* _points;

	// some helper getters/setters
	float get_point(int x, int y);
	void set_point(int x, int y, float height);

	// and methods to generate terrain / vbo objects
	void generate_terrain();
	void setup_vbos();
};