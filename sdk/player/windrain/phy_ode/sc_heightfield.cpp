//sc_heightfield.cpp
#include "ode_script.h"
#include "sc_geom.h"
#include "..\ode\src\heightfield.h"
#include <map>
extern std::map<dHeightfieldDataID, float *> heightdata_map;


//ode heightfield
typedef struct : public SCUD_HEAD {
	dGeomID gmheight;
	dHeightfieldDataID hfdataID;
} sc_heightfield;

#define CHECK_SCHEIGHTFIELD \
	sc_heightfield* pscheightfield = (sc_heightfield *)lua_touserdata(L,1); \
	luaL_argcheck(L, (NULL!=pscheightfield) && (UD_HEIGHTFIELD==pscheightfield->dwClass), 1, " 'heightfield' expected"); \

#define HEIGHTFIELD_METATABLE "ode_heightfield_metatable"

//new heightfield
static int new_heightfield(lua_State *L)
{
	dGeomID gmheight = (dGeomID)lua_touserdata(L, 1); //参数1是dGeomID (lightuserdata)
	luaL_argcheck(L, NULL != gmheight, 1, " 'heightgeom' expected");

	dHeightfieldDataID heightdata = (dHeightfieldDataID)lua_touserdata(L, 2); //参数2是heightdata (lightuserdata)
	luaL_argcheck(L, NULL != heightdata, 2, " 'heightdata' expected");

	size_t nbytes = sizeof(sc_heightfield);
	sc_heightfield *pscheightfield = (sc_heightfield *)lua_newuserdata(L, nbytes);
	ZeroMemory(pscheightfield, sizeof(sc_heightfield));
	pscheightfield->dwClass = UD_HEIGHTFIELD;
	pscheightfield->hfdataID = heightdata;
	pscheightfield->gmheight = gmheight;

	luaL_getmetatable(L, HEIGHTFIELD_METATABLE);
	lua_setmetatable(L, -2);
	return 1;  /* new userdatum is already on the stack */
}


//get dimension
static int l_get_dimension(lua_State *L)
{
	CHECK_SCHEIGHTFIELD // 参数1是heightfield
	lua_pushinteger(L, pscheightfield->hfdataID->m_nWidthSamples);
	lua_pushinteger(L, pscheightfield->hfdataID->m_nDepthSamples);
	return 2;
}

//set height
static int l_set_height(lua_State *L)
{
	CHECK_SCHEIGHTFIELD // 参数1是heightfield
	unsigned int row = luaL_checkinteger( L, 2 ); //  参数2是row (int)
	unsigned int col = luaL_checkinteger( L, 3 ); //  参数3是col (int)
	luaL_argcheck(L, row < pscheightfield->hfdataID->m_nWidthSamples, 2, " must row < width samples ");
	luaL_argcheck(L, col < pscheightfield->hfdataID->m_nDepthSamples, 3, " must col < depth samples ");
	float fHeight = luaL_checknumber( L, 4 )*UNIT_SCALE_INV; // 参数4是 height (number)
	if ( ! heightdata_map.count(pscheightfield->hfdataID) || ! heightdata_map[pscheightfield->hfdataID] )
		luaL_error(L, "cann't find height data buffer!");

	heightdata_map[pscheightfield->hfdataID][ row + (col*pscheightfield->hfdataID->m_nWidthSamples) ] = fHeight;
	return 0;
}

//set bounds
static int l_set_bounds(lua_State *L)
{
	CHECK_SCHEIGHTFIELD // 参数1是heightfield
	float minHeight = luaL_checknumber( L, 2 )*UNIT_SCALE_INV; // 参数2是 min height (number)
	float maxHeight = luaL_checknumber( L, 3 )*UNIT_SCALE_INV; // 参数3是 max height (number)
	dGeomHeightfieldDataSetBounds( pscheightfield->hfdataID, minHeight, maxHeight );
	return 0;
}




static const struct luaL_reg heightfieldlib_m[] = {
	UDHEAD_METHODS
	GEOM_METHODS
	{ "get_dimension",		l_get_dimension },
	{ "set_height",			l_set_height},
	{ "set_bounds",			l_set_bounds},
	{ NULL, NULL }
};

int openlib_ode_heightfield(lua_State *L)
{
	luaL_newmetatable(L, HEIGHTFIELD_METATABLE);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_register(L, NULL, heightfieldlib_m);
	lua_register(L, "_new_ode_heightfield_ud", new_heightfield);
	lua_settop(L, 0);
	return 1;
}

