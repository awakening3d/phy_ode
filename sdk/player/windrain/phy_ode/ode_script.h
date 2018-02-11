#ifndef ODE_SCRIPT_H
#define ODE_SCRIPT_H

#define dSINGLE

#include <windows.h>
#include "..\..\..\lua\include\lua.hpp"
#include "ode\ode.h"
#include "..\..\common\dmclist.h"

#define UD_NONE			0x00000000L //none, invalid userdata

#define UD_WORLD		0x10000001L // world
#define UD_SPACE		0x10000002L // space

#define UD_GEOM			0x20000001L // geom
#define UD_HEIGHTFIELD	0x20000002L // heightfield

#define UD_SPHERE		0x30000001L	// sphere
#define UD_BOX			0x30000002L // box
#define UD_CYLINDER		0x30000003L // cylinder
#define UD_CAPSULE		0x30000004L // capsule




typedef struct {
	DWORD dwClass; //标明userdata的种类
} SCUD_HEAD; //所有userdata的头


//get classid
int get_classid(lua_State *L);
//create space
int l_create_space(lua_State *L);
// get worldid
int l_get_worldid(lua_State *L);

#define UDHEAD_METHODS	\
	{"get_classid",		get_classid},	\


int openlib_ode_world(lua_State *L);
int openlib_ode_space(lua_State *L);
int openlib_ode_sphere(lua_State *L);
int openlib_ode_box(lua_State *L);
int openlib_ode_cylinder(lua_State *L);
int openlib_ode_capsule(lua_State *L);
int openlib_ode_geom(lua_State *L);
int openlib_ode_heightfield(lua_State *L);


extern dWorldID g_ode_world;

#define ODE_DENSITY (5.0)		// density of all objects
#define UNIT_SCALE 100
#define UNIT_SCALE_INV	(1.0f/UNIT_SCALE)

#endif // ODE_SCRIPT_H