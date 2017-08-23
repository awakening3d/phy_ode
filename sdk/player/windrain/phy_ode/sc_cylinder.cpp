//sc_cylinder.cpp
#include "ode_script.h"
#include "sc_body.h"


//ode cylinder
typedef struct : public SCUD_HEAD {
	dBodyID pcylinder;
} sc_cylinder;

#define CHECK_SCCYLINDER(i) \
	sc_cylinder* psccylinder = (sc_cylinder *)lua_touserdata(L,i); \
	luaL_argcheck(L, (NULL!=psccylinder) && (UD_CYLINDER==psccylinder->dwClass), i, " 'cylinder' expected"); \

#define CYLINDER_METATABLE "ode_cylinder_metatable"

//new cylinder
static int new_cylinder(lua_State *L)
{
	dBodyID pcylinder = (dBodyID)lua_touserdata(L, 1); //²ÎÊý1ÊÇbodyid (lightuserdata)
	luaL_argcheck(L, NULL != pcylinder, 1, " 'cylinder' expected");

	size_t nbytes = sizeof(sc_cylinder);
	sc_cylinder *psccylinder = (sc_cylinder *)lua_newuserdata(L, nbytes);
	ZeroMemory(psccylinder, sizeof(sc_cylinder));
	psccylinder->dwClass = UD_CYLINDER;
	psccylinder->pcylinder = pcylinder;

	luaL_getmetatable(L, CYLINDER_METATABLE);
	lua_setmetatable(L, -2);
	return 1;  /* new userdatum is already on the stack */
}

static const struct luaL_reg cylinderlib_m[] = {
	UDHEAD_METHODS
	BODY_METHODS
	{ NULL, NULL }
};

int openlib_ode_cylinder(lua_State *L)
{
	luaL_newmetatable(L, CYLINDER_METATABLE);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_register(L, NULL, cylinderlib_m);
	lua_register(L, "_new_ode_cylinder_ud", new_cylinder);
	return 1;
}

