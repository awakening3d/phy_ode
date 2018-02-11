//Draw.cpp

#include "Draw.h"
#include "util.h"
//#include "d3dapp/DXUtil.h"


CDraw& GetGCDraw() { //全局公用的draw
	static CDraw draw;
	return draw;
}


const float REPNUM = -.46875f; //修正2d坐标

struct DRAW2DVERTEX { Vec4 p;   DWORD color; };
struct DRAW3DVERTEX { Vec3 p;   DWORD color; };
struct DRAW2DTEXVERTEX { Vec4 p;   DWORD color; float u, v; };
struct DRAW3DTEXVERTEX { Vec3 p;   DWORD color; float u, v; };

#define D3DFVF_DRAW2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)
#define D3DFVF_DRAW3DVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)
#define D3DFVF_DRAW2DTEXVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_DRAW3DTEXVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

inline DRAW2DVERTEX InitDraw2DVertex(const Vec4& p, D3DCOLOR color)
{
	DRAW2DVERTEX v;   v.p = p;   v.color = color;
	return v;
}

inline DRAW3DVERTEX InitDraw3DVertex(const Vec3& p, D3DCOLOR color)
{
	DRAW3DVERTEX v;   v.p = p;   v.color = color;
	return v;
}

inline DRAW2DTEXVERTEX InitDraw2DTexVertex(const Vec4& p, D3DCOLOR color, float tu, float tv)
{
	DRAW2DTEXVERTEX v;   v.p = p;   v.color = color; v.u = tu; v.v = tv;
	return v;
}

inline DRAW3DTEXVERTEX InitDraw3DTexVertex(const Vec3& p, D3DCOLOR color, float tu, float tv)
{
	DRAW3DTEXVERTEX v;   v.p = p;   v.color = color; v.u = tu; v.v = tv;
	return v;
}


CDraw::CDraw()
{
	m_pd3dDevice = NULL;
	m_CurrentPosition2=Vec2(0.0f,0.0f); //当前点(2d)
	m_CurrentPosition3=Vec3(0.0f,0.0f,0.0f); //当前点(3d)
	m_OrgPosition=Vec2(0.0f,0.0f); //坐标原点
	m_eLineMode=LM_NORMAL; //画线模式
	m_dwColor=0x00000000; //前景色
	m_dwBkColor=0xffffffff; //背景色
	m_bZEnable=true;
	D3DXMatrixIdentity( &m_matWorld );

	m_eSrcBlend=D3DBLEND_SRCALPHA;
	m_eDestBlend=D3DBLEND_INVSRCALPHA;

	m_pVB=NULL; //顶点缓冲
	m_nVBSize=0;
	m_pVBData=NULL;
	m_nUsedSize=0;

    m_pFont        = new CD3DFont( _T("Arial"), 10, 0 );

    m_dwSavedStateBlock=0;
	m_dwDrawStateBlock=0;
	m_nPointsCount=0;
	m_nSegmentsCount=0;
}

CDraw::~CDraw()
{
//	SAFE_DELETE( m_pFont );
	SAFE_DELETE_ARRAY(m_pVBData);
}

bool CDraw::Point(const Vec2& Position2) //画点(2d)
{
	if (!m_pVBData) return false;
	SetVertexShader( D3DFVF_DRAW2DVERTEX, sizeof(DRAW2DVERTEX), D3DPT_POINTLIST );
	if (0xff000000!=(m_dwColor & 0xff000000)) m_DC.bAlphaBlend=TRUE;

    DRAW2DVERTEX* pVertices=(DRAW2DVERTEX*)GetVBBuffer(sizeof(DRAW2DVERTEX));
	//*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(Position2.x)+REPNUM,Geo2DeviceY(Position2.y)+REPNUM,0.0f,1.0f), m_dwColor );
	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(Position2.x),Geo2DeviceY(Position2.y),0.0f,1.0f), m_dwColor );

	DrawPrimitive(1);
	return true;
}

bool CDraw::Point(const Vec3& Position3) //画点(3d)
{
	if (!m_pVBData) return false;
    SetVertexShader( D3DFVF_DRAW3DVERTEX, sizeof(DRAW3DVERTEX), D3DPT_POINTLIST );
	if (0xff000000!=(m_dwColor & 0xff000000)) m_DC.bAlphaBlend=TRUE;

	DRAW3DVERTEX* pVertices=(DRAW3DVERTEX*)GetVBBuffer(sizeof(DRAW3DVERTEX));
	*pVertices++ = InitDraw3DVertex( Position3, m_dwColor );

	DrawPrimitive(1);
	return true;
}


//指定当前点(2d)
Vec2 CDraw::MoveTo(const Vec2& Position2)
{
	Vec2 tmpPos2=m_CurrentPosition2;
	m_CurrentPosition2=Position2;
	return tmpPos2;
}

//指定当前点(3d)
Vec3 CDraw::MoveTo(const Vec3& Position3)
{
	Vec3 tmpPos3=m_CurrentPosition3;
	m_CurrentPosition3=Position3;
	return tmpPos3;
}

//画线到指定点(2d)
bool CDraw::LineTo(const Vec2& Position2)
{
	if (!m_pVBData) return false;
    SetVertexShader( D3DFVF_DRAW2DVERTEX, sizeof(DRAW2DVERTEX), D3DPT_LINELIST );

	D3DCOLOR startcolor=m_dwColor;
	D3DCOLOR endcolor=m_dwColor;
	if (LM_GRADING==m_eLineMode) endcolor=m_dwBkColor;

	if (0xff000000 != (startcolor & 0xff000000) || 0xff000000 != (endcolor & 0xff000000)) m_DC.bAlphaBlend = TRUE;

    DRAW2DVERTEX* pVertices=(DRAW2DVERTEX*)GetVBBuffer(sizeof(DRAW2DVERTEX)*2);
	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(m_CurrentPosition2.x)+REPNUM,Geo2DeviceY(m_CurrentPosition2.y)+REPNUM,0.0f,1.0f), startcolor );
	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(Position2.x)+REPNUM,Geo2DeviceY(Position2.y)+REPNUM,0.0f,1.0f), endcolor );
