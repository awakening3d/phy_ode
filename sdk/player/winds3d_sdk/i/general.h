
// general component interface define

#ifndef _general_h_
#define _general_h_

#include "..\com\component_d3d.h"

#define GENERAL_COM_VERSION	10

class IW3D_GENERAL : public IComponentD3D
{
public:
	int Version()	{ return GENERAL_COM_VERSION; }

    virtual HRESULT InitDeviceObjects( IDirect3DDevice9* pD3DDevice ) = 0;
    virtual HRESULT RestoreDeviceObjects() = 0;
    virtual HRESULT InvalidateDeviceObjects() = 0;
    virtual HRESULT DeleteDeviceObjects() = 0;
	
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, void* pUserContext=NULL ) = 0;

	virtual bool Render( int nType=0 ) = 0;	// draw 3d content in this function
	virtual bool Render2D( int nType=0 ) = 0; // draw 2d content in this function
	virtual bool FrameMove(float fTimeD) = 0;
};


#endif
