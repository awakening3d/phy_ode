//sc_space.cpp

#include "ode_script.h"



//ui space
typedef struct : public SCUD_HEAD {
	dSpaceID pspace;
} sc_space;

#define CHECK_SCSPACE(i) \
	sc_space* pscspace = (sc_space *)lua_touserdata(L,i); \
	luaL_argcheck(L, (NULL!=pscspace) && (UD_SPACE==pscspace->dwClass), i, " 'space' expected"); \

#define SPACE_METATABLE "ode_space_metatable"

//new space
static int new_space(lua_State *L)
{
	dSpaceID pspace = (dSpaceID)lua_touserdata(L, 1); //参数1是Space指针 (lightuserdata)
	luaL_argcheck(L, NULL != pspace, 1, " 'space' expected");

	size_t nbytes = sizeof(sc_space);
	sc_space *pscspace = (sc_space *)lua_newuserdata(L, nbytes);
	ZeroMemory(pscspace, sizeof(sc_space));
	pscspace->dwClass = UD_SPACE;
	pscspace->pspace = pspace;

	luaL_getmetatable(L, SPACE_METATABLE);
	lua_setmetatable(L, -2);
	return 1;  /* new userdatum is already on the stack */
}

static int l_create_panel(lua_State* L)
{
	CHECK_SCSPACE(1) //参数1是sc_space (userdata)

	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	float z = luaL_checknumber(L, 4);
	float d = luaL_checknumber(L, 5) * UNIT_SCALE_INV;

	dGeomID geom = dCreatePlane(pscspace->pspace, x, y, z, d); // create a geom

	lua_pushlightuserdata(L, geom);
	return 1;
}

static int l_add_sphere(lua_State* L)
{
	CHECK_SCSPACE(1) //参数1是sc_space (userdata)

	float radius = luaL_checknumber(L, 2) * UNIT_SCALE_INV; // the radius of sphere

	dBodyID body = dBodyCreate(g_ode_world); // create a body
	if (!body) return 0;
	dGeomID geom = dCreateSphere(pscspace->pspace, radius); // create a geom
	dMass m;
	dMassSetSphere(&m, ODE_DENSITY, radius);
	dBodySetMass(body, &m);
	dGeomSetBody(geom, body);

	lua_pushlightuserdata(L, body);
	return 1;
}

static int l_add_box(lua_State* L)
{
	CHECK_SCSPACE(1) //参数1是sc_space (userdata)

	float lx = luaL_checknumber(L, 2) * UNIT_SCALE_INV; // the length of the box along the X axis
	float ly = luaL_checknumber(L, 3) * UNIT_SCALE_INV;
	float lz = luaL_checknumber(L, 4) * UNIT_SCALE_INV;

	dBodyID body = dBodyCreate(g_ode_world); // create a body
	if (!body) return 0;
	dGeomID geom = dCreateBox(pscspace->pspace, lx, ly, lz); // create a geom
	dMass m;
	dMassSetBox(&m, ODE_DENSITY, lx,ly,lz);
	dBodySetMass(body, &m);
	dGeomSetBody(geom, body);
	
	lua_pushlightuserdata(L, body);
	return 1;
}

static int l_add_cylinder(lua_State* L)
{
	CHECK_SCSPACE(1) //参数1是sc_space (userdata)

	float radius = luaL_checknumber(L, 2) * UNIT_SCALE_INV;
	float length = luaL_checknumber(L, 3) * UNIT_SCALE_INV;


	dBodyID body = dBodyCreate(g_ode_world); // create a body
	if (!body) return 0;
	dGeomID geom = dCreateCylinder(pscspace->pspace, radius, length); // create a geom
	dMass m;
	dMassSetCylinder(&m, ODE_DENSITY, 3, radius, length);
	dBodySetMass(body, &m);
	dGeomSetBody(geom, body);

	lua_pushlightuserdata(L, body);
	return 1;
}


static const struct luaL_reg spacelib_m[] = {
	UDHEAD_METHODS
{ "create_panel",		l_create_panel },
{ "add_sphere",			l_add_sphere },
{ "add_box",			l_add_box },
{ "add_cylinder",		l_add_cylinder },
{ NULL, NULL }
};

int openlib_ode_space(lua_State *L)
{
	luaL_newmetatable(L, SPACE_METATABLE);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_register(L, NULL, spacelib_m);
	lua_register(L, "_new_ode_space_ud", new_space);
	return 1;
}