//	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(m_CurrentPosition2.x),Geo2DeviceY(m_CurrentPosition2.y),0.0f,1.0f), startcolor );
//	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(Position2.x),Geo2DeviceY(Position2.y),0.0f,1.0f), endcolor );

	DrawPrimitive(1);
	
	m_CurrentPosition2=Position2;
	return true;
}


//画线到指定点(3d)
bool CDraw::LineTo(const Vec3& Position3)
{
	if (!m_pVBData) return false;
    SetVertexShader( D3DFVF_DRAW3DVERTEX, sizeof(DRAW3DVERTEX), D3DPT_LINELIST );

	D3DCOLOR startcolor=m_dwColor;
	D3DCOLOR endcolor=m_dwColor;
	if (LM_GRADING==m_eLineMode) endcolor=m_dwBkColor;

	if (0xff000000 != (startcolor & 0xff000000) || 0xff000000 != (endcolor & 0xff000000)) m_DC.bAlphaBlend = TRUE;

    DRAW3DVERTEX* pVertices=(DRAW3DVERTEX*)GetVBBuffer(sizeof(DRAW3DVERTEX)*2);
    *pVertices++ = InitDraw3DVertex( m_CurrentPosition3, startcolor );
	*pVertices++ = InitDraw3DVertex( Position3, endcolor );

	DrawPrimitive(1);
	
	m_CurrentPosition3=Position3;
	return true;
}

//画三角形(2d)
bool CDraw::Triangle(float x,float y,float x1,float y1,float x2,float y2)
{
	if (!m_pVBData) return false;
    SetVertexShader( D3DFVF_DRAW2DVERTEX, sizeof(DRAW2DVERTEX), D3DPT_TRIANGLELIST, true);
	if (0xff000000!=(m_dwColor & 0xff000000)) m_DC.bAlphaBlend=TRUE;

    DRAW2DVERTEX* pVertices=(DRAW2DVERTEX*)GetVBBuffer(sizeof(DRAW2DVERTEX)*3);
    *pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(x)+REPNUM,Geo2DeviceY(y)+REPNUM,0.0f,1.0f), m_dwColor );
	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(x1)+REPNUM,Geo2DeviceY(y1)+REPNUM,0.0f,1.0f), m_dwColor );
	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(x2)+REPNUM,Geo2DeviceY(y2)+REPNUM,0.0f,1.0f), m_dwColor );

	DrawPrimitive(1);

	return true;
}

//画三角形(3d)
bool CDraw::Triangle(const Vec3& v0,const Vec3& v1,const Vec3& v2)
{
	if (!m_pVBData) return false;
    SetVertexShader( D3DFVF_DRAW3DVERTEX, sizeof(DRAW3DVERTEX), D3DPT_TRIANGLELIST, true);
	if (0xff000000!=(m_dwColor & 0xff000000)) m_DC.bAlphaBlend=TRUE;

    DRAW3DVERTEX* pVertices=(DRAW3DVERTEX*)GetVBBuffer(sizeof(DRAW3DVERTEX)*3);
    *pVertices++ = InitDraw3DVertex( v0, m_dwColor );
	*pVertices++ = InitDraw3DVertex( v1, m_dwColor );
	*pVertices++ = InitDraw3DVertex( v2, m_dwColor );

	DrawPrimitive(1);

	return true;
}

//填充三角形(2d)
bool CDraw::FillTriangle(float x,float y,float x1,float y1,float x2,float y2)
{
	if (!m_pVBData) return false;
    SetVertexShader( D3DFVF_DRAW2DVERTEX, sizeof(DRAW2DVERTEX), D3DPT_TRIANGLELIST );
	if (0xff000000!=(m_dwBkColor & 0xff000000)) m_DC.bAlphaBlend=TRUE;

    DRAW2DVERTEX* pVertices=(DRAW2DVERTEX*)GetVBBuffer(sizeof(DRAW2DVERTEX)*3);
    *pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(x)+REPNUM,Geo2DeviceY(y)+REPNUM,0.0f,1.0f), m_dwBkColor );
	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(x1)+REPNUM,Geo2DeviceY(y1)+REPNUM,0.0f,1.0f), m_dwBkColor );
	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(x2)+REPNUM,Geo2DeviceY(y2)+REPNUM,0.0f,1.0f), m_dwBkColor );

	DrawPrimitive(1);
	
	return true;
}

//填充三角形(3d)
bool CDraw::FillTriangle(const Vec3& v0,const Vec3& v1,const Vec3& v2)
{
	if (!m_pVBData) return false;
    SetVertexShader( D3DFVF_DRAW3DVERTEX, sizeof(DRAW3DVERTEX), D3DPT_TRIANGLELIST);
	if (0xff000000!=(m_dwBkColor & 0xff000000)) m_DC.bAlphaBlend=TRUE;

    DRAW3DVERTEX* pVertices=(DRAW3DVERTEX*)GetVBBuffer(sizeof(DRAW3DVERTEX)*3);
    *pVertices++ = InitDraw3DVertex( v0, m_dwBkColor );
	*pVertices++ = InitDraw3DVertex( v1, m_dwBkColor );
	*pVertices++ = InitDraw3DVertex( v2, m_dwBkColor );

	DrawPrimitive(1);

	return true;
}


