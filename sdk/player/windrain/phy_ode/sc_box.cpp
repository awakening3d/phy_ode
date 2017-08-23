//sc_box.cpp
#include "ode_script.h"
#include "sc_body.h"


//ode box
typedef struct : public SCUD_HEAD {
	dBodyID pbox;
} sc_box;

#define CHECK_SCBOX(i) \
	sc_box* pscbox = (sc_box *)lua_touserdata(L,i); \
	luaL_argcheck(L, (NULL!=pscbox) && (UD_BOX==pscbox->dwClass), i, " 'box' expected"); \

#define BOX_METATABLE "ode_box_metatable"

//new box
static int new_box(lua_State *L)
{
	dBodyID pbox = (dBodyID)lua_touserdata(L, 1); //²ÎÊý1ÊÇbodyid (lightuserdata)
	luaL_argcheck(L, NULL != pbox, 1, " 'box' expected");

	size_t nbytes = sizeof(sc_box);
	sc_box *pscbox = (sc_box *)lua_newuserdata(L, nbytes);
	ZeroMemory(pscbox, sizeof(sc_box));
	pscbox->dwClass = UD_BOX;
	pscbox->pbox = pbox;

	luaL_getmetatable(L, BOX_METATABLE);
	lua_setmetatable(L, -2);
	return 1;  /* new userdatum is already on the stack */
}

static const struct luaL_reg boxlib_m[] = {
	UDHEAD_METHODS
	BODY_METHODS
	{ NULL, NULL }
};

int openlib_ode_box(lua_State *L)
{
	luaL_newmetatable(L, BOX_METATABLE);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_register(L, NULL, boxlib_m);
	lua_register(L, "_new_ode_box_ud", new_box);
	return 1;
}

