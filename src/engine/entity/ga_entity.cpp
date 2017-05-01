/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_entity.h"
#include "ga_component.h"

ga_entity::ga_entity()
{
	_transform.make_identity();
}

ga_entity::~ga_entity()
{
}

void ga_entity::add_component(ga_component* comp)
{
	_components.push_back(comp);
}

void ga_entity::dynamic_add_component(ga_component* comp)
{
	_to_add_dynamic.push_back(comp);
}

void ga_entity::dynamic_remove_component(ga_component* comp)
{
	_to_remove_dynamic.push_back(comp);
}

void ga_entity::update(ga_frame_params* params)
{
	// include components added in the last frame
	for (auto& c : _to_add_dynamic)
	{
		_components.push_back(c);
	}
	_to_add_dynamic.clear();

	// remove components as needed
	for (auto& c : _to_remove_dynamic)
	{
		auto i = _components.begin();
		while( i != _components.end())
		{
			if (*i == c)
			{
				auto comp = *i;
				i = _components.erase(i);
				delete comp;
			}
			else
			{
				i++;
			}
		}
	}
	_to_remove_dynamic.clear();

	for (auto& c : _components)
	{
		c->update(params);
	}
}

void ga_entity::late_update(ga_frame_params* params)
{
	for (auto& c : _components)
	{
		c->late_update(params);
	}
}

void ga_entity::translate(const ga_vec3f& translation)
{
	_transform.translate(translation);
}

void ga_entity::rotate(const ga_quatf& rotation)
{
	ga_mat4f rotation_m;
	rotation_m.make_rotation(rotation);
	_transform = rotation_m * _transform;
}