//画矩形
bool CDraw::Rect(const Vec4& rect)
{
	if (!m_pVBData) return false;
    SetVertexShader( D3DFVF_DRAW2DVERTEX, sizeof(DRAW2DVERTEX), D3DPT_LINELIST );
	if (0xff000000!=(m_dwColor & 0xff000000)) m_DC.bAlphaBlend=TRUE;

    DRAW2DVERTEX* pVertices=(DRAW2DVERTEX*)GetVBBuffer(sizeof(DRAW2DVERTEX)*8);
    *pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(rect.x),Geo2DeviceY(rect.y),0.0f,1.0f), m_dwColor );
	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(rect.z)-1,Geo2DeviceY(rect.y),0.0f,1.0f), m_dwColor );

	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(rect.z)-1,Geo2DeviceY(rect.y),0.0f,1.0f), m_dwColor );
	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(rect.z)-1,Geo2DeviceY(rect.w)-1,0.0f,1.0f), m_dwColor );

	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(rect.z)-1,Geo2DeviceY(rect.w)-1,0.0f,1.0f), m_dwColor );
	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(rect.x),Geo2DeviceY(rect.w)-1,0.0f,1.0f), m_dwColor );

	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(rect.x),Geo2DeviceY(rect.w)-1,0.0f,1.0f), m_dwColor );
	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(rect.x),Geo2DeviceY(rect.y),0.0f,1.0f), m_dwColor );

	DrawPrimitive(4);
	return true;
}

//填充矩形
bool CDraw::FillRect(const Vec4& rect) //填充矩形
{
	if (!m_pVBData) return false;
    SetVertexShader( D3DFVF_DRAW2DVERTEX, sizeof(DRAW2DVERTEX), D3DPT_TRIANGLELIST );
	if (0xff000000!=(m_dwBkColor & 0xff000000)) m_DC.bAlphaBlend=TRUE;

    DRAW2DVERTEX* pVertices=(DRAW2DVERTEX*)GetVBBuffer(sizeof(DRAW2DVERTEX)*6);
    *pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(rect.x)+REPNUM,Geo2DeviceY(rect.w)+REPNUM,0.0f,1.0f), m_dwBkColor );
	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(rect.x)+REPNUM,Geo2DeviceY(rect.y)+REPNUM,0.0f,1.0f), m_dwBkColor );
	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(rect.z)+REPNUM,Geo2DeviceY(rect.w)+REPNUM,0.0f,1.0f), m_dwBkColor );

	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(rect.x)+REPNUM,Geo2DeviceY(rect.y)+REPNUM,0.0f,1.0f), m_dwBkColor );
	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(rect.z)+REPNUM,Geo2DeviceY(rect.y)+REPNUM,0.0f,1.0f), m_dwBkColor );
	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(rect.z)+REPNUM,Geo2DeviceY(rect.w)+REPNUM,0.0f,1.0f), m_dwBkColor );

	DrawPrimitive(2);

	return true;
}

//画圆(2d)
bool CDraw::Circularity(const Vec2& vCenter,float fRadius,int nSegment/*=16*/)
{
	if (!m_pVBData) return false;
	if (nSegment<3) return false;

	SetVertexShader( D3DFVF_DRAW2DVERTEX, sizeof(DRAW2DVERTEX), D3DPT_LINESTRIP);
	if (0xff000000!=(m_dwColor & 0xff000000)) m_DC.bAlphaBlend=TRUE;

	float fAngle=(float)(g_PI*2.0f/nSegment);
	Vec2 vStart(fRadius,0.0f);

    DRAW2DVERTEX* pVertices=(DRAW2DVERTEX*)GetVBBuffer(sizeof(DRAW2DVERTEX)*(nSegment+1));

    *pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(vStart.x+vCenter.x)+REPNUM,Geo2DeviceY(vStart.y+vCenter.y)+REPNUM,0.0f,1.0f), m_dwColor );
	for (int i=1;i<nSegment;i++)
		*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(cosf(fAngle*i)*fRadius+vCenter.x)+REPNUM,Geo2DeviceY(sinf(fAngle*i)*fRadius+vCenter.y)+REPNUM,0.0f,1.0f), m_dwColor );
	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(vStart.x+vCenter.x)+REPNUM,Geo2DeviceY(vStart.y+vCenter.y)+REPNUM,0.0f,1.0f), m_dwColor );


	DrawPrimitive( nSegment );
	Flush();
	return true;
}

//画圆(3d)
bool CDraw::Circularity(const Vec3& vCenter,const Vec3& vDir,float fRadius,int nSegment/*=16*/,bool bOldMode/*=false*/)
{
	if (!m_pVBData) return false;
	if (nSegment<3) return false;

	SetVertexShader( D3DFVF_DRAW3DVERTEX, sizeof(DRAW3DVERTEX), D3DPT_LINESTRIP);
	if (0xff000000!=(m_dwColor & 0xff000000)) m_DC.bAlphaBlend=TRUE;

	float fAngle=(float)(g_PI*2.0f/nSegment);
	Vec3 vStart(fRadius,0.0f,0.0f);

    DRAW3DVERTEX* pVertices=(DRAW3DVERTEX*)GetVBBuffer(sizeof(DRAW3DVERTEX)*(nSegment+1));

	*pVertices++ = InitDraw3DVertex( vStart, m_dwColor );
	for (int i=1;i<nSegment;i++)
		*pVertices++ = InitDraw3DVertex(Vec3(cosf(fAngle*i)*fRadius,sinf(fAngle*i)*fRadius,0.0f), m_dwColor );
	*pVertices++ = InitDraw3DVertex(vStart, m_dwColor );

	//保存并设置world矩阵
    D3DXMATRIX  matWorldFinal;
	Vec3 vSour(0.0f,0.0f,1.0f),vDest;
	D3DXVec3Normalize(&vDest,&vDir);

	D3DXMATRIX mr,mt;
	if (dmIsEqual(vDest.z,1.0f))
		D3DXMatrixIdentity(&mr);
	else if (dmIsEqual(vDest.z,-1.0f))
		D3DXMatrixRotationY(&mr,g_DEGTORAD * 180);
	else {
		float fACos=acosf(D3DXVec3Dot(&vSour,&vDest)); //两个方向的夹角
		Vec3 vAxis;
		D3DXVec3Cross(&vAxis,&vSour,&vDest); //转轴
		D3DXMatrixRotationAxis(&mr,&vAxis,fACos);
	}

	D3DXMatrixTranslation( &mt, vCenter.x,vCenter.y,vCenter.z );
	if (bOldMode) {
		D3DXMatrixMultiply( &matWorldFinal, &mr, &mt );
		D3DXMatrixMultiply( &matWorldFinal, &m_matWorld, &matWorldFinal );
	} else {
		matWorldFinal=mr*mt*m_matWorld;
	}


	D3DXMATRIX matSave=SetMatWorld(matWorldFinal);

	DrawPrimitive( nSegment );

	Flush();

	SetMatWorld(matSave);

	return true;
}

