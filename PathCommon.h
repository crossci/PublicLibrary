#pragma once

#include "shlwapi.h"
#include "Macro.h"

#pragma comment(lib,"shlwapi.lib")

//得到程序路径
inline void GetProgPath( LPTSTR pPath, LPCTSTR ExtFileName=NULL )
{
	TCHAR Buf[MAX_PATH];
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR];
	ZERO_MEMORYQ(Buf);
	GetModuleFileName( NULL, Buf, MAX_PATH );

#pragma warning(disable : 4996)
#ifdef _UNICODE
	_wsplitpath(Buf,drive,dir,NULL,NULL);
#else
	_splitpath(Buf,drive,dir,NULL,NULL);
#endif
#pragma warning(disable:  )
	if( pPath )
	{
		wsprintf( pPath, _T("%s%s%s"), drive, dir, ExtFileName );
	}
}

//得到程序路径
inline void GetPath( LPCTSTR FileName, LPTSTR pPath, LPCTSTR ExtFileName=NULL )
{
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR];

#pragma warning(disable : 4996)
#ifdef _UNICODE
	_wsplitpath(FileName,drive,dir,NULL,NULL);
#else
	_splitpath(FileName,drive,dir,NULL,NULL);
#endif
#pragma warning(disable:  )
	if( pPath )
	{
		wsprintf( pPath, _T("%s%s%s"), drive, dir, ExtFileName );
	}
}

inline void SetPath( LPTSTR Buf, LPCTSTR Path, LPCTSTR File, char c='\\' )
{
	TCHAR t[2] = {0};
	while(File[0]==c)
	{
		File++;
	}
#ifdef _UNICODE
	if( (Path[wcslen(Path)-1]!=c)&&(File[0]!=c) )
#else
	if( (Path[strlen(Path)-1]!=c)&&(File[0]!=c) )
#endif
	{
		t[0] = c;
	}
	wsprintf( Buf, _T("%s%s%s"), Path, t, File );
}

inline void CreateDir(LPCTSTR p,BOOL IsPath=false)
{
	TCHAR tmp[MAX_PATH];
	TCHAR* s = (TCHAR*)p;
	s = StrChr( s, '\\' );
	while(s!=NULL)
	{
		ZERO_MEMORYQ( tmp );
		memcpy( tmp, p, (UINT64)s-(UINT64)p );
		if (!PathIsDirectory(tmp))
		{
			if(! CreateDirectory( tmp, NULL ) )
			{
				break;
			}
		}
		s++;
		s = StrChr( s, '\\' );
	}
	if (IsPath)
	{
		if (!PathIsDirectory(p))
		{
			if(! CreateDirectory( p, NULL ) )
			{

			}
		}
	}
}
