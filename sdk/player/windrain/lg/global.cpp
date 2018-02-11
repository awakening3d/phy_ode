#include "global.h"
#include "..\..\winds3d_sdk\com\app.h"


extern "C" DLL_EXPORT DWORD VersionOfSDK() { return WINDS3D_SDK_VERSION; }


dmCList<GeneralModule> gGeneralModuleList;


IW3D_UI* gpui=NULL;
static char g_szUIModule[MAX_PATH] = "dxui.dll";

IW3D_VR* gpvr=NULL;
static char g_szVRModule[MAX_PATH] = "winds3dvr.dll";

IW3D_PHY* gphy=NULL;
static char g_szPhyModule[MAX_PATH] = "phy_ode.dll";


static const int PACKAGE_NUM = 3;


static IApp* gpApp = NULL;

bool AppInit( IApp* pApp )
{
	gpApp = pApp;
	//for ( int i=0; i<PACKAGE_NUM; i++) load_package(i);

	extern void open_scripts( lua_State* LuaVM );
	open_scripts( gpApp->GetLuaVM() );

 	return true;
}

void AppCleanup()
{
	for ( int i=0; i<PACKAGE_NUM; i++) unload_package(i);

	dmPOSITION pos = gGeneralModuleList.GetHeadPosition();
	while (pos) {
		const GeneralModule& gm = gGeneralModuleList.GetNext(pos);
		gpApp->ReleaseComponent( gm.pComponent );
		gpApp->UnloadComponentPackage( gm.szModuleFileName );
	}
	gGeneralModuleList.RemoveAll();
}


bool load_general_package( const char* szModuleFileName, const char* szComponentName )
{
	if ( !gpApp->LoadComponentPackage( szModuleFileName ) ) return false;
	GeneralModule gm;
	ZeroMemory(&gm, sizeof(gm));

	gm.pComponent = (IW3D_GENERAL*)gpApp->CreateComponent( szComponentName );
	if (!gm.pComponent) {
		gpApp->OutputToConsole("Create Component Failed! : %s", szComponentName);
		gpApp->UnloadComponentPackage( szModuleFileName );
		return false;
	}

	strcpy( gm.szModuleFileName, szModuleFileName );
	strcpy( gm.szComponentName, szComponentName );

	gGeneralModuleList.AddTail( gm );

	return true;

}

void unload_general_package( const char* szModuleFileName )
{

	dmPOSITION pos = gGeneralModuleList.GetHeadPosition();
	while (pos) {
		dmPOSITION savepos=pos;
		const GeneralModule& gm = gGeneralModuleList.GetNext(pos);
		
		if ( 0 != strcmp(szModuleFileName, gm.szModuleFileName) ) continue;
		gpApp->ReleaseComponent( gm.pComponent );
		gpApp->UnloadComponentPackage( gm.szModuleFileName );
		gGeneralModuleList.RemoveAt(savepos);
	}

}


void load_package(int p)
{
	switch (p) {
	case 0:
		if (gpApp->LoadComponentPackage( g_szUIModule ))
			gpui = (IW3D_UI*)gpApp->CreateComponent( "dxui" );
		break;
	case 1:
		if (gpApp->LoadComponentPackage( g_szVRModule ))
			gpvr = (IW3D_VR*)gpApp->CreateComponent( "vr" );
		break;
	case 2:
		if (gpApp->LoadComponentPackage( g_szPhyModule ))
			gphy = (IW3D_PHY*)gpApp->CreateComponent( "phy_ode" );
		break;
	}
}

void unload_package(int p)
{
	switch (p) {
	case 0:
		gpApp->ReleaseComponent( gpui );
		gpui = NULL;
		gpApp->UnloadComponentPackage( g_szUIModule );
		break;
	case 1:
		gpApp->ReleaseComponent( gpvr );
		gpvr = NULL;
		gpApp->UnloadComponentPackage( g_szVRModule );
		break;
	case 2:
		gpApp->ReleaseComponent( gphy );
		gphy = NULL;
		gpApp->UnloadComponentPackage( g_szPhyModule );
		break;
	}
}