#include "ode_script.h"


#define CHECK_SCHEAD	\
	SCUD_HEAD* pud=(SCUD_HEAD*)lua_touserdata(L,1);	\
	luaL_argcheck(L, (NULL!=pud)&&(UD_NONE!=pud->dwClass), 1, " 'schead' expected");


//get classid
int get_classid(lua_State *L)
{
	CHECK_SCHEAD //²ÎÊý1ÊÇschead
		lua_pushlightuserdata(L, (void*)pud->dwClass);
	return 1;
}


extern dmCList<dSpaceID> spacelist;
//create space
int l_create_space(lua_State *L)
{
	dSpaceID space = dHashSpaceCreate(0);
	if (!space) return 0;
	spacelist.AddTail(space); // add space to list
	lua_pushlightuserdata(L, space);
	return 1;
}