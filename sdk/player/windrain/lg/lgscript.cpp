#include "global.h"
#include "..\..\..\lua\include\lua.hpp"


//lg load package
int lg_load_package(lua_State *L)
{
	if (lua_isnumber(L,1)) {
		int n = luaL_checkint(L, 1);
		load_package(n);
	} else {
		LPCSTR szModuleFileName = luaL_checkstring( L, 1 );
		LPCSTR szComponentName = luaL_checkstring( L, 2 );
		load_general_package( szModuleFileName, szComponentName );
	}
	return 0;
}

//lg unload package
int lg_unload_package(lua_State *L)
{
	if (lua_isnumber(L,1)) {
		int n = luaL_checkint(L, 1);
		unload_package(n);
	} else {
		LPCSTR szModuleFileName = luaL_checkstring( L, 1 );
		unload_general_package( szModuleFileName );
	}
	return 0;
}


void open_scripts( lua_State* LuaVM )
{
	lua_register( LuaVM, "lgLoadPackage",		lg_load_package );
	lua_register( LuaVM, "lgUnloadPackage",		lg_unload_package );
	luaL_dostring( LuaVM, "PACKAGE_UI, PACKAGE_VR, PACKAGE_ODE = 0,1,2" );
}