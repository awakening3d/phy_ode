//util.h
#ifndef UTIL_H
#define UTIL_H

#include "d3dx9.h"
//vector
typedef D3DXVECTOR2 Vec2;
typedef D3DXVECTOR3 Vec3;
typedef D3DXVECTOR4 Vec4;
typedef D3DXQUATERNION Quaternion;

#ifndef SAFE_DELETE

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#endif


const FLOAT g_PI       =  3.14159265358979323846f; // Pi
const FLOAT g_2_PI     =  6.28318530717958623200f; // 2 * Pi
const FLOAT g_PI_DIV_2 =  1.57079632679489655800f; // Pi / 2
const FLOAT g_PI_DIV_4 =  0.78539816339744827900f; // Pi / 4
const FLOAT g_INV_PI   =  0.31830988618379069122f; // 1 / Pi
const FLOAT g_DEGTORAD =  0.01745329251994329547f; // Degrees to Radians
const FLOAT g_RADTODEG = 57.29577951308232286465f; // Radians to Degrees
const FLOAT g_HUGE     =  1.0e+38f;                // Huge number for FLOAT
const FLOAT g_EPSILON  =  1.0e-5f;                 // Tolerance for FLOATs


#define dmRand()		(((FLOAT)rand() ) / RAND_MAX)
#define dmMin(a,b)		( (a)<(b)?(a):(b) )
#define dmMax(a,b)		( (a)>(b)?(a):(b) )
#define dmAbs(x)		( (x)>0?(x):-(x)  )

// Fuzzy compares (within tolerance)
//-----------------------------------------------------------------------------
inline bool dmIsZero( FLOAT a)
{ return ( a <= 0.0f ) ? ( a >= -g_EPSILON ) : ( a <= g_EPSILON ); }

inline bool dmIsZero( FLOAT a, FLOAT fTol )
{ return ( a <= 0.0f ) ? ( a >= -fTol ) : ( a <= fTol ); }

inline bool dmIsVectorZero(const Vec3& vVec)
{
	return ( dmIsZero(vVec.x) && dmIsZero(vVec.y) && dmIsZero(vVec.z) );
}

inline bool dmIsEqual(FLOAT a,FLOAT b)
{
	return dmIsZero(a-b);
}

inline int dmRound(FLOAT f)
{
	if (f>0)
		return (int)(f+.5f);
	else
		return (int)(f-.5f);
}

// This code supposedly originates from Id-software
// It computes a fast 1 / sqrtf(v) approximation
inline float fastInvSqrt(float x)
{
        float xhalf = 0.5f*x;
        int i = *(int*)&x;
        i = 0x5f3759df - (i >> 1); // 计算第一个近似根 //0x5f375a86
        x = *(float*)&i;
        x = x*(1.5f - xhalf*x*x); // 牛顿迭代法
        return x;
}

float fastSin(UINT degree);
float fastCos(UINT degree);


DWORD GDI_D3D_ColorConver(DWORD dwColor);
void DWORDtoColorValue(DWORD dwColor,D3DCOLORVALUE& cv);
DWORD ColorValuetoDWORD(const D3DCOLORVALUE& cv);
DWORD ReverseColor(const DWORD dwColor,bool bKeepAlpha=false);



//-------------------------------------------------------
//Ansi to Unicode, nBufferLen返回转换后字符串长度
bool AnsiToUnicode(LPCSTR szAnsi, size_t ansiStringLen, wchar_t* wszString,int& nBufferLen);
//Unicode to Ansi, nBufferLen返回转换后字符串长度
bool UnicodeToAnsi(const wchar_t* wszString, size_t wszStringLen, char* szAnsi, int& nBufferLen);

#endif