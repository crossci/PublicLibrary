#ifndef __GUID__
#define __GUID__

#include <string.h>

#ifndef WIN32
typedef struct {
	unsigned int  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[ 8 ];
} GUID;

inline bool InlineIsEqualGUID(const GUID& g1,const GUID& g2)
{
	if(memcmp(&g1,&g2,sizeof(GUID))==0)
	{
		return true;
	}
	return false;
}
#endif

#endif