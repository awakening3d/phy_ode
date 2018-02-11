
// winds3d vr component interface define

#ifndef _winds3d_vr_h_
#define _winds3d_vr_h_

#include "..\com\component_d3d.h"

class IW3D_VR : public IComponentD3D
{
public:
    virtual HRESULT InitDeviceObjects( IDirect3DDevice9* pD3DDevice ) = 0;
    virtual HRESULT RestoreDeviceObjects() = 0;
    virtual HRESULT InvalidateDeviceObjects() = 0;
    virtual HRESULT DeleteDeviceObjects() = 0;
	
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, void* pUserContext=NULL ) = 0;

	virtual bool Render() = 0;
	virtual bool FrameMove(float fTimeD) = 0;
};


#endif
