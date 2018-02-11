//Draw.h 作图类

#ifndef Draw_H
#define Draw_H

#include "util.h"
#include "D3DFont.h"


typedef enum LINEMODE { LM_NORMAL,LM_GRADING };

#define POINTSBUF_SIZE	1024


typedef struct _DRAWCONTEXT {
	DWORD dwVShader;	//vertex shader
	UINT	nStride;	// size of vertex struct
	bool bWireFrame;
	bool bAlphaBlend;
	D3DPRIMITIVETYPE PrimitiveType;
	UINT	PrimitiveCount;


	void SetDefault() {
		dwVShader=0;
		nStride=0;
		bWireFrame=false;
		bAlphaBlend=false;
		PrimitiveType=D3DPT_POINTLIST;
		PrimitiveCount=0;
	}
	_DRAWCONTEXT()
	{
		SetDefault();
	}

} DRAWCONTEXT;

class CDraw
{
protected:
	LPDIRECT3DDEVICE9       m_pd3dDevice; // A D3DDevice used for rendering

protected:
	Vec2 m_CurrentPosition2; //当前点(2d)
	Vec3 m_CurrentPosition3; //当前点(3d)
	Vec2	m_OrgPosition; //坐标原点
	LINEMODE m_eLineMode; //画线模式
	D3DCOLOR	m_dwColor,m_dwBkColor; //前景色，背景色
	bool	m_bZEnable;
    D3DXMATRIX  m_matWorld;

	DWORD m_eSrcBlend, m_eDestBlend; //for alpha blend

	LPDIRECT3DVERTEXBUFFER9 m_pVB;        // VertexBuffer
	int m_nVBSize;
	BYTE* m_pVBData;
	int m_nUsedSize;
		
	DRAWCONTEXT m_DC;

	D3DVIEWPORT9 m_SaveViewport;

    // Stateblocks for setting and restoring render states
    IDirect3DStateBlock9*   m_dwSavedStateBlock;
    IDirect3DStateBlock9*   m_dwDrawStateBlock;

	Vec3 PointsBuffer[POINTSBUF_SIZE];
	int	m_nPointsCount;
	Vec3 SegmentsBuffer[POINTSBUF_SIZE*2];
	int m_nSegmentsCount;

	bool CheckVBSize(int nVBSize);
	BYTE* GetVBBuffer(int nDataSize);
	void SetVertexShader(DWORD dwShader,UINT Stride,D3DPRIMITIVETYPE PrimitiveType,bool bWireFrame=false);
	void DrawPrimitive(UINT PrimitiveCount);
public:
	CDraw();
	~CDraw();
	Vec2		GetOrgPosition() { return m_OrgPosition; } //取坐标原点
	void		SetOrgPosition(const Vec2& Position2) { m_OrgPosition=Position2; } //设坐标原点
	void		SetOrgPosition(float x,float y) { SetOrgPosition( Vec2(x,y) ); } //设坐标原点
	void		MoveOrgPosition(float fXOfs,float fYOfs) { m_OrgPosition+=Vec2(fXOfs,fYOfs); } //移动坐标原点
	float		Geo2DeviceX(float fX) { return fX+m_OrgPosition.x; } //几何坐标到设备坐标
	float		Geo2DeviceY(float fY) { return fY+m_OrgPosition.y; } //几何坐标到设备坐标

	bool		Point(const Vec2& Position2); //画点(2d)
	bool		Point(const Vec3& Position3); //画点(3d)

	Vec2		MoveTo(const Vec2& Position2); //指定当前点(2d)
	Vec2		MoveTo(float x,float y) { return MoveTo(Vec2(x,y)); }
	Vec3		MoveTo(const Vec3& Position3); //指定当前点(3d)
	Vec3		MoveTo(float x,float y,float z) { return MoveTo(Vec3(x,y,z)); }
	bool		LineTo(const Vec2& Position2); //画线到指定点(2d)
	bool		LineTo(float x,float y) { return LineTo(Vec2(x,y)); }
	bool		LineTo(const Vec3& Position3); //画线到指定点(3d)
	bool		LineTo(float x,float y,float z) { return LineTo(Vec3(x,y,z)); }

	bool		Triangle(float x,float y,float x1,float y1,float x2,float y2); //画三角形(2d)
	bool		Triangle(const Vec3& v0,const Vec3& v1,const Vec3& v2); //画三角形(3d)
	bool		FillTriangle(float x,float y,float x1,float y1,float x2,float y2); //填充三角形(2d)
	bool		FillTriangle(const Vec3& v0,const Vec3& v1,const Vec3& v2); //填充三角形(3d)

