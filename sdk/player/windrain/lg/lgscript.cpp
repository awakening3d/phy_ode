#include "global.h"
#include "..\..\..\lua\include\lua.hpp"


//lg load package
int lg_load_package(lua_State *L)
{
	int n = luaL_checkint(L, 1);
	load_package(n);
	return 0;
}

//lg unload package
int lg_unload_package(lua_State *L)
{
	int n = luaL_checkint(L, 1);
	unload_package(n);
	return 0;
}


void open_scripts( lua_State* LuaVM )
{
	lua_register( LuaVM, "lgLoadPackage",		lg_load_package );
	lua_register( LuaVM, "lgUnloadPackage",		lg_unload_package );
	luaL_dostring( LuaVM, "PACKAGE_UI, PACKAGE_VR, PACKAGE_ODE = 0,1,2" );
}