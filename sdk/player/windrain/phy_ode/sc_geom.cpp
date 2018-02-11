//sc_geom.cpp

#include "ode_script.h"
#include "sc_geom.h"

#define CHECK_PGEOM	\
	SCUD_HEAD* pud=(SCUD_HEAD*)lua_touserdata(L,1);	\
	luaL_argcheck(L, (NULL!=pud)&&(UD_NONE!=pud->dwClass), 1, " 'geom' expected");	\
	dGeomID pGeom = *(dGeomID*)(&pud[1]);	\

#define CHECK_XYZ(scale) \
	float x = luaL_checknumber(L, 2) * scale; \
	float y = luaL_checknumber(L, 3) * scale; \
	float z = luaL_checknumber(L, 4) * scale; \


#define PUSH_POS(scale) \
	lua_pushnumber(L, p[0] * scale); \
	lua_pushnumber(L, p[1] * scale); \
	lua_pushnumber(L, p[2] * scale); \





//ode geom
typedef struct : public SCUD_HEAD {
	dGeomID gmgeom;
} sc_geom;

#define CHECK_SCGEOM \
	sc_geom* pscgeom = (sc_geom *)lua_touserdata(L,1); \
	luaL_argcheck(L, (NULL!=pscgeom) && (UD_GEOM==pscgeom->dwClass), 1, " 'geom' expected"); \

#define GEOM_METATABLE "ode_geom_metatable"

//new geom
static int new_geom(lua_State *L)
{
	dGeomID gmgeom = (dGeomID)lua_touserdata(L, 1); //参数1是dGeomID (lightuserdata)
	luaL_argcheck(L, NULL != gmgeom, 1, " 'geom' expected");

	size_t nbytes = sizeof(sc_geom);
	sc_geom *pscgeom = (sc_geom *)lua_newuserdata(L, nbytes);
	ZeroMemory(pscgeom, sizeof(sc_geom));
	pscgeom->dwClass = UD_GEOM;
	pscgeom->gmgeom = gmgeom;

	luaL_getmetatable(L, GEOM_METATABLE);
	lua_setmetatable(L, -2);
	return 1;  /* new userdatum is already on the stack */
}




//get enable
int l_geo_get_enable(lua_State *L)
{
	CHECK_PGEOM //参数1是Geom (userdata)
	lua_pushboolean(L, dGeomIsEnabled(pGeom));
	return 1;
}

//set enable
int l_geo_set_enable(lua_State *L)
{
	CHECK_PGEOM //参数1是Geom (userdata)

	bool bEnable = true;
	if (!lua_isnoneornil(L, 2))
		bEnable = lua_toboolean(L, 2); // enable or disable

	if (bEnable)
		dGeomEnable(pGeom);
	else
		dGeomDisable(pGeom);
	return 0;
}

//get id
int l_geo_get_id(lua_State* L)
{
	CHECK_PGEOM //参数1是Geom (userdata)
	lua_pushlightuserdata(L, pGeom);
	return 1;
}


//get position
int l_geo_get_position(lua_State* L)
{
	CHECK_PGEOM //参数1是Geom (userdata)
	const dReal* p = dGeomGetPosition(pGeom);
	PUSH_POS(UNIT_SCALE)
	return 3;
}

//set position
int l_geo_set_position(lua_State* L)
{
	CHECK_PGEOM //参数1是Geom (userdata)
	CHECK_XYZ(UNIT_SCALE_INV)
	dGeomSetPosition(pGeom, x, y, z);
	dGeomEnable(pGeom);
	return 0;
}

//get quaternion
int l_geo_get_quaternion(lua_State* L)
{
	CHECK_PGEOM //参数1是Geom (userdata)
	dQuaternion qua;
	dGeomGetQuaternion (pGeom, qua);
	const dReal* q = qua;
	lua_pushnumber(L, q[1]);
	lua_pushnumber(L, q[2]);
	lua_pushnumber(L, q[3]);
	lua_pushnumber(L, q[0]);
	return 4;
}

//set quaternion
int l_geo_set_quaternion(lua_State* L)
{
	CHECK_PGEOM //参数1是Geom (userdata)
	dQuaternion q;
	q[1] = luaL_checknumber(L, 2);
	q[2] = luaL_checknumber(L, 3);
	q[3] = luaL_checknumber(L, 4);
	q[0] = luaL_checknumber(L, 5);
	dGeomSetQuaternion(pGeom, q);
	dGeomEnable(pGeom);
	return 0;
}


//get matrix
int l_geo_get_matrix(lua_State* L)
{
	CHECK_PGEOM //参数1是Geom (userdata)
		//const dReal* q = dGeomGetQuaternion(pGeom);
	const dReal* p = dGeomGetPosition(pGeom);
	const dReal* r = dGeomGetRotation(pGeom);
	//const dReal* r = dGeomGetRotation(pGeom->geom);
	static float m[4][4];
	//memcpy(m, r, sizeof(float) * 12);
	m[0][0] = r[0];		m[1][0] = r[1];		m[2][0] = r[2];		m[3][0] = r[3];
	m[0][1] = r[4+0]; m[1][1] = r[4+1];	m[2][1] = r[4+2];	m[3][1] = r[4+3];
	m[0][2] = r[4*2+0]; m[1][2] = r[4*2+1]; m[2][2] = r[4*2+2]; m[3][2] = r[4*2+3];

	m[3][0] = p[0] * UNIT_SCALE; m[3][1] = p[1] * UNIT_SCALE; m[3][2] = p[2] * UNIT_SCALE; m[3][3] = 1.0f;
	lua_pushlightuserdata(L, m);
	return 1;
}






static const struct luaL_reg geomlib_m[] = {
	UDHEAD_METHODS
	GEOM_METHODS
	{ NULL, NULL }
};

int openlib_ode_geom(lua_State *L)
{
	luaL_newmetatable(L, GEOM_METATABLE);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_register(L, NULL, geomlib_m);
	lua_register(L, "_new_ode_geom_ud", new_geom);
	lua_settop(L, 0);
	return 1;
}

