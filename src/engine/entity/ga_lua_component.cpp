/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_lua_component.h"

#include "entity/ga_entity.h"
#include "framework/ga_frame_params.h"

#include <lua.hpp>

#include <iostream>

ga_lua_component::ga_lua_component(ga_entity* ent, const char* path) : ga_component(ent)
{
	// First initialize lua
	_lua = luaL_newstate();
	luaL_openlibs(_lua);

	// register our custom functions
	lua_register(_lua, "frame_params_get_input_left", frame_params_get_input_left);
	lua_register(_lua, "frame_params_get_input_right", frame_params_get_input_right);
	lua_register(_lua, "component_get_entity", component_get_entity);
	lua_register(_lua, "entity_translate", entity_translate);

	// Figure out the script's full path
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += path;

	// and actually load it into lua
	int status = luaL_loadfile(_lua, fullpath.c_str());
	if (status)
	{
		std::cerr << "Failed to load script " << path << ": " << lua_tostring(_lua, -1);
		lua_close(_lua);
	}

	// now add the function to the global table
	status = lua_pcall(_lua, 0, 0, 0);
	if (status)
	{
		std::cerr << "Error loading global functions: " << lua_tostring(_lua, -1);
		lua_close(_lua);
	}
}

ga_lua_component::~ga_lua_component()
{
	lua_close(_lua);
}

void ga_lua_component::update(ga_frame_params* params)
{
	// call the lua script's update function
	lua_getglobal(_lua, "update");

	// pass this component as the first argument
	ga_component** comp = (ga_component**) lua_newuserdata(_lua, sizeof(ga_component*));
	*comp = this;

	// and the frame params as the second argument
	ga_frame_params** par = (ga_frame_params**) lua_newuserdata(_lua, sizeof(ga_frame_params*));
	*par = params;

	// now call the function
	int status = lua_pcall(_lua, 2, 0, 0);

	if (status)
	{
		std::cerr << "Error in function 'update': " << lua_tostring(_lua, -1);
	}
}

// get input from the "left" key ("J")
int ga_lua_component::frame_params_get_input_left(lua_State* state)
{
	int argc = lua_gettop(state);

	if (argc != 1)
	{
		std::cerr << "Error: wrong number of args to 'frame_params_get_input_left'" << std::endl;
		return 0;
	}

	// grab frame_params 
	ga_frame_params** params = (ga_frame_params**) lua_touserdata(state, 1);

	// return the value of button press "J"
	lua_pushboolean(state, (*params)->_button_mask & k_button_j);

	// and tell Lua we have one return value
	return 1;
}

// get input from the "right" key ("L")
int ga_lua_component::frame_params_get_input_right(lua_State* state)
{
	int argc = lua_gettop(state);

	if (argc != 1)
	{
		std::cerr << "Error: wrong number of args to 'frame_params_get_input_right'" << std::endl;
		return 0;
	}

	// grab frame_params
	ga_frame_params** params = (ga_frame_params**) lua_touserdata(state, 1);

	// return the value of button press "L"
	lua_pushboolean(state, (*params)->_button_mask & k_button_l);

	// and tell Lua we have one return value
	return 1;
}

// get the entity associated with the given component
int ga_lua_component::component_get_entity(lua_State* state)
{
	int argc = lua_gettop(state);

	if (argc != 1)
	{
		std::cerr << "Error: wrong number of args to 'component_get_entity'" << std::endl;
		return 0;
	}

	// grab the component
	ga_component** component = (ga_component**) lua_touserdata(state, 1);

	// give lua a pointer to the entity
	ga_entity** ent = (ga_entity**) lua_newuserdata(state, sizeof(ga_entity*));
	*ent = (*component)->get_entity();

	return 1;
}

// translate the given entity
int ga_lua_component::entity_translate(lua_State* state)
{
	int argc = lua_gettop(state);

	if (argc != 4)
	{
		std::cerr << "Error: wrong number of args to 'entity_translate'" << std::endl;
		return 0;
	}

	// first element should be the entity pointer
	ga_entity** entity = (ga_entity**) lua_touserdata(state, 1);

	// x, y, and z are the next 3 args
	float x = luaL_checknumber(state, 2);
	float y = luaL_checknumber(state, 3);
	float z = luaL_checknumber(state, 4);

	// actually do the translate
	ga_vec3f translate_vector = { x, y, z };
	(*entity)->translate(translate_vector);

	// no return values
	return 0;
}