	bool		Rect(const Vec4& rect); //画矩形
	bool		Rect(const RECT& rect) { return Rect(Vec4((float)rect.left,(float)rect.top,(float)rect.right,(float)rect.bottom)); }
	bool		FillRect(const Vec4& rect); //填充矩形
	bool		FillRect(const RECT& rect) { return FillRect(Vec4((float)rect.left,(float)rect.top,(float)rect.right,(float)rect.bottom)); }

	bool		Circularity(const Vec2& vCenter,float fRadius,int nSegment=16);	//画圆(2d)
	bool		Circularity(const Vec3& vCenter,const Vec3& vDir,float fRadius,int nSegment=16,bool bOldMode=false); //画圆(3d)
	bool		FillCircularity(const Vec2& vCenter,float fRadius,int nSegment=16); //填充圆(2d)
	bool		FillCircularity(const Vec3& vCenter,const Vec3& vDir,float fRadius,int nSegment=16,bool bOldMode=false); //填充圆(3d)
	bool		Box(const Vec3& vMin,const Vec3& vMax); //画盒子
	bool		Cylinder(const Vec3& vCenter,const Vec3& vDir,float fHeight,float fRadius1,float fRadius2,int nSegment=16,bool bOldMode=false); //画圆柱
	bool		Blt(int x,int y,LPDIRECT3DTEXTURE9 pTexture); //画纹理矩形
	bool		StretchBlt(const Vec4& rect,LPDIRECT3DTEXTURE9 pTexture, Vec4* psrcrect=NULL); //画纹理矩形(拉伸)
	bool		StretchBlt(const RECT& rect,LPDIRECT3DTEXTURE9 pTexture) { return StretchBlt(Vec4((float)rect.left,(float)rect.top,(float)rect.right,(float)rect.bottom),pTexture); }
	bool		StretchBlt(const RECT& rect,LPDIRECT3DTEXTURE9 pTexture, const RECT& srcrect) { return StretchBlt(Vec4((float)rect.left,(float)rect.top,(float)rect.right,(float)rect.bottom),pTexture, &Vec4((float)srcrect.left,(float)srcrect.top,(float)srcrect.right,(float)srcrect.bottom) );  }
	bool		StretchBlt3D(const Vec3& org, const Vec3& width, const Vec3& height, LPDIRECT3DTEXTURE9 pTexture, Vec4* psrcrect = NULL); //画纹理矩形(拉伸)

	DWORD		SetColor(D3DCOLOR dwColor); //设置前景色
	DWORD		SetBkColor(D3DCOLOR dwBkColor); //设置背景色

	DWORD	GetSrcBlend() { return m_eSrcBlend; }
	DWORD	GetDestBlend() { return m_eDestBlend; } //for alpha blend
	void		SetBlendMode(DWORD srcBlend, DWORD destBlend) { m_eSrcBlend=srcBlend; m_eDestBlend=destBlend; }

	bool		SetViewport(RECT vrect); //设置视区
	bool		RestoreViewport(); //恢复视区
	bool		SetLineMode(LINEMODE lm); //设置画线模式
	LINEMODE	GetLineMode() { return m_eLineMode;} //取画线模式

	D3DXMATRIX SetMatWorld(const D3DXMATRIX& mat);
	bool SetZEnable(bool bZEnable);
	LPDIRECT3DSURFACE9 GetRenderTarget(int nRTIndex=0);
	bool SetRenderTarget(LPDIRECT3DSURFACE9 pSurf,int nRTIndex=0);

	bool ClearFloatRenderTarget(int width, int height); // fill float render target with 0


	void Flush();

	bool TextOut( int x, int y, TCHAR* szText, D3DCOLOR color=0xffffffff, float fWidthScale=1.0f, float fHeightScale=1.0f );
	bool TextOut3D(const Vec3& vOrg, const Vec3& vWidthDir, const Vec3& vHeightDir, TCHAR* szText);

	SIZE GetTextExtent(TCHAR* szText) const;
public:
	CD3DFont*     m_pFont;              // Font for drawing text
//	CDMText		m_cText;

	//有关d3d device
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ); //d3d设备相关初始化
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
};

CDraw& GetGCDraw();
#define g_Draw GetGCDraw()

#endif