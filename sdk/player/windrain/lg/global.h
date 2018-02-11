#include "..\..\..\lua\include\lua.hpp"
#include "..\..\winds3d_sdk\i\ui.h"
#include "..\..\winds3d_sdk\i\vr.h"
#include "..\..\winds3d_sdk\i\phy.h"
#include "..\..\winds3d_sdk\i\general.h"
#include "..\..\common\dmclist.h"

extern IW3D_UI* gpui;
extern IW3D_VR* gpvr;
extern IW3D_PHY* gphy;

typedef struct {
	IW3D_GENERAL* pComponent;
	char szModuleFileName[MAX_PATH];
	char szComponentName[64];
} GeneralModule;

extern dmCList<GeneralModule> gGeneralModuleList;



void load_package(int p);
void unload_package(int p);

bool load_general_package( const char* szModuleFileName, const char* szComponentName );
void unload_general_package( const char* szModuleFileName );


bool AppInit( IApp* pApp );
void AppCleanup();