
#ifndef _winds3d_component_h_
#define _winds3d_component_h_

#include "common.h"

class IApp;

typedef enum { COM_UNKNOWN, COM_D3D } ComponentType;

// Component Base Class
class IComponent
{
public:
	IComponent() 	// constructor
	:	m_nReferenceCount( 1 )
	{}

	void AddRef() { // ++ reference count
		m_nReferenceCount++;
	}

	int GetRefCount() { // get reference count
		return m_nReferenceCount;
	}

	virtual void Release() { // -- reference count
		m_nReferenceCount--;
		if ( m_nReferenceCount <= 0 ) delete this;
	}

	virtual ComponentType GetType() {// get component type
		return COM_UNKNOWN;
	}

	virtual bool OneTimeInit(IApp* pApp) { return true; }		// will be called when load component
	virtual bool FinalCleanup() { return true; }	// will be called when unload component

protected:
	virtual ~IComponent() {} // destructor

private:
	int		m_nReferenceCount;

}; //class IComponent



// Component Information
struct ComponentInfo {
	const char*		szName;
	IComponent*		(*Factory)();
	void*			hModule; // handle of dll module
}; //struct ComponentInfo



// I: class identifier C: component name
// for example: IMPLEMENT_COMPONENT( CTest, test )
#define IMPLEMENT_COMPONENT( I, C )		\
	static IComponent* Component_Factory_##C()	{ \
		return new I;	\
	}	\
	ComponentInfo Component_##C =	{ \
		#C,	\
		Component_Factory_##C	\
	};


// I: class idenifier
// for example: SINGLETON_COMPONENT( CTest )
#define SINGLETON_COMPONENT( I )	\
	private:						\
		static I* ms_pSingleton;	\
	public:							\
		static I* GetSingleton() {	\
			return ms_pSingleton;	\
		}							\
		void SetSingleton()	{		\
			ms_pSingleton = this;	\
		}							\
		void Release()	{			\
			ms_pSingleton = NULL;	\
			IComponent::Release();	\
		}


// I: class identifier C: component name
// for example: IMPLEMENT_SINGLETON_COMPONENT( CTest, test )
#define IMPLEMENT_SINGLETON_COMPONENT( I, C )	\
	I*	I::ms_pSingleton = NULL;				\
	static IComponent* Component_Factory_##C()	{ \
		if( I::GetSingleton() )	{		\
			I::GetSingleton()->AddRef();	\
			return I::GetSingleton();	\
		}	\
		I* pCom = new I;		\
		Assert( pCom );			\
		pCom->SetSingleton();	\
		return pCom;			\
	}	\
	ComponentInfo Component_##C =	{ \
		#C,	\
		Component_Factory_##C	\
	};



#define DECLARE_COMPONENT( C )	extern ComponentInfo	Component_##C;


#define BEGIN_PACKAGE_LIST	\
	extern "C" DLL_EXPORT ComponentInfo**	Factory() {	\
		static ComponentInfo*	apComponentInfo[] =	{ \

#define COMPONENT( C )		&Component_##C,

#define END_PACKAGE_LIST	\
			0	\
		};		\
		return apComponentInfo;	\
	}


// ------- for using component --------
bool LoadComponentPackage( const char* szFileName );
bool UnloadComponentPackage( const char* szFileName );


#endif
