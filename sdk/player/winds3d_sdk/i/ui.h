
// ui.h
// this file is not a part of winds3d_sdk, here just demonstrate how to use sdk:
// 1. define your interface (IW3D_UI)
// 2. implement the interface in dll (dxui.dll)
// 3. load dll & call interface in logic module (lg.dll)
// you can see practical at http://awingsoft.com/windrain/

#ifndef _winds3d_ui_h_
#define _winds3d_ui_h_

#include "..\com\component_d3d.h"

class IW3D_UI : public IComponentD3D
{
public:
    virtual HRESULT InitDeviceObjects( IDirect3DDevice9* pD3DDevice ) = 0;
    virtual HRESULT RestoreDeviceObjects() = 0;
    virtual HRESULT InvalidateDeviceObjects() = 0;
    virtual HRESULT DeleteDeviceObjects() = 0;
	
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, void* pUserContext=NULL ) = 0;

	virtual bool Render() = 0;
};


#endif
