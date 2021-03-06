//sc_body.cpp

#include "ode_script.h"

#define CHECK_PBODY	\
	SCUD_HEAD* pud=(SCUD_HEAD*)lua_touserdata(L,1);	\
	luaL_argcheck(L, (NULL!=pud)&&(UD_NONE!=pud->dwClass), 1, " 'body' expected");	\
	dBodyID pBody = *(dBodyID*)(&pud[1]);	\

#define CHECK_XYZ(scale) \
	float x = luaL_checknumber(L, 2) * scale; \
	float y = luaL_checknumber(L, 3) * scale; \
	float z = luaL_checknumber(L, 4) * scale; \


#define PUSH_POS(scale) \
	lua_pushnumber(L, p[0] * scale); \
	lua_pushnumber(L, p[1] * scale); \
	lua_pushnumber(L, p[2] * scale); \


//get enable
int l_get_enable(lua_State *L)
{
	CHECK_PBODY //参数1是Body (userdata)
	lua_pushboolean(L, dBodyIsEnabled(pBody));
	return 1;
}

//set enable
int l_set_enable(lua_State *L)
{
	CHECK_PBODY //参数1是Body (userdata)

	bool bEnable = true;
	if (!lua_isnoneornil(L, 2))
		bEnable = lua_toboolean(L, 2); // enable or disable

	if (bEnable)
		dBodyEnable(pBody);
	else
		dBodyDisable(pBody);
	return 0;
}

//get id
int l_get_id(lua_State* L)
{
	CHECK_PBODY //参数1是Body (userdata)
	lua_pushlightuserdata(L, pBody);
	return 1;
}


//get position
int l_get_position(lua_State* L)
{
	CHECK_PBODY //参数1是Body (userdata)
	const dReal* p = dBodyGetPosition(pBody);
	PUSH_POS(UNIT_SCALE)
	return 3;
}

//set position
int l_set_position(lua_State* L)
{
	CHECK_PBODY //参数1是Body (userdata)
	CHECK_XYZ(UNIT_SCALE_INV)
	dBodySetPosition(pBody, x, y, z);
	dBodyEnable(pBody);
	return 0;
}

//get quaternion
int l_get_quaternion(lua_State* L)
{
	CHECK_PBODY //参数1是Body (userdata)
	const dReal* q = dBodyGetQuaternion(pBody);
	lua_pushnumber(L, q[1]);
	lua_pushnumber(L, q[2]);
	lua_pushnumber(L, q[3]);
	lua_pushnumber(L, q[0]);
	return 4;
}

//set quaternion
int l_set_quaternion(lua_State* L)
{
	CHECK_PBODY //参数1是Body (userdata)
	dQuaternion q;
	q[1] = luaL_checknumber(L, 2);
	q[2] = luaL_checknumber(L, 3);
	q[3] = luaL_checknumber(L, 4);
	q[0] = luaL_checknumber(L, 5);
	dBodySetQuaternion(pBody, q);
	dBodyEnable(pBody);
	return 0;
}

//get linear velocity
int l_get_linear_vel(lua_State* L)
{
	CHECK_PBODY //参数1是Body (userdata)
	const dReal* p = dBodyGetLinearVel(pBody);
	PUSH_POS(UNIT_SCALE)
	return 3;
}

//set linear velocity   
int l_set_linear_vel(lua_State* L)
{
	CHECK_PBODY //参数1是Body (userdata)
	CHECK_XYZ(UNIT_SCALE_INV)
	dBodySetLinearVel(pBody, x, y, z);
	dBodyEnable(pBody);
	return 0;
}

//get angular velocity
int l_get_angular_vel(lua_State* L)
{
	CHECK_PBODY //参数1是Body (userdata)
	const dReal* p = dBodyGetAngularVel(pBody);
	PUSH_POS(1)
	return 3;
}

//set angular velocity   
int l_set_angular_vel(lua_State* L)
{
	CHECK_PBODY //参数1是Body (userdata)
	CHECK_XYZ(1)
	dBodySetAngularVel(pBody, x, y, z);
	dBodyEnable(pBody);
	return 0;
}


//get matrix
int l_get_matrix(lua_State* L)
{
	CHECK_PBODY //参数1是Body (userdata)
		//const dReal* q = dBodyGetQuaternion(pBody);
	const dReal* p = dBodyGetPosition(pBody);
	const dReal* r = dBodyGetRotation(pBody);
	//const dReal* r = dGeomGetRotation(pBody->geom);
	static float m[4][4];
	//memcpy(m, r, sizeof(float) * 12);
	m[0][0] = r[0];		m[1][0] = r[1];		m[2][0] = r[2];		m[3][0] = r[3];
	m[0][1] = r[4+0]; m[1][1] = r[4+1];	m[2][1] = r[4+2];	m[3][1] = r[4+3];
	m[0][2] = r[4*2+0]; m[1][2] = r[4*2+1]; m[2][2] = r[4*2+2]; m[3][2] = r[4*2+3];

	m[3][0] = p[0] * UNIT_SCALE; m[3][1] = p[1] * UNIT_SCALE; m[3][2] = p[2] * UNIT_SCALE; m[3][3] = 1.0f;
	lua_pushlightuserdata(L, m);
	return 1;
}