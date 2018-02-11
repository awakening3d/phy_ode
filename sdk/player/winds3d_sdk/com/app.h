
#ifndef _winds3d_app_h_
#define _winds3d_app_h_

#include <windows.h>
#include "component.h"

#define WINDS3D_SDK_VERSION 0x0004

typedef DWORD (*VersionOfSDK_Type)(); // SDK's version, always return WINDS3D_SDK_VERSION
// you should export a function from your dll:
// extern "C" DLL_EXPORT DWORD VersionOfSDK() { return WINDS3D_SDK_VERSION; }


typedef struct _dmMATRIX {
	float m[4][4];
} dmMATRIX;


class IApp
{
public:
	virtual bool LoadComponentPackage( const char* szFileName ) = 0;
	virtual bool UnloadComponentPackage( const char* szFileName ) = 0;

	virtual IComponent* CreateComponent( const char* szName ) = 0;
	virtual void ReleaseComponent( IComponent* pCom ) = 0;

	virtual const char* GetVersion() = 0; // get app version info
	virtual void OutputToConsole( LPCSTR szMsg, ... ) = 0;
	//thread safe version of CallLuaFunction()
	virtual int CallScriptFunction( LPCSTR szFunc, int nParam1=0, int nParam2=0, int nParam3=0, void* pvoid=0 ) = 0;
	virtual lua_State* GetLuaVM() = 0;

	virtual void GetViewMatrix( const dmMATRIX* *ppmatView, const dmMATRIX* *ppmatViewInv = NULL ) = 0;
	virtual void SetVRMatrix( const dmMATRIX* pmatEye, const dmMATRIX* pmatProj = NULL, int use=0 ) = 0;
	virtual void RenderFrame( int bbWidth, int bbHeight, int isRight ) = 0;
	virtual void Render2DContent(int x,int y,int width,int height, bool bClearBk=false) = 0;
};

#endif
