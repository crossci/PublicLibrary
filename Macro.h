#pragma once
#include "string"
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef WIN32
#include <io.h>
#include <direct.h>
//windows
#pragma warning(disable:4996)
#define ERRNO GetLastError()
#define snprintf _snprintf
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
//linux
#define HANDLE	int
#define SOCKET	int
#define INVALID_HANDLE_VALUE		-1
#define ERRNO errno
#endif


#define SUB(x) GetLogger()->log(_LOG_NOTICE,x"\n");

#define ZERO_MEMORYQ(x) memset(&x,0,sizeof(x))
#define ZERO_MEMORY(x,s) memset(x,0,s)
#define CHECK_LEN(x,len) ((x>0)&&(x<len))
#define CHECK_LEN_RANGE(x,llen,hlen) ((x>=llen)&&(x<=hlen))
#define PRINT_DEBUG(str) GetLogger()->log(_LOG_DEBUG,"\n\t%s(%d)\n\t%s",__FILE__,__LINE__,str);
#define RAND_ROUND(min_v,max_v) (((max_v - min_v) > 0)?(min_v + rand() % (max_v - min_v)):(min_v))
#define _max(a,b)    (((a) > (b)) ? (a) : (b))
#define _min(a,b)    (((a) < (b)) ? (a) : (b))

#ifdef WIN32
#define MKDIR(fn) mkdir(fn);
#else
#define MKDIR(fn) mkdir(fn, S_IRWXU | S_IRWXG | S_IRWXO);
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////
//create
#define NEW_CREATE(c) \
	static c* CreateInstance() \
	{ \
	return new c; \
	}

#define NEW_CREATE1(c,P1) \
	static c* CreateInstance(P1 p1) \
	{ \
	return new c(p1); \
	}

#define NEW_CREATE2(c,P1,P2) \
	static c* CreateInstance(P1 p1,P2 p2) \
	{ \
	return new c(p1,p2); \
	}

#define STATIC_CREATE(c) \
	static c* CreateInstance() \
	{ \
		static c* p = NULL; \
		if (!p) \
		{ \
			p = new c; \
		} \
		if (p) \
		{ \
			p->AddRef(); \
		} \
		return p; \
	}

#define POOL_CREATE(c) \
	static c* CreateInstance() \
	{ \
		static CMemoryPool<c> MemoryPool; \
		return MemoryPool.Alloc(); \
	}
#define POOL_CREATE1(c,P1) \
	static c* CreateInstance(P1 p1) \
	{ \
	static CMemoryPool<c> MemoryPool; \
	return MemoryPool.Alloc(p1); \
	}
#define POOL_CREATE2(c,P1,P2) \
	static c* CreateInstance(P1 p1,P2 p2) \
	{ \
	static CMemoryPool<c> MemoryPool; \
	return MemoryPool.Alloc(p1,p2); \
	}
#define POOL_CREATE3(c,P1,P2,P3) \
	static c* CreateInstance(P1 p1,P2 p2,P3 p3) \
	{ \
	static CMemoryPool<c> MemoryPool; \
	return MemoryPool.Alloc(p1,p2,p3); \
	}

//////////////////////////////////////////////////////////////////////////////////////
//quick define
#define DEFINE_VALUE(name,type) \
	protected: \
	type m_##name; \
	public: \
	virtual void Set##name(type n){m_##name=n;}; \
	virtual type Get##name(){return m_##name;};
#define DEFINE_STRING(name) \
	protected: \
	std::string m_##name; \
	public: \
	virtual void Set##name(const char* n){m_##name=n;}; \
	virtual const char* Get##name(){return m_##name.c_str();}; \
	virtual int Get##name##Length(){return (int)m_##name.length();}; \
	virtual bool Comp##name(const char* n){return m_##name==n;}

#define DEFINE_RVALUE(name,type) \
	protected: \
	type m_##name; \
	public: \
	virtual type Get##name(){return m_##name;};
#define DEFINE_RSTRING(name) \
	protected: \
	std::string m_##name; \
	public: \
	virtual const char* Get##name(){return m_##name.c_str();}; \
	virtual int Get##name##Length(){return (int)m_##name.length();};


#define DEFINE_VIRTUAL_FUNCTION(name,type) \
	public: \
	virtual void Set##name(type n) = 0; \
	virtual type Get##name() = 0;

#define DEFINE_VIRTUAL_STR_FUNCTION(name) \
	public: \
	virtual void Set##name(const char* n) = 0; \
	virtual const char* Get##name() = 0;

#define DEFINE_CALL_FUNCTION(name,type,c) \
	public: \
	virtual void Set##name(type n){c::Set##name(n);}; \
	virtual type Get##name(){return c::Get##name();};


/////////////////////////////////////////////////////////////////////////////////
inline void InitSrand()
{
	srand(time(0));
}

inline int snprintf_safe(char * _DstBuf, size_t _MaxCount, const char * _Format, ...)
{
	int ret = -1;
	if (_MaxCount>0)
	{
		va_list vArgList;
		va_start(vArgList, _Format);
		ret = vsnprintf(_DstBuf, _MaxCount - 1, _Format, vArgList);
		if (ret>= 0)
		{
			_DstBuf[ret] = 0;
		}
		else
		{
			_DstBuf[0] = 0;
		}
		va_end(vArgList);
	}

	return ret;
}