//填充圆(2d)
bool CDraw::FillCircularity(const Vec2& vCenter,float fRadius,int nSegment)
{
	if (!m_pVBData) return false;
	if (nSegment<3) return false;

    SetVertexShader( D3DFVF_DRAW2DVERTEX, sizeof(DRAW2DVERTEX), D3DPT_TRIANGLEFAN);
	if (0xff000000!=(m_dwBkColor & 0xff000000)) m_DC.bAlphaBlend=TRUE;


	float fAngle=(float)(g_PI*2.0f/nSegment);
	Vec2 vStart(fRadius,0.0f);

    DRAW2DVERTEX* pVertices=(DRAW2DVERTEX*)GetVBBuffer(sizeof(DRAW2DVERTEX)*(nSegment+2));


	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(vCenter.x)+REPNUM,Geo2DeviceY(vCenter.y)+REPNUM,0.0f,1.0f), m_dwBkColor );
    *pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(vStart.x+vCenter.x)+REPNUM,Geo2DeviceY(vStart.y+vCenter.y)+REPNUM,0.0f,1.0f), m_dwBkColor );

	for (int i=1;i<nSegment;i++)
		*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(cosf(fAngle*i)*fRadius+vCenter.x)+REPNUM,Geo2DeviceY(sinf(fAngle*i)*fRadius+vCenter.y)+REPNUM,0.0f,1.0f), m_dwBkColor );

	*pVertices++ = InitDraw2DVertex( Vec4(Geo2DeviceX(vStart.x+vCenter.x)+REPNUM,Geo2DeviceY(vStart.y+vCenter.y)+REPNUM,0.0f,1.0f), m_dwBkColor );

	DrawPrimitive( nSegment );
	Flush();

	return true;
}

//填充圆(3d)
bool CDraw::FillCircularity(const Vec3& vCenter,const Vec3& vDir,float fRadius,int nSegment,bool bOldMode/*=false*/)
{
	if (!m_pVBData) return false;
	if (nSegment<3) return false;

    SetVertexShader( D3DFVF_DRAW3DVERTEX, sizeof(DRAW3DVERTEX), D3DPT_TRIANGLEFAN);
	if (0xff000000!=(m_dwBkColor & 0xff000000)) m_DC.bAlphaBlend=TRUE;


	float fAngle=(float)(g_PI*2.0f/nSegment);
	Vec3 vStart(fRadius,0.0f,0.0f);

    DRAW3DVERTEX* pVertices=(DRAW3DVERTEX*)GetVBBuffer(sizeof(DRAW3DVERTEX)*(nSegment+2));

	*pVertices++ = InitDraw3DVertex( Vec3(0,0,0), m_dwBkColor );
	*pVertices++ = InitDraw3DVertex( vStart, m_dwBkColor );
	for (int i=1;i<nSegment;i++)
		*pVertices++ = InitDraw3DVertex(Vec3(cosf(fAngle*i)*fRadius,sinf(fAngle*i)*fRadius,0.0f), m_dwBkColor );
	*pVertices++ = InitDraw3DVertex( vStart, m_dwBkColor );

	//保存并设置world矩阵
    D3DXMATRIX  matWorldFinal;
	Vec3 vSour(0.0f,0.0f,1.0f),vDest;
	D3DXVec3Normalize(&vDest,&vDir);

	D3DXMATRIX mr,mt;
	if (dmIsEqual(vDest.z,1.0f))
		D3DXMatrixIdentity(&mr);
	else if (dmIsEqual(vDest.z,-1.0f))
		D3DXMatrixRotationY(&mr,g_DEGTORAD * 180);
	else {
		float fACos=acosf(D3DXVec3Dot(&vSour,&vDest)); //两个方向的夹角
		Vec3 vAxis;
		D3DXVec3Cross(&vAxis,&vSour,&vDest); //转轴
		D3DXMatrixRotationAxis(&mr,&vAxis,fACos);
	}


	D3DXMatrixTranslation( &mt, vCenter.x,vCenter.y,vCenter.z );
	if (bOldMode) {
		D3DXMatrixMultiply( &matWorldFinal, &mr, &mt );
		D3DXMatrixMultiply( &matWorldFinal, &m_matWorld, &matWorldFinal );
	} else {
		matWorldFinal=mr*mt*m_matWorld;
	}


	D3DXMATRIX matSave=SetMatWorld(matWorldFinal);

	DrawPrimitive( nSegment );
	Flush();

	SetMatWorld(matSave);

	return true;
}

