// winds3d logic component interface define

#ifndef _winds3d_logic_h_
#define _winds3d_logic_h_

#include "..\com\component.h"

class IW3D_Logic : public IComponent
{
public:
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, void* pUserContext=NULL ) = 0;

	virtual bool Render( int nType=0 ) = 0;	// draw 3d content in this function
	virtual bool Render2D( int nType=0 ) = 0; // draw 2d content in this function
	virtual bool VRRender(  int nType=0 ) = 0;

	virtual bool FrameMove(float fTimeD) = 0;
};


#endif
