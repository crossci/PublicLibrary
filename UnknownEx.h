#ifndef __UNKNOWNEX__
#define __UNKNOWNEX__

#include "guid.h"
#include "Lock.h"
#include "WeakReference.h"
#include "assert.h"
#include <sstream>

#ifdef _DEBUG
#include "../DebugLog/DebugLog.h"
#endif

//UNKNOWNEX接口
//安全释放
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)	{ if(p) { (p)->Release(); (p)=NULL; } }
#endif

#define __Addref virtual int AddRef(void)
#define __Release virtual int Release(void)
#define __QueryInterface virtual bool QueryInterface( const GUID& guid, void **ppvObject)

#define GUID_OF(c) GUID_##c
#define QUERYINTERFACE(c) QUERYINTERFACE_GUID(GUID_OF(c),c,this)
#define QUERYINTERFACE_GUID(Guid,GuidClass,Object) if (InlineIsEqualGUID(guid, Guid)){ *ppvObject = static_cast<GuidClass*>(Object); if(*ppvObject){Object->AddRef();return true;}return false; }

#define IF_TRUE(pc) if(pc)return true;
#define QUERYINTERFACE_PARENT(c) c::QueryInterface(guid,ppvObject)

// {CA140B7E-DB1B-4CD8-B5F3-439CDEB13CC8}
static const GUID GUID_OF(IUnknownEx) = { 0xca140b7e, 0xdb1b, 0x4cd8, { 0xb5, 0xf3, 0x43, 0x9c, 0xde, 0xb1, 0x3c, 0xc8 } };
class IUnknownEx
{
public:
	__Addref = 0;
	__Release = 0;
	__QueryInterface = 0;
};

// {DB2E0432-7D8A-4DFC-BAE0-448D1E9C69DD}
static const GUID GUID_OF(CUnknownEx) = { 0xdb2e0432, 0x7d8a, 0x4dfc, { 0xba, 0xe0, 0x44, 0x8d, 0x1e, 0x9c, 0x69, 0xdd } };
class CUnknownEx : public IUnknownEx//, public CWeakReferenceList
{
protected:
	long m_dwRef;
public:
	CUnknownEx()
		:m_dwRef(1)
	{

	}
	virtual ~CUnknownEx()
	{

	}
	__Addref
	{
		long lResult = _SYNC_ADD(&m_dwRef);
// 		if (lResult >40)
// 		{
// 			_asm
// 			{
// 				int 3;
// 			}
// 		}

		return lResult;
	}
	__Release
	{
		int lResult = _SYNC_DEC(&m_dwRef);
// 		if (lResult < 0)
// 		{
// 			_asm
// 			{
// 				int 3;
// 			}
// 		}
		if (lResult == 0)
		{
			//WeakRelease();
			delete this;
		}
		return lResult;
	}
	virtual int AddRef_Debug(void)
	{
		long lResult = _SYNC_ADD(&m_dwRef);
		//assert(lResult < 20);
#ifdef _DEBUG
// 		std::stringstream log;
// 		log << this << "\tAddRef()\t" << lResult;
// 		DebugLog::LogStack(log.str().c_str());
#endif
		return lResult;
	}
	virtual int Release_Debug(void)
	{
		int lResult = _SYNC_DEC(&m_dwRef);
#ifdef _DEBUG
		if (lResult<=0)
		{
			std::stringstream log;
			log << this << "\tRelease()\t" << lResult;
			DebugLog::LogStack(log.str().c_str(), lResult < 0);
		}
#endif
		if (lResult < 0)
		{
			_asm
			{
				int 3;
			}
		}
		if (lResult == 0)
		{
			//WeakRelease();
			delete this;
		}
		return lResult;
	}
	__QueryInterface
	{
		QUERYINTERFACE(CUnknownEx);
		QUERYINTERFACE(IUnknownEx);

		return false;
	}
};

#define UNKNOWNEX_INTERFACE(x) \
	__Addref{return x::AddRef();} \
	__Release{return x::Release();} \
	__QueryInterface;

#define UNKNOWNEX_INTERFACE_DEBUG(x) \
	__Addref{return x::AddRef_Debug();} \
	__Release{return x::Release_Debug();} \
	__QueryInterface;

#define IF_QUERYINTERFACE(source,dest,c) \
	if(!source->QueryInterface(GUID_OF(c),(void**)&dest)){ \
		GetLogger()->log(_LOG_WARN,"QueryInterface FAILED:%s\n",#c); \
	}else

#endif
