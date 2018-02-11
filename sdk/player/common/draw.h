//Draw.h ��ͼ��

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
	Vec2 m_CurrentPosition2; //��ǰ��(2d)
	Vec3 m_CurrentPosition3; //��ǰ��(3d)
	Vec2	m_OrgPosition; //����ԭ��
	LINEMODE m_eLineMode; //����ģʽ
	D3DCOLOR	m_dwColor,m_dwBkColor; //ǰ��ɫ������ɫ
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
	Vec2		GetOrgPosition() { return m_OrgPosition; } //ȡ����ԭ��
	void		SetOrgPosition(const Vec2& Position2) { m_OrgPosition=Position2; } //������ԭ��
	void		SetOrgPosition(float x,float y) { SetOrgPosition( Vec2(x,y) ); } //������ԭ��
	void		MoveOrgPosition(float fXOfs,float fYOfs) { m_OrgPosition+=Vec2(fXOfs,fYOfs); } //�ƶ�����ԭ��
	float		Geo2DeviceX(float fX) { return fX+m_OrgPosition.x; } //�������굽�豸����
	float		Geo2DeviceY(float fY) { return fY+m_OrgPosition.y; } //�������굽�豸����

	bool		Point(const Vec2& Position2); //����(2d)
	bool		Point(const Vec3& Position3); //����(3d)

	Vec2		MoveTo(const Vec2& Position2); //ָ����ǰ��(2d)
	Vec2		MoveTo(float x,float y) { return MoveTo(Vec2(x,y)); }
	Vec3		MoveTo(const Vec3& Position3); //ָ����ǰ��(3d)
	Vec3		MoveTo(float x,float y,float z) { return MoveTo(Vec3(x,y,z)); }
	bool		LineTo(const Vec2& Position2); //���ߵ�ָ����(2d)
	bool		LineTo(float x,float y) { return LineTo(Vec2(x,y)); }
	bool		LineTo(const Vec3& Position3); //���ߵ�ָ����(3d)
	bool		LineTo(float x,float y,float z) { return LineTo(Vec3(x,y,z)); }

	bool		Triangle(float x,float y,float x1,float y1,float x2,float y2); //��������(2d)
	bool		Triangle(const Vec3& v0,const Vec3& v1,const Vec3& v2); //��������(3d)
	bool		FillTriangle(float x,float y,float x1,float y1,float x2,float y2); //���������(2d)
	bool		FillTriangle(const Vec3& v0,const Vec3& v1,const Vec3& v2); //���������(3d)

	bool		Rect(const Vec4& rect); //������
	bool		Rect(const RECT& rect) { return Rect(Vec4((float)rect.left,(float)rect.top,(float)rect.right,(float)rect.bottom)); }
	bool		FillRect(const Vec4& rect); //������
	bool		FillRect(const RECT& rect) { return FillRect(Vec4((float)rect.left,(float)rect.top,(float)rect.right,(float)rect.bottom)); }

	bool		Circularity(const Vec2& vCenter,float fRadius,int nSegment=16);	//��Բ(2d)
	bool		Circularity(const Vec3& vCenter,const Vec3& vDir,float fRadius,int nSegment=16,bool bOldMode=false); //��Բ(3d)
	bool		FillCircularity(const Vec2& vCenter,float fRadius,int nSegment=16); //���Բ(2d)
	bool		FillCircularity(const Vec3& vCenter,const Vec3& vDir,float fRadius,int nSegment=16,bool bOldMode=false); //���Բ(3d)
	bool		Box(const Vec3& vMin,const Vec3& vMax); //������
	bool		Cylinder(const Vec3& vCenter,const Vec3& vDir,float fHeight,float fRadius1,float fRadius2,int nSegment=16,bool bOldMode=false); //��Բ��
	bool		Blt(int x,int y,LPDIRECT3DTEXTURE9 pTexture); //���������
	bool		StretchBlt(const Vec4& rect,LPDIRECT3DTEXTURE9 pTexture, Vec4* psrcrect=NULL); //���������(����)
	bool		StretchBlt(const RECT& rect,LPDIRECT3DTEXTURE9 pTexture) { return StretchBlt(Vec4((float)rect.left,(float)rect.top,(float)rect.right,(float)rect.bottom),pTexture); }
	bool		StretchBlt(const RECT& rect,LPDIRECT3DTEXTURE9 pTexture, const RECT& srcrect) { return StretchBlt(Vec4((float)rect.left,(float)rect.top,(float)rect.right,(float)rect.bottom),pTexture, &Vec4((float)srcrect.left,(float)srcrect.top,(float)srcrect.right,(float)srcrect.bottom) );  }
	bool		StretchBlt3D(const Vec3& org, const Vec3& width, const Vec3& height, LPDIRECT3DTEXTURE9 pTexture, Vec4* psrcrect = NULL); //���������(����)

	DWORD		SetColor(D3DCOLOR dwColor); //����ǰ��ɫ
	DWORD		SetBkColor(D3DCOLOR dwBkColor); //���ñ���ɫ

	DWORD	GetSrcBlend() { return m_eSrcBlend; }
	DWORD	GetDestBlend() { return m_eDestBlend; } //for alpha blend
	void		SetBlendMode(DWORD srcBlend, DWORD destBlend) { m_eSrcBlend=srcBlend; m_eDestBlend=destBlend; }

	bool		SetViewport(RECT vrect); //��������
	bool		RestoreViewport(); //�ָ�����
	bool		SetLineMode(LINEMODE lm); //���û���ģʽ
	LINEMODE	GetLineMode() { return m_eLineMode;} //ȡ����ģʽ

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

	//�й�d3d device
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ); //d3d�豸��س�ʼ��
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
};

CDraw& GetGCDraw();
#define g_Draw GetGCDraw()

#endif