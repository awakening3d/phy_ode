//sc_body.h

#ifndef SC_BODY_H
#define SC_BODY_H

//get enable
int l_get_enable(lua_State *L);
//set enable
int l_set_enable(lua_State *L);
//get id
int l_get_id(lua_State* L);
//get position
int l_get_position(lua_State* L);
//set position
int l_set_position(lua_State* L);
//get quaternion
int l_get_quaternion(lua_State* L);
//set quaternion
int l_set_quaternion(lua_State* L);
//get matrix
int l_get_matrix(lua_State* L);



#define BODY_METHODS	\
	{"get_enable",		l_get_enable},	\
	{"set_enable",		l_set_enable},	\
	{"get_id",			l_get_id},	\
	{"get_position",	l_get_position},	\
	{"set_position",	l_set_position},	\
	{"get_quaternion",	l_get_quaternion},	\
	{"set_quaternion",	l_set_quaternion},	\
	{"get_matrix",		l_get_matrix},	\


#endif