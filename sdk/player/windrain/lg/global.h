#include "..\..\..\lua\include\lua.hpp"
#include "..\..\winds3d_sdk\i\ui.h"
#include "..\..\winds3d_sdk\i\vr.h"
#include "..\..\winds3d_sdk\i\phy.h"

extern IW3D_UI* gpui;
extern IW3D_VR* gpvr;
extern IW3D_PHY* gphy;

void load_package(int p);
void unload_package(int p);


bool AppInit( IApp* pApp );
void AppCleanup();