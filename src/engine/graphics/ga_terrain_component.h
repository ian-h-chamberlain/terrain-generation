
#include "entity/ga_component.h"

class ga_terrain_component : public ga_component
{
public:
	ga_terrain_component(class ga_entity* ent, const char* param_file);
	virtual ~ga_terrain_component();

	virtual void update(struct ga_frame_params* params) override;

private:
	class ga_material* _material;
	uint32_t _vao;
	uint32_t _vbos[4];
	uint32_t _index_count;
};
