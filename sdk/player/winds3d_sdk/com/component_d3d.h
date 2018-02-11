
#ifndef _winds3d_component_d3d_h_
#define _winds3d_component_d3d_h_

#include "component.h"
#include <d3d9.h>


// D3D Component Base Class
class IComponentD3D : public IComponent
{
	IDirect3DDevice9* m_pD3DDevice;
public:
	IComponentD3D() 	// constructor
	:	m_pD3DDevice( NULL )
	{
	}

	virtual ComponentType GetType() // get component type
	{
		return COM_D3D;
	}

    virtual HRESULT InitDeviceObjects( IDirect3DDevice9* pD3DDevice )
	{
		m_pD3DDevice = pD3DDevice;
		return S_OK;
	}

    virtual HRESULT RestoreDeviceObjects() = 0;
    virtual HRESULT InvalidateDeviceObjects() = 0;
    virtual HRESULT DeleteDeviceObjects() = 0;
};


#endif