//画盒子
bool CDraw::Box(const Vec3& vMin,const Vec3& vMax)
{
	if (!m_pVBData) return false;
    SetVertexShader( D3DFVF_DRAW3DVERTEX, sizeof(DRAW3DVERTEX), D3DPT_LINELIST);
	if (0xff000000!=(m_dwColor & 0xff000000)) m_DC.bAlphaBlend=TRUE;

	Vec3 ver[8];
	ver[0]=Vec3(vMin.x,vMin.y,vMin.z);
	ver[1]=Vec3(vMin.x,vMin.y,vMax.z);
	ver[2]=Vec3(vMax.x,vMin.y,vMin.z);
	ver[3]=Vec3(vMax.x,vMin.y,vMax.z);
	ver[4]=Vec3(vMin.x,vMax.y,vMin.z);
	ver[5]=Vec3(vMin.x,vMax.y,vMax.z);
	ver[6]=Vec3(vMax.x,vMax.y,vMin.z);
	ver[7]=Vec3(vMax.x,vMax.y,vMax.z);

    DRAW3DVERTEX* pVertices=(DRAW3DVERTEX*)GetVBBuffer(sizeof(DRAW3DVERTEX)*24);
	*pVertices++ = InitDraw3DVertex( ver[0], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[1], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[1], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[3], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[3], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[2], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[2], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[0], m_dwColor );

	*pVertices++ = InitDraw3DVertex( ver[4], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[5], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[5], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[7], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[7], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[6], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[6], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[4], m_dwColor );

	*pVertices++ = InitDraw3DVertex( ver[0], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[4], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[1], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[5], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[3], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[7], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[2], m_dwColor );
	*pVertices++ = InitDraw3DVertex( ver[6], m_dwColor );

	DrawPrimitive(12);

	return true;
}

//画圆柱
bool CDraw::Cylinder(const Vec3& vCenter,const Vec3& vDir,float fHeight,float fRadius1,float fRadius2,int nSegment,bool bOldMode/*=false*/)
{
	if (nSegment<3) return false;
	if (NULL==m_pd3dDevice) return false;

	if (m_nVBSize < (nSegment*4+2)*sizeof(DRAW3DVERTEX)) return false;

	Flush();

    m_pd3dDevice->SetVertexShader( NULL );
	m_pd3dDevice->SetFVF( D3DFVF_DRAW3DVERTEX );

    m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(DRAW3DVERTEX) );

	float fAngle=(float)(g_PI*2.0f/nSegment);

    DRAW3DVERTEX* pVertices;
    if ( FAILED(m_pVB->Lock( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD ))) return false;
	int i = 0;
	//circularity of radius1
	Vec3 vStart(fRadius1,0.0f,0.0f);
	DRAW3DVERTEX* pVertices1=pVertices;
	*pVertices++ = InitDraw3DVertex( vStart, m_dwColor );
	for (i=1;i<nSegment;i++)
		*pVertices++ = InitDraw3DVertex(Vec3(cosf(fAngle*i)*fRadius1,sinf(fAngle*i)*fRadius1,0.0f), m_dwColor );
	*pVertices++ = InitDraw3DVertex(vStart, m_dwColor );

	//circularity of radius2
	vStart=Vec3(fRadius2,0,fHeight);
	DRAW3DVERTEX* pVertices2=pVertices;
	*pVertices++ = InitDraw3DVertex( vStart, m_dwColor );
	for (i=1;i<nSegment;i++)
		*pVertices++ = InitDraw3DVertex(Vec3(cosf(fAngle*i)*fRadius2,sinf(fAngle*i)*fRadius2,fHeight), m_dwColor );
	*pVertices++ = InitDraw3DVertex(vStart, m_dwColor );

	//sides
	for (i=0;i<nSegment;i++) {
		*pVertices++ = InitDraw3DVertex( (*pVertices1++).p, m_dwColor );
		*pVertices++ = InitDraw3DVertex( (*pVertices2++).p, m_dwColor );
	}

	m_pVB->Unlock();

	//保存并设置world矩阵
    D3DXMATRIX  matWorldFinal,matWorldSave;
	Vec3 vSour(0.0f,0.0f,1.0f),vDest;
	D3DXVec3Normalize(&vDest,&vDir);

	D3DXMATRIX mr,mt;
	if (dmIsEqual(vDest.z,1.0f))
		D3DXMatrixIdentity(&mr);
	else if (dmIsEqual(vDest.z,-1.0f))
		D3DXMatrixRotationY(&mr,g_DEGTORAD * 180);
	else {
		float fACos=acosf(D3DXVec3Dot(&vSour,&vDest)); //两个方向的夹角
		Vec3 vAxis;
		D3DXVec3Cross(&vAxis,&vSour,&vDest); //转轴
		D3DXMatrixRotationAxis(&mr,&vAxis,fACos);
	}


	D3DXMatrixTranslation( &mt, vCenter.x,vCenter.y,vCenter.z );
	if (bOldMode) {
		D3DXMatrixMultiply( &matWorldFinal, &mr, &mt );
		D3DXMatrixMultiply( &matWorldFinal, &m_matWorld, &matWorldFinal );
	} else {
		matWorldFinal=mr*mt*m_matWorld;
	}


    m_pd3dDevice->GetTransform( D3DTS_WORLD, &matWorldSave );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorldFinal );

    // Setup renderstate
	if (m_dwSavedStateBlock) m_dwSavedStateBlock->Capture();
	if (m_dwDrawStateBlock) m_dwDrawStateBlock->Apply();


	if (!m_bZEnable) m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

	if (0xff000000!=(m_dwColor & 0xff000000)) {
		// Enable alpha blending
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,   m_eSrcBlend );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,  m_eDestBlend );
	}

	m_pd3dDevice->DrawPrimitive( D3DPT_LINESTRIP, 0, nSegment );
	m_pd3dDevice->DrawPrimitive( D3DPT_LINESTRIP, nSegment+1, nSegment );
	m_pd3dDevice->DrawPrimitive( D3DPT_LINELIST,  nSegment*2+2, nSegment );

	if (m_dwSavedStateBlock) m_dwSavedStateBlock->Apply();

	//恢复world矩阵
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorldSave );

	return true;
}


