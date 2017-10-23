//sc_capsule.cpp
#include "ode_script.h"
#include "sc_body.h"


//ode capsule
typedef struct : public SCUD_HEAD {
	dBodyID pcapsule;
} sc_capsule;

#define CHECK_SCCAPSULE(i) \
	sc_capsule* psccapsule = (sc_capsule *)lua_touserdata(L,i); \
	luaL_argcheck(L, (NULL!=psccapsule) && (UD_CAPSULE==psccapsule->dwClass), i, " 'capsule' expected"); \

#define CAPSULE_METATABLE "ode_capsule_metatable"

//new capsule
static int new_capsule(lua_State *L)
{
	dBodyID pcapsule = (dBodyID)lua_touserdata(L, 1); //²ÎÊý1ÊÇbodyid (lightuserdata)
	luaL_argcheck(L, NULL != pcapsule, 1, " 'capsule' expected");

	size_t nbytes = sizeof(sc_capsule);
	sc_capsule *psccapsule = (sc_capsule *)lua_newuserdata(L, nbytes);
	ZeroMemory(psccapsule, sizeof(sc_capsule));
	psccapsule->dwClass = UD_CAPSULE;
	psccapsule->pcapsule = pcapsule;

	luaL_getmetatable(L, CAPSULE_METATABLE);
	lua_setmetatable(L, -2);
	return 1;  /* new userdatum is already on the stack */
}

static const struct luaL_reg capsulelib_m[] = {
	UDHEAD_METHODS
	BODY_METHODS
	{ NULL, NULL }
};

int openlib_ode_capsule(lua_State *L)
{
	luaL_newmetatable(L, CAPSULE_METATABLE);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_register(L, NULL, capsulelib_m);
	lua_register(L, "_new_ode_capsule_ud", new_capsule);
	lua_settop(L, 0);
	return 1;
}

