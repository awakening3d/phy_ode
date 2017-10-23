
#include "global.h"
#include "..\..\winds3d_sdk\i\logic.h"

class CLogic : public IW3D_Logic
{

SINGLETON_COMPONENT( CLogic )

	IApp* pApp;
public:
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, void* pUserContext=NULL );
	virtual bool Render();
	virtual bool FrameMove(float fTimeD);

	virtual ~CLogic();

	virtual bool OneTimeInit(IApp* pApp); // will be called when load component
	virtual bool FinalCleanup(); // will be called when unload component
};

IMPLEMENT_SINGLETON_COMPONENT( CLogic, logic )

BEGIN_PACKAGE_LIST

COMPONENT( logic )

END_PACKAGE_LIST




bool CLogic::OneTimeInit(IApp* pApp) // will be called when load component
{
	bool bret = IW3D_Logic::OneTimeInit(pApp);
	bret = bret && AppInit( pApp );
	this->pApp=pApp;
	return bret;
}

bool CLogic::FinalCleanup() // will be called when unload component
{
	AppCleanup();
	return IW3D_Logic::FinalCleanup();
}


CLogic::~CLogic()
{
}


bool CLogic::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, void* pUserContext )
{
	if ( gpui && gpui->MsgProc( hWnd, uMsg, wParam, lParam ) ) return true;
	if ( gpvr && gpvr->MsgProc( hWnd, uMsg, wParam, lParam ) ) return true;

	return false;
}

bool CLogic::Render()
{
	if (gpui) gpui->Render(); // dxui component
	if (gpvr) gpvr->Render(); // vr component


	return true;
}

bool CLogic::FrameMove(float fTimeD)
{
	if (gphy) gphy->StepSimulation(fTimeD);
	if (gpvr) gpvr->FrameMove(fTimeD); // vr component

	return true;
}