//画纹理矩形
bool CDraw::Blt(int x,int y,LPDIRECT3DTEXTURE9 pTexture)
{
	if (NULL==m_pd3dDevice || NULL==m_pVB) return false;
	if (NULL==pTexture) return false;

	D3DSURFACE_DESC desc;
	pTexture->GetLevelDesc(0,&desc);
	Vec4 rect((float)x,(float)y,(float)(x+(int)desc.Width),(float)(y+(int)desc.Height));
	if (rect.x<0 && rect.z<0) return false;
	if (rect.y<0 && rect.w<0) return false;

	Flush();

    m_pd3dDevice->SetVertexShader( NULL );
	m_pd3dDevice->SetFVF( D3DFVF_DRAW2DTEXVERTEX );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(DRAW2DTEXVERTEX) );

    DRAW2DTEXVERTEX* pVertices;
    if (FAILED(m_pVB->Lock( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD ))) return false;
    *pVertices++ = InitDraw2DTexVertex( Vec4(Geo2DeviceX(rect.x)+REPNUM,Geo2DeviceY(rect.w)+REPNUM,0.0f,1.0f), m_dwBkColor, 0.0f, 1.0f );
	*pVertices++ = InitDraw2DTexVertex( Vec4(Geo2DeviceX(rect.x)+REPNUM,Geo2DeviceY(rect.y)+REPNUM,0.0f,1.0f), m_dwBkColor, 0.0f, 0.0f );
	*pVertices++ = InitDraw2DTexVertex( Vec4(Geo2DeviceX(rect.z)+REPNUM,Geo2DeviceY(rect.w)+REPNUM,0.0f,1.0f), m_dwBkColor, 1.0f, 1.0f );
	*pVertices++ = InitDraw2DTexVertex( Vec4(Geo2DeviceX(rect.z)+REPNUM,Geo2DeviceY(rect.y)+REPNUM,0.0f,1.0f), m_dwBkColor, 1.0f, 0.0f );
	m_pVB->Unlock();
	
    // Setup renderstate
	if (m_dwSavedStateBlock) m_dwSavedStateBlock->Capture();
	if (m_dwDrawStateBlock) m_dwDrawStateBlock->Apply();

	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,   m_eSrcBlend );
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,  m_eDestBlend );


	//设置渲染状态
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	m_pd3dDevice->SetTexture(0,pTexture);

	m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0, 2 );
	
	if (m_dwSavedStateBlock) m_dwSavedStateBlock->Apply();


	return true;
}

//画纹理矩形(拉伸)
bool CDraw::StretchBlt(const Vec4& rect,LPDIRECT3DTEXTURE9 pTexture, Vec4* psrcrect/*=NULL*/)
{
	if (NULL==m_pd3dDevice || NULL==m_pVB) return false;

	Flush();

    m_pd3dDevice->SetVertexShader( NULL );
	m_pd3dDevice->SetFVF( D3DFVF_DRAW2DTEXVERTEX );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(DRAW2DTEXVERTEX) );

	float left = 0, right = 1, top=0, bottom=1;
	if (psrcrect && pTexture) {
		D3DSURFACE_DESC desc;
		pTexture->GetLevelDesc(0,&desc);
		float w=desc.Width, h=desc.Height;
		left = psrcrect->x / w;
		right = psrcrect->z / w;
		top = psrcrect->y / h;
		bottom = psrcrect->w /h;
	}


    DRAW2DTEXVERTEX* pVertices;
    if (FAILED(m_pVB->Lock( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD ))) return false;
    *pVertices++ = InitDraw2DTexVertex( Vec4(Geo2DeviceX(rect.x)+REPNUM,Geo2DeviceY(rect.w)+REPNUM,0.0f,1.0f), m_dwBkColor, left, bottom );
	*pVertices++ = InitDraw2DTexVertex( Vec4(Geo2DeviceX(rect.x)+REPNUM,Geo2DeviceY(rect.y)+REPNUM,0.0f,1.0f), m_dwBkColor, left, top );
	*pVertices++ = InitDraw2DTexVertex( Vec4(Geo2DeviceX(rect.z)+REPNUM,Geo2DeviceY(rect.w)+REPNUM,0.0f,1.0f), m_dwBkColor, right, bottom );
	*pVertices++ = InitDraw2DTexVertex( Vec4(Geo2DeviceX(rect.z)+REPNUM,Geo2DeviceY(rect.y)+REPNUM,0.0f,1.0f), m_dwBkColor, right, top );
	m_pVB->Unlock();
	
    // Setup renderstate
	if (m_dwSavedStateBlock) m_dwSavedStateBlock->Capture();
	if (m_dwDrawStateBlock) m_dwDrawStateBlock->Apply();


	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,   m_eSrcBlend );
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,  m_eDestBlend );


	//设置渲染状态
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	m_pd3dDevice->SetTexture(0,pTexture);

	m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0, 2 );

	if (m_dwSavedStateBlock) m_dwSavedStateBlock->Apply();

	return true;
}


