//sc_sphere.cpp
#include "ode_script.h"
#include "sc_body.h"


//ode sphere
typedef struct : public SCUD_HEAD {
	dBodyID psphere;
} sc_sphere;

#define CHECK_SCSPHERE(i) \
	sc_sphere* pscsphere = (sc_sphere *)lua_touserdata(L,i); \
	luaL_argcheck(L, (NULL!=pscsphere) && (UD_SPHERE==pscsphere->dwClass), i, " 'sphere' expected"); \

#define SPHERE_METATABLE "ode_sphere_metatable"

//new sphere
static int new_sphere(lua_State *L)
{
	dBodyID psphere = (dBodyID)lua_touserdata(L, 1); //²ÎÊý1ÊÇbodyid (lightuserdata)
	luaL_argcheck(L, NULL != psphere, 1, " 'sphere' expected");

	size_t nbytes = sizeof(sc_sphere);
	sc_sphere *pscsphere = (sc_sphere *)lua_newuserdata(L, nbytes);
	ZeroMemory(pscsphere, sizeof(sc_sphere));
	pscsphere->dwClass = UD_SPHERE;
	pscsphere->psphere = psphere;

	luaL_getmetatable(L, SPHERE_METATABLE);
	lua_setmetatable(L, -2);
	return 1;  /* new userdatum is already on the stack */
}

static const struct luaL_reg spherelib_m[] = {
	UDHEAD_METHODS
	BODY_METHODS
	{ NULL, NULL }
};

int openlib_ode_sphere(lua_State *L)
{
	luaL_newmetatable(L, SPHERE_METATABLE);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_register(L, NULL, spherelib_m);
	lua_register(L, "_new_ode_sphere_ud", new_sphere);
	lua_settop(L, 0);
	return 1;
}

