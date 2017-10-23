//sc_space.cpp

#include "ode_script.h"
#include <map>
std::map<dHeightfieldDataID, float *> heightdata_map;

//ode space
typedef struct : public SCUD_HEAD {
	dSpaceID pspace;
} sc_space;

#define CHECK_SCSPACE(i) \
	sc_space* pscspace = (sc_space *)lua_touserdata(L,i); \
	luaL_argcheck(L, (NULL!=pscspace) && (UD_SPACE==pscspace->dwClass), i, " 'space' expected"); \

#define SPACE_METATABLE "ode_space_metatable"

//new space
static int new_space(lua_State *L)
{
	dSpaceID pspace = (dSpaceID)lua_touserdata(L, 1); //参数1是Space指针 (lightuserdata)
	luaL_argcheck(L, NULL != pspace, 1, " 'space' expected");

	size_t nbytes = sizeof(sc_space);
	sc_space *pscspace = (sc_space *)lua_newuserdata(L, nbytes);
	ZeroMemory(pscspace, sizeof(sc_space));
	pscspace->dwClass = UD_SPACE;
	pscspace->pspace = pspace;

	luaL_getmetatable(L, SPACE_METATABLE);
	lua_setmetatable(L, -2);
	return 1;  /* new userdatum is already on the stack */
}

// destroy
static int l_destroy(lua_State *L)
{
	CHECK_SCSPACE(1) //参数1是sc_space (userdata)
	dSpaceID space = pscspace->pspace;
	if (!space) return 0;
	dSpaceSetCleanup(space,1);
	dSpaceDestroy(space);
	extern dmCList<dSpaceID> spacelist;
	spacelist.RemoveAt( spacelist.Find(space) ); // remove from list
	pscspace->dwClass = UD_NONE;
	return 0;
}

// destroy object
static int l_destroy_object(lua_State *L)
{
	CHECK_SCSPACE(1) //参数1是sc_space (userdata)
	SCUD_HEAD* pud=(SCUD_HEAD*)lua_touserdata(L,2); // 参数2是 userdata
	luaL_argcheck(L, (NULL!=pud)&&(UD_NONE!=pud->dwClass), 2, " 'userdata' expected");
	
	dGeomID pGeom = NULL;
	dBodyID pBody = NULL;
	switch (pud->dwClass) {
	case UD_PANEL:
		pGeom = *(dGeomID*)(&pud[1]);
		break;
	case UD_HEIGHTFIELD:
		{
		pGeom = *(dGeomID*)(&pud[1]);
		dHeightfieldDataID hfdataID = *(dHeightfieldDataID*)( sizeof(dGeomID) + (char*)&pud[1] );
		if (heightdata_map.count(hfdataID)) {
			float* pfloat = heightdata_map[hfdataID];
			if (pfloat) delete [] pfloat;
			heightdata_map.erase(hfdataID);
		}
		if (hfdataID) dGeomHeightfieldDataDestroy(hfdataID);
		}
		break;
	case UD_SPHERE:
	case UD_BOX:
	case UD_CYLINDER:
	case UD_CAPSULE:
		pBody = *(dBodyID*)(&pud[1]);
		if (pBody) pGeom = dBodyGetFirstGeom(pBody);
		break;
	}

	if (pGeom) dGeomDestroy(pGeom);
	if (pBody) dBodyDestroy(pBody);
	pud->dwClass = UD_NONE;
	return 0;
}

// create panel
static int l_create_panel(lua_State* L)
{
	CHECK_SCSPACE(1) //参数1是sc_space (userdata)

	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	float z = luaL_checknumber(L, 4);
	float d = luaL_checknumber(L, 5) * UNIT_SCALE_INV;

	dGeomID geom = dCreatePlane(pscspace->pspace, x, y, z, d); // create a geom

	lua_pushlightuserdata(L, geom);
	return 1;
}


#define MAX_HFWIDTH  (16*1024)	// max width of height field

