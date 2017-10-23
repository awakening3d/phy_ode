//sc_panel.cpp
#include "ode_script.h"
#include "sc_geom.h"

//ode panel
typedef struct : public SCUD_HEAD {
	dGeomID gmpanel;
} sc_panel;

#define CHECK_SCPANEL \
	sc_panel* pscpanel = (sc_panel *)lua_touserdata(L,1); \
	luaL_argcheck(L, (NULL!=pscpanel) && (UD_PANEL==pscpanel->dwClass), 1, " 'panel' expected"); \

#define PANEL_METATABLE "ode_panel_metatable"

//new panel
static int new_panel(lua_State *L)
{
	dGeomID gmpanel = (dGeomID)lua_touserdata(L, 1); //²ÎÊý1ÊÇdGeomID (lightuserdata)
	luaL_argcheck(L, NULL != gmpanel, 1, " 'panel' expected");

	size_t nbytes = sizeof(sc_panel);
	sc_panel *pscpanel = (sc_panel *)lua_newuserdata(L, nbytes);
	ZeroMemory(pscpanel, sizeof(sc_panel));
	pscpanel->dwClass = UD_PANEL;
	pscpanel->gmpanel = gmpanel;

	luaL_getmetatable(L, PANEL_METATABLE);
	lua_setmetatable(L, -2);
	return 1;  /* new userdatum is already on the stack */
}



static const struct luaL_reg panellib_m[] = {
	UDHEAD_METHODS
	GEOM_METHODS
	{ NULL, NULL }
};

int openlib_ode_panel(lua_State *L)
{
	luaL_newmetatable(L, PANEL_METATABLE);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_register(L, NULL, panellib_m);
	lua_register(L, "_new_ode_panel_ud", new_panel);
	lua_settop(L, 0);
	return 1;
}

