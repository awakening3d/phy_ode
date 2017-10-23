//sc_world.cpp

#include "ode_script.h"



//ode world
typedef struct : public SCUD_HEAD {
	dWorldID pworld;
} sc_world;

#define CHECK_SCWORLD(i) \
	sc_world* pscworld = (sc_world *)lua_touserdata(L,i); \
	luaL_argcheck(L, (NULL!=pscworld) && (UD_WORLD==pscworld->dwClass), i, " 'world' expected"); \

#define WORLD_METATABLE "ode_world_metatable"

//new world
static int new_world(lua_State *L)
{
	dWorldID pworld = (dWorldID)lua_touserdata(L, 1); //参数1是World指针 (lightuserdata)
	luaL_argcheck(L, NULL != pworld, 1, " 'world' expected");

	size_t nbytes = sizeof(sc_world);
	sc_world *pscworld = (sc_world *)lua_newuserdata(L, nbytes);
	ZeroMemory(pscworld, sizeof(sc_world));
	pscworld->dwClass = UD_WORLD;
	pscworld->pworld = pworld;

	luaL_getmetatable(L, WORLD_METATABLE);
	lua_setmetatable(L, -2);
	return 1;  /* new userdatum is already on the stack */
}




// get gravity
static int l_get_gravity(lua_State* L)
{
	CHECK_SCWORLD(1) //参数1是sc_world (userdata)
	dVector3 p;
	dWorldGetGravity( pscworld->pworld, p );
	lua_pushnumber(L, p[0] * UNIT_SCALE);
	lua_pushnumber(L, p[1] * UNIT_SCALE);
	lua_pushnumber(L, p[2] * UNIT_SCALE);
	return 3;

}

// set gravity
static int l_set_gravity(lua_State* L)
{
	CHECK_SCWORLD(1) //参数1是sc_world (userdata)
	float x = luaL_checknumber(L, 2) * UNIT_SCALE_INV;
	float y = luaL_checknumber(L, 3) * UNIT_SCALE_INV;
	float z = luaL_checknumber(L, 4) * UNIT_SCALE_INV;
	dWorldSetGravity( pscworld->pworld, x, y, z );
	return 0;
}




static const struct luaL_reg worldlib_m[] = {
	UDHEAD_METHODS
{ "get_gravity",		l_get_gravity },
{ "set_gravity",		l_set_gravity },
{ NULL, NULL }
};

int openlib_ode_world(lua_State *L)
{
	luaL_newmetatable(L, WORLD_METATABLE);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_register(L, NULL, worldlib_m);
	lua_register(L, "_new_ode_world_ud", new_world);
	lua_settop(L, 0);
	return 1;
}