//画纹理矩形(拉伸)
bool CDraw::StretchBlt3D(const Vec3& org, const Vec3& width, const Vec3& height, LPDIRECT3DTEXTURE9 pTexture, Vec4* psrcrect)
{
	if (NULL == m_pd3dDevice || NULL == m_pVB) return false;

	Flush();

	m_pd3dDevice->SetVertexShader(NULL);
	m_pd3dDevice->SetFVF(D3DFVF_DRAW3DTEXVERTEX);
	m_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(DRAW3DTEXVERTEX));

	float left = 0, right = 1, top = 0, bottom = 1;
	if (psrcrect && pTexture) {
		D3DSURFACE_DESC desc;
		pTexture->GetLevelDesc(0, &desc);
		float w = desc.Width, h = desc.Height;
		left = psrcrect->x / w;
		right = psrcrect->z / w;
		top = psrcrect->y / h;
		bottom = psrcrect->w / h;
	}


	DRAW3DTEXVERTEX* pVertices;
	if (FAILED(m_pVB->Lock(0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD))) return false;
	*pVertices++ = InitDraw3DTexVertex( org + height, m_dwBkColor, left, bottom);
	*pVertices++ = InitDraw3DTexVertex( org, m_dwBkColor, left, top);
	*pVertices++ = InitDraw3DTexVertex( org+width+height, m_dwBkColor, right, bottom);
	*pVertices++ = InitDraw3DTexVertex( org+width, m_dwBkColor, right, top);
	m_pVB->Unlock();

	// Setup renderstate
	if (m_dwSavedStateBlock) m_dwSavedStateBlock->Capture();
	if (m_dwDrawStateBlock) m_dwDrawStateBlock->Apply();


	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, m_eSrcBlend);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, m_eDestBlend);


	//设置渲染状态
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	m_pd3dDevice->SetTexture(0, pTexture);

	m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	if (m_dwSavedStateBlock) m_dwSavedStateBlock->Apply();

	return true;
}


//设置前景色
DWORD CDraw::SetColor(D3DCOLOR dwColor)
{
	DWORD dwTmp=m_dwColor;
	m_dwColor=dwColor;
	return dwTmp;
}

//设置背景色
DWORD CDraw::SetBkColor(D3DCOLOR dwBkColor)
{
	DWORD dwTmp=m_dwBkColor;
	m_dwBkColor=dwBkColor;
	return dwTmp;
}

//设置视区
bool CDraw::SetViewport(RECT vrect)
{
    // get render target surface desc
    LPDIRECT3DSURFACE9 pRenderTarget;
	D3DSURFACE_DESC rtdesc;
    m_pd3dDevice->GetRenderTarget(0,&pRenderTarget);
    pRenderTarget->GetDesc( &rtdesc );
    pRenderTarget->Release();

	if (vrect.left<0 && vrect.right<0) return false;
	if (vrect.top<0 && vrect.bottom<0) return false;
	if (vrect.left>=(int)rtdesc.Width && vrect.right>=(int)rtdesc.Width) return false;
	if (vrect.top>=(int)rtdesc.Height && vrect.bottom>=(int)rtdesc.Height) return false;

	if (vrect.left<0) vrect.left=0;
	if (vrect.top<0) vrect.top=0;
	if (vrect.right>(int)rtdesc.Width) vrect.right=rtdesc.Width;
	if (vrect.bottom>(int)rtdesc.Height) vrect.bottom=rtdesc.Height;

	if (FAILED(m_pd3dDevice->GetViewport(&m_SaveViewport))) return false;
	D3DVIEWPORT9 vp=m_SaveViewport;
	vp.X=vrect.left;
	vp.Y=vrect.top;
	vp.Width=vrect.right-vrect.left;
	vp.Height=vrect.bottom-vrect.top;

	if (FAILED(m_pd3dDevice->SetViewport(&vp))) {
		m_pd3dDevice->SetViewport(&m_SaveViewport);
		return false;
	}
	return true;
}

//恢复视区
bool CDraw::RestoreViewport()
{
	if (FAILED(m_pd3dDevice->SetViewport(&m_SaveViewport))) return false;
	return true;
}

//设置画线模式
bool CDraw::SetLineMode(LINEMODE lm)
{
	m_eLineMode=lm;
	return true;
}

bool CDraw::CheckVBSize(int nVBSize)
{
	if (nVBSize<=m_nVBSize) return true;

	SAFE_RELEASE(m_pVB);
	SAFE_DELETE_ARRAY(m_pVBData);
	m_nVBSize=0;

    // Create vertex buffer
    if( FAILED(m_pd3dDevice->CreateVertexBuffer( nVBSize,
                                                       D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0,
                                                       D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
    {
        return false;
    }
	m_pVBData=new BYTE[nVBSize];
	if (!m_pVBData) return false;
	
	m_nVBSize=nVBSize;
	m_nUsedSize=0;

	return true;
}


BYTE* CDraw::GetVBBuffer(int nDataSize)
{
	if (!m_pVBData) return NULL;
	if (nDataSize>m_nVBSize) return NULL;
	if (m_nUsedSize+nDataSize > m_nVBSize) Flush();
	m_nUsedSize+=nDataSize;
	return m_pVBData+m_nUsedSize-nDataSize;
}


void CDraw::SetVertexShader(DWORD dwShader,UINT Stride,D3DPRIMITIVETYPE PrimitiveType,bool bWireFrame)
{
	if ((dwShader!=m_DC.dwVShader) ||
		(Stride!=m_DC.nStride)	||
		(PrimitiveType!=m_DC.PrimitiveType) ||
		(bWireFrame!=m_DC.bWireFrame))
	{
		Flush();
	}

	m_DC.dwVShader=dwShader;
	m_DC.nStride=Stride;
	m_DC.PrimitiveType=PrimitiveType;
	m_DC.bWireFrame=bWireFrame;
}

void CDraw::DrawPrimitive(UINT PrimitiveCount)
{
	m_DC.PrimitiveCount+=PrimitiveCount;
}

D3DXMATRIX CDraw::SetMatWorld(const D3DXMATRIX& mat)
{
	if (mat!=m_matWorld) Flush();
	D3DXMATRIX tmp=m_matWorld;
	m_matWorld=mat;
	return tmp;
}

bool CDraw::SetZEnable(bool bZEnable)
{
	if (bZEnable!=m_bZEnable) Flush();
	bool tmp=m_bZEnable; m_bZEnable=bZEnable; return tmp;
}

LPDIRECT3DSURFACE9 CDraw::GetRenderTarget(int nRTIndex)
{
	if (NULL==m_pd3dDevice) return NULL;

	LPDIRECT3DSURFACE9 pSurfLDR=NULL;
	m_pd3dDevice->GetRenderTarget(nRTIndex, &pSurfLDR );
	return pSurfLDR;
}

bool CDraw::SetRenderTarget(LPDIRECT3DSURFACE9 pSurf,int nRTIndex/*=0*/)
{
	if (NULL==m_pd3dDevice) return false;
	return SUCCEEDED(m_pd3dDevice->SetRenderTarget(nRTIndex,pSurf));
}


void CDraw::Flush()
{
	if (0==m_DC.PrimitiveCount) return;
	if (NULL==m_pd3dDevice || NULL==m_pVB || NULL==m_pVBData) return;

	BYTE* pBuf;
	if ( FAILED(m_pVB->Lock( 0, 0, (VOID**)&pBuf, D3DLOCK_DISCARD )) ) return;
	memcpy(pBuf,m_pVBData,m_nUsedSize);
	m_pVB->Unlock();

    m_pd3dDevice->SetVertexShader( NULL );
	m_pd3dDevice->SetFVF( m_DC.dwVShader );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, m_DC.nStride );

	//保存并设置world矩阵
    D3DXMATRIX  matWorldSave;
    m_pd3dDevice->GetTransform( D3DTS_WORLD, &matWorldSave );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

    // Setup renderstate
	if (m_dwSavedStateBlock) m_dwSavedStateBlock->Capture();
	if (m_dwDrawStateBlock) m_dwDrawStateBlock->Apply();


	if (!m_bZEnable) m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	if (m_DC.bWireFrame) m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	if (m_DC.bAlphaBlend) {
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,   m_eSrcBlend );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,  m_eDestBlend );
	}


	if (D3DFVF_XYZRHW & m_DC.dwVShader ) m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	m_pd3dDevice->DrawPrimitive( m_DC.PrimitiveType, 0, m_DC.PrimitiveCount );
	
	if (m_dwSavedStateBlock) m_dwSavedStateBlock->Apply();
	
	//恢复world矩阵
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorldSave );

	m_nUsedSize=0;
	m_DC.SetDefault();
}


