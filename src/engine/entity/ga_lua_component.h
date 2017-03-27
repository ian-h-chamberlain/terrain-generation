#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_component.h"

/*
** A component whose logic is implemented in LUA.
*/
class ga_lua_component : public ga_component
{
	class lua_State* _lua;
	class ga_entity* _entity;

	// function calls available to lua
	static int frame_params_get_input_left(class lua_State* state);
	static int frame_params_get_input_right(class lua_State* state);
	static int component_get_entity(class lua_State* state);
	static int entity_translate(class lua_State* state);

public:
	ga_lua_component(class ga_entity* ent, const char* path);
	virtual ~ga_lua_component();

	virtual void update(struct ga_frame_params* params) override;
};
