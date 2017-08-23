// phy_ode.cpp : 定义 DLL 应用程序的导出函数。
//
#include "..\..\..\LUA\include\lua.hpp"
#include "..\..\winds3d_sdk\com\app.h"
#include "..\..\winds3d_sdk\i\phy.h"


#include "ode_script.h"



extern "C" DLL_EXPORT DWORD VersionOfSDK() { return WINDS3D_SDK_VERSION; }


dmCList<dSpaceID> spacelist;



class CPhy : public IW3D_PHY
{
	SINGLETON_COMPONENT(CPhy)
public:
	virtual bool OneTimeInit(IApp* pApp);		// will be called when load component
	virtual bool FinalCleanup();	// will be called when unload component
	virtual int StepSimulation(float fTimeD);
};

IMPLEMENT_SINGLETON_COMPONENT(CPhy, phy_ode)
BEGIN_PACKAGE_LIST
COMPONENT(phy_ode)
END_PACKAGE_LIST


IApp* g_pApp = NULL;
dWorldID g_ode_world;
static dJointGroupID contactgroup;

dThreadingImplementationID threading;
dThreadingThreadPoolID pool;

#define GRAVITY         REAL(0.5)
#define MAX_CONTACTS 8          // maximum number of contact points per body


bool CPhy::OneTimeInit(IApp* pApp)		// will be called when load component
{
	bool bret = IW3D_PHY::OneTimeInit(pApp);
	g_pApp = pApp;


	// create world
	dInitODE2(0);
	g_ode_world = dWorldCreate();
	contactgroup = dJointGroupCreate(0);
	dWorldSetGravity(g_ode_world, 0, -GRAVITY, 0);
	dWorldSetCFM(g_ode_world, 1e-5);
	dWorldSetAutoDisableFlag(g_ode_world, 1);

	dWorldSetAutoDisableAverageSamplesCount(g_ode_world, 10);
	dWorldSetLinearDamping(g_ode_world, 0.00001);
	dWorldSetAngularDamping(g_ode_world, 0.005);
	dWorldSetMaxAngularSpeed(g_ode_world, 200);

	dWorldSetContactMaxCorrectingVel(g_ode_world, 0.1);
	dWorldSetContactSurfaceLayer(g_ode_world, 0.001);

	dThreadingImplementationID threading = dThreadingAllocateMultiThreadedImplementation();
	dThreadingThreadPoolID pool = dThreadingAllocateThreadPool(4, 0, dAllocateFlagBasicData, NULL);
	dThreadingThreadPoolServeMultiThreadedImplementation(pool, threading);
	// dWorldSetStepIslandsProcessingMaxThreadCount(g_ode_world, 1);
	dWorldSetStepThreadingImplementation(g_ode_world, dThreadingImplementationGetFunctions(threading), threading);

	
	openlib_ode_space(pApp->GetLuaVM());
	openlib_ode_sphere(pApp->GetLuaVM());
	openlib_ode_box(pApp->GetLuaVM());
	openlib_ode_cylinder(pApp->GetLuaVM());
	lua_register(pApp->GetLuaVM(), "ode_CreateSpace", l_create_space);

	return bret;
}

bool CPhy::FinalCleanup()	// will be called when unload component
{

	dThreadingImplementationShutdownProcessing(threading);
	dThreadingFreeThreadPool(pool);
	dWorldSetStepThreadingImplementation(g_ode_world, NULL, NULL);
	dThreadingFreeImplementation(threading);

	dJointGroupDestroy(contactgroup);
	dmPOSITION pos = spacelist.GetHeadPosition();
	while (pos)   dSpaceDestroy(spacelist.GetNext(pos));
	dWorldDestroy(g_ode_world);
	dCloseODE();


	return IW3D_PHY::FinalCleanup();
}



static void nearCallback(void *data, dGeomID o1, dGeomID o2)
{
	int i;
	// if (o1->body && o2->body) return;

	// exit without doing anything if the two bodies are connected by a joint
	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);
	if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact)) return;

	dContact contact[MAX_CONTACTS];   // up to MAX_CONTACTS contacts per box-box
	for (i = 0; i<MAX_CONTACTS; i++) {
		contact[i].surface.mode = dContactBounce | dContactSoftCFM;
		contact[i].surface.mu = dInfinity;
		contact[i].surface.mu2 = 0;
		contact[i].surface.bounce = 0.1;
		contact[i].surface.bounce_vel = 0.1;
		contact[i].surface.soft_cfm = 0.01;
	}
	if (int numc = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom,
		sizeof(dContact))) {
		dMatrix3 RI;
		dRSetIdentity(RI);
		const dReal ss[3] = { 0.02,0.02,0.02 };
		for (i = 0; i<numc; i++) {
			dJointID c = dJointCreateContact(g_ode_world, contactgroup, contact + i);
			dJointAttach(c, b1, b2);
			//if (show_contacts) dsDrawBox(contact[i].geom.pos, RI, ss);
		}
	}
}

int CPhy::StepSimulation( float fTimeD )
{
	dmPOSITION pos = spacelist.GetHeadPosition();
	while (pos) dSpaceCollide( spacelist.GetNext(pos), 0, &nearCallback);
	dWorldQuickStep(g_ode_world, fTimeD < 0.03f ? fTimeD : 0.03f);
	//dWorldQuickStep(g_ode_world, 0.02f);
	// remove all contact joints
	dJointGroupEmpty(contactgroup);

	return 0;
}