// create heightfield
static int l_create_heightfield(lua_State* L)
{
	CHECK_SCSPACE(1) //参数1是sc_space (userdata)
	
	unsigned int row = luaL_checkinteger( L, 2 ); //  参数2是row (int)
	unsigned int col = luaL_checkinteger( L, 3 ); //  参数3是col (int)
	luaL_argcheck(L, 2 < row && row <= MAX_HFWIDTH, 2, " must 2 < row <= 16*1024 ");
	luaL_argcheck(L, 2 < col && col <= MAX_HFWIDTH, 3, " must 2 < col <= 16*1024 ");
	float fInterval = luaL_checknumber( L, 4 )*UNIT_SCALE_INV; // 参数4是 interval (number)
	luaL_argcheck(L, fInterval>0.0f, 4, " must interval > 0 ");
	float fThickness = 1;
	if (!lua_isnoneornil(L,5))
		fThickness = luaL_checknumber( L, 5 )*UNIT_SCALE_INV; // 参数5是 Thickness (number)
	bool bWrap = false;
	if (!lua_isnoneornil(L,6))
		bWrap = lua_toboolean( L, 6 ); // 参数6是 wrap (bool)

	float *pHeightData = new float[row*col];
	if (!pHeightData) {
		luaL_error(L, "create height field: not enough memory!");
		return 0;
	}

	dHeightfieldDataID heightid = dGeomHeightfieldDataCreate();
	heightdata_map[heightid] = pHeightData; // record to map
	dGeomHeightfieldDataBuildSingle ( heightid, pHeightData, false, (row-1)*fInterval, (col-1)*fInterval, row, col, 1, 0, fThickness, bWrap );
	dGeomID geom = dCreateHeightfield( pscspace->pspace, heightid, true );

	lua_pushlightuserdata( L, geom );
	lua_pushlightuserdata( L, heightid );
	return 2;
}


// add sphere
static int l_add_sphere(lua_State* L)
{
	CHECK_SCSPACE(1) //参数1是sc_space (userdata)

	float radius = luaL_checknumber(L, 2) * UNIT_SCALE_INV; // the radius of sphere

	dBodyID body = dBodyCreate(g_ode_world); // create a body
	if (!body) return 0;
	dGeomID geom = dCreateSphere(pscspace->pspace, radius); // create a geom
	dMass m;
	dMassSetSphere(&m, ODE_DENSITY, radius);
	dBodySetMass(body, &m);
	dGeomSetBody(geom, body);

	lua_pushlightuserdata(L, body);
	return 1;
}

// add box
static int l_add_box(lua_State* L)
{
	CHECK_SCSPACE(1) //参数1是sc_space (userdata)

	float lx = luaL_checknumber(L, 2) * UNIT_SCALE_INV; // the length of the box along the X axis
	float ly = luaL_checknumber(L, 3) * UNIT_SCALE_INV;
	float lz = luaL_checknumber(L, 4) * UNIT_SCALE_INV;

	dBodyID body = dBodyCreate(g_ode_world); // create a body
	if (!body) return 0;
	dGeomID geom = dCreateBox(pscspace->pspace, lx, ly, lz); // create a geom
	dMass m;
	dMassSetBox(&m, ODE_DENSITY, lx,ly,lz);
	dBodySetMass(body, &m);
	dGeomSetBody(geom, body);
	
	lua_pushlightuserdata(L, body);
	return 1;
}

// add cylinder
static int l_add_cylinder(lua_State* L)
{
	CHECK_SCSPACE(1) //参数1是sc_space (userdata)

	float radius = luaL_checknumber(L, 2) * UNIT_SCALE_INV;
	float length = luaL_checknumber(L, 3) * UNIT_SCALE_INV;

	dBodyID body = dBodyCreate(g_ode_world); // create a body
	if (!body) return 0;
	dGeomID geom = dCreateCylinder(pscspace->pspace, radius, length); // create a geom
	dMass m;
	dMassSetCylinder(&m, ODE_DENSITY, 3, radius, length);
	dBodySetMass(body, &m);
	dGeomSetBody(geom, body);

	lua_pushlightuserdata(L, body);
	return 1;
}

// add capsule
static int l_add_capsule(lua_State* L)
{
	CHECK_SCSPACE(1) //参数1是sc_space (userdata)

	float radius = luaL_checknumber(L, 2) * UNIT_SCALE_INV;
	float length = luaL_checknumber(L, 3) * UNIT_SCALE_INV;

	dBodyID body = dBodyCreate(g_ode_world); // create a body
	if (!body) return 0;
	dGeomID geom = dCreateCapsule(pscspace->pspace, radius, length); // create a geom
	dMass m;
	dMassSetCapsule(&m, ODE_DENSITY, 3, radius, length);
	dBodySetMass(body, &m);
	dGeomSetBody(geom, body);

	lua_pushlightuserdata(L, body);
	return 1;
}


static const struct luaL_reg spacelib_m[] = {
	UDHEAD_METHODS
{ "create_panel",		l_create_panel },
{ "create_heightfield",	l_create_heightfield },
{ "add_sphere",			l_add_sphere },
{ "add_box",			l_add_box },
{ "add_cylinder",		l_add_cylinder },
{ "add_capsule",		l_add_capsule },

{ "destroy_object",		l_destroy_object },
{ "destroy",			l_destroy },
{ NULL, NULL }
};

int openlib_ode_space(lua_State *L)
{
	luaL_newmetatable(L, SPACE_METATABLE);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_register(L, NULL, spacelib_m);
	lua_register(L, "_new_ode_space_ud", new_space);
	lua_settop(L, 0);
	return 1;
}

