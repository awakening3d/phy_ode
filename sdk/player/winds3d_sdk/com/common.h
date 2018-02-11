#pragma warning( disable : 4786 )


#include <windows.h>


#define DLL_IMPORT	__declspec( dllimport )
#define DLL_EXPORT	__declspec( dllexport )



#ifdef _DEBUG

#define Assert( exp )			\
	{							\
		if( !(exp) )			\
		{						\
			__asm	int		3h	\
		}						\
	}

#else

#define Assert( exp )

#endif // #ifdef _DEBUG