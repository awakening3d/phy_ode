#ifndef ODE_SCRIPT_H
#define ODE_SCRIPT_H

#define dSINGLE

#include <windows.h>
#include "..\..\..\lua\include\lua.hpp"
#include "ode\ode.h"
#include "dmclist.h"

#define UD_NONE			0x00000000L //none, invalid userdata

#define UD_SPACE		0x10000002L // space
#define UD_SPHERE		0x10000008L	// sphere
#define UD_BOX			0x10000009L // box
#define UD_CYLINDER		0x1000000aL // cylinder


typedef struct {
	DWORD dwClass; //标明userdata的种类
} SCUD_HEAD; //所有userdata的头


//get classid
int get_classid(lua_State *L);
//create space
int l_create_space(lua_State *L);

#define UDHEAD_METHODS	\
	{"get_classid",		get_classid},	\


int openlib_ode_space(lua_State *L);
int openlib_ode_sphere(lua_State *L);
int openlib_ode_box(lua_State *L);
int openlib_ode_cylinder(lua_State *L);


extern dWorldID g_ode_world;

#define ODE_DENSITY (5.0)		// density of all objects
#define UNIT_SCALE 100
#define UNIT_SCALE_INV	(1.0f/UNIT_SCALE)

#endif // ODE_SCRIPT_H