#include "..\..\..\lua\include\lua.hpp"
#include "..\..\winds3d_sdk\i\ui.h"
#include "..\..\winds3d_sdk\i\phy.h"
//#include "..\..\winds3d_sdk\i\vr.h"

extern IW3D_UI* gpui;
extern IW3D_PHY* gphy;
//extern IW3D_VR* gpvr;


bool AppInit( IApp* pApp );
void AppCleanup();