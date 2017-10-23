#include "ode_script.h"


#define CHECK_SCHEAD	\
	SCUD_HEAD* pud=(SCUD_HEAD*)lua_touserdata(L,1);	\
	luaL_argcheck(L, (NULL!=pud)&&(UD_NONE!=pud->dwClass), 1, " 'schead' expected");


//get classid
int get_classid(lua_State *L)
{
	CHECK_SCHEAD //参数1是schead
	lua_pushlightuserdata(L, (void*)pud->dwClass);
	return 1;
}


extern dmCList<dSpaceID> spacelist;
//create space
int l_create_space(lua_State *L)
{
	dSpaceID pspace = (dSpaceID)lua_touserdata(L, 1); //参数1是Space指针 (lightuserdata)
	dSpaceID space = dHashSpaceCreate(pspace);
	if (!space) return 0;
	spacelist.AddTail(space); // add space to list
	lua_pushlightuserdata(L, space);
	return 1;
}

// get worldid
int l_get_worldid(lua_State *L)
{
	lua_pushlightuserdata(L, g_ode_world );
	return 1;
}