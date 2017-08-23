#include "global.h"
#include "..\..\winds3d_sdk\com\app.h"


extern "C" DLL_EXPORT DWORD VersionOfSDK() { return WINDS3D_SDK_VERSION; }


IW3D_UI* gpui=NULL;
static char g_szUIModule[MAX_PATH] = "dxui.dll";

IW3D_PHY* gphy=NULL;
static char g_szPhyModule[MAX_PATH] = "phy_ode.dll";

//IW3D_VR* gpvr=NULL;
static char g_szVRModule[MAX_PATH] = "winds3dvr.dll";


static IApp* gpApp = NULL;

bool AppInit( IApp* pApp )
{
	gpApp = pApp;

	gpApp->LoadComponentPackage( g_szUIModule );
	gpui = (IW3D_UI*)gpApp->CreateComponent( "dxui" );

	gpApp->LoadComponentPackage( g_szVRModule );
	//gpvr = (IW3D_VR*)gpApp->CreateComponent( "vr" );


	gpApp->LoadComponentPackage( g_szPhyModule );
	gphy = (IW3D_PHY*)gpApp->CreateComponent( "phy_ode" );
	return true;
}

void AppCleanup()
{
	gpApp->ReleaseComponent( gpui );
	gpui = NULL;
	gpApp->UnloadComponentPackage( g_szUIModule );

	//gpApp->ReleaseComponent( gpvr );
	//gpvr = NULL;
	gpApp->UnloadComponentPackage( g_szVRModule );


	gpApp->ReleaseComponent( gphy );
	gphy = NULL;
	gpApp->UnloadComponentPackage( g_szPhyModule );
}