bool CDraw::TextOut( int x, int y, TCHAR* szText, D3DCOLOR color, float fWidthScale, float fHeightScale )
{
	Flush();
	return m_pFont->DrawText(x,y, szText,color);//,fWidthScale,fHeightScale);
}


bool CDraw::TextOut3D(const Vec3& vOrg, const Vec3& vWidthDir, const Vec3& vHeightDir, TCHAR* szText)
{
	Flush();
	return m_pFont->Render3DText(vOrg, vWidthDir, vHeightDir, szText);
}

SIZE CDraw::GetTextExtent(TCHAR* szText) const
{
	SIZE sz;
	m_pFont->GetTextExtent(szText, &sz);
	return sz;
}


//d3d设备相关初始化
HRESULT CDraw::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{	
	m_pd3dDevice = pd3dDevice;
	m_pFont->InitDeviceObjects(pd3dDevice);
	return S_OK;
}

HRESULT CDraw::RestoreDeviceObjects()
{

	m_pFont->RestoreDeviceObjects();

	if (!CheckVBSize(1024*32)) return E_FAIL;

    // Create the state blocks for rendering text
    for( UINT which=0; which<2; which++ )
    {
        m_pd3dDevice->BeginStateBlock();
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
        m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
        m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
        m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_CLIPPING,         TRUE );
//        m_pd3dDevice->SetRenderState( D3DRS_EDGEANTIALIAS,    FALSE );
//        m_pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND,      FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,        FALSE );
		
	    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

        if( which==0 )
            m_pd3dDevice->EndStateBlock( &m_dwSavedStateBlock );
        else
            m_pd3dDevice->EndStateBlock( &m_dwDrawStateBlock );
    }

	return S_OK;
}

HRESULT CDraw::InvalidateDeviceObjects()
{
	SAFE_RELEASE(m_pVB);
	SAFE_DELETE_ARRAY(m_pVBData);
	m_nVBSize=0;


	m_pFont->InvalidateDeviceObjects();

    // Delete the state blocks
    if( m_pd3dDevice )
    {
        if( m_dwSavedStateBlock )
			m_dwSavedStateBlock->Release();
        if( m_dwDrawStateBlock )
			m_dwDrawStateBlock->Release();
    }

    m_dwSavedStateBlock		= 0L;
    m_dwDrawStateBlock		= 0L;

	return S_OK;
}

HRESULT CDraw::DeleteDeviceObjects()
{
	m_pFont->DeleteDeviceObjects();
	return S_OK;
}



bool CDraw::ClearFloatRenderTarget(int width, int height) // fill float render target with 0
{
	if (!m_pVBData) return false;

    SetVertexShader( D3DFVF_DRAW2DVERTEX, sizeof(DRAW2DVERTEX), D3DPT_TRIANGLELIST );

    DRAW2DVERTEX* pVertices=(DRAW2DVERTEX*)GetVBBuffer(sizeof(DRAW2DVERTEX)*6);
    *pVertices++ = InitDraw2DVertex( Vec4(0,height,0.0f,1.0f), 0x00000000L );
	*pVertices++ = InitDraw2DVertex( Vec4(0,0,0.0f,1.0f), 0x00000000L );
	*pVertices++ = InitDraw2DVertex( Vec4(width,height,0.0f,1.0f), 0x00000000L );

	*pVertices++ = InitDraw2DVertex( Vec4(0,0,0.0f,1.0f), 0x00000000L );
	*pVertices++ = InitDraw2DVertex( Vec4(width,0,0.0f,1.0f), 0x00000000L );
	*pVertices++ = InitDraw2DVertex( Vec4(width,height,0.0f,1.0f), 0x00000000L );

	DrawPrimitive(2);
	Flush();

	return true;
}
