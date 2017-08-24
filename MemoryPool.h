#ifndef __MEMORY_POOL__
#define __MEMORY_POOL__
#include <vector>
#include <queue>
#include <stdlib.h>
#include "Lock.h"
#include "CircularBuffer.h"
#include "UnknownEx.h"

#define MEMORY_INTERFACE public: \
	UNKNOWNEX_INTERFACE(CCircularMemory)

#define MEMORY_INTERFACE_DEBUG public: \
	UNKNOWNEX_INTERFACE_DEBUG(CCircularMemory)

class IMemoryPool
{
public:
	virtual void Free(void* pMemory) = 0;
};

template<typename T>
class CMemoryPool : public IMemoryPool
{
private:
	size_t m_MemoryBlockSize;

	typedef std::vector<void*> _MEMORYS;
	_MEMORYS m_MemoryArray;
	size_t m_MemorySize;
	size_t m_Current;
	void* m_CurrentMemory;

	CCircularBuffer m_FreeMemorys;
	CLock m_Lock;
protected:
	void AllocMemory()
	{
		m_CurrentMemory = 0;
		m_MemorySize = 0;
		m_Current = 0;
		void* p = malloc(m_MemoryBlockSize);
		if (p)
		{
			m_MemoryArray.push_back(p);
			m_MemorySize = m_MemoryBlockSize;
			m_Current = 0;
			m_CurrentMemory = p;
		}
	}

	T* AllocObject()
	{
		T* p = NULL;
		m_Lock.Lock();
		if (m_FreeMemorys.GetCount())
		{
			m_FreeMemorys.ReadString((char*)&p,sizeof(p));
		}
		else
		{
			if((m_Current+sizeof(T))>m_MemorySize)
			{
				//printf("AllocMemory\n");
				AllocMemory();
			}
			if ((m_Current+sizeof(T))<=m_MemorySize)
			{
				p = (T*)((char*)m_CurrentMemory+m_Current);
				m_Current+=sizeof(T);
			}
		}
		m_Lock.Unlock();
		return p;
	}
public:
	CMemoryPool(size_t MemoryBlockSize=1024*1024 )
		:m_MemoryBlockSize(MemoryBlockSize),m_MemorySize(0),m_Current(0),m_CurrentMemory(0)
	{
	}

	virtual ~CMemoryPool()
	{
		for (auto iter : m_MemoryArray)
		{
			free(iter);
		}
	}

	//申请内存
	T* Alloc()
	{
		T* p = AllocObject();
		if (p)
		{
			new(p)T;
			p->SetMemoryPool(this);
		}

		return p;
	}

	template<typename P1>
	T* Alloc(P1 p1)
	{
		T* p = AllocObject();
		if (p)
		{
			new(p)T(p1);
			p->SetMemoryPool(this);
		}

		return p;
	}

	template<typename P1,typename P2>
	T* Alloc(P1 p1,P2 p2)
	{
		T* p = AllocObject();
		if (p)
		{
			//初始化
			new(p)T(p1, p2);
			p->SetMemoryPool(this);
		}

		return p;
	}

	template<typename P1,typename P2,typename P3>
	T* Alloc(P1 p1,P2 p2,P3 p3)
	{
		T* p = AllocObject();
		if (p)
		{
			//初始化
			new(p)T(p1, p2, p3);
			p->SetMemoryPool(this);
		}

		return p;
	}

	//申请内存
// 	T* Alloc(size_t size)
// 	{
// 		T* p = NULL;
// 		m_Lock.Lock();
// 		if (m_FreeMemorys.GetCount())
// 		{
// 			m_FreeMemorys.Read((char*)&p,sizeof(p));
// 			m_FreeMemorys.HeadIncrease(sizeof(p));
// 		}
// 		else
// 		{
// 			if((m_Current+size)>m_MemorySize)
// 			{
// 				AllocMemory();
// 			}
// 			if ((m_Current+size)<=m_MemorySize)
// 			{
// 				p = (T*)((char*)m_CurrentMemory+m_Current);
// 				m_Current+=size;
// 			}
// 		}
// 		m_Lock.Unlock();
// 		return p;
// 	}

	void Free(void* pMemory)
	{
		((T*)pMemory)->~T();
		m_Lock.Lock();
		m_FreeMemorys.Write((const char*)&pMemory,sizeof(pMemory));
		m_Lock.Unlock();
	}
};

// {88C761EE-5E6F-4210-8497-E227EAAE868F}
static const GUID GUID_OF(CCircularMemory) = { 0x88c761ee, 0x5e6f, 0x4210, { 0x84, 0x97, 0xe2, 0x27, 0xea, 0xae, 0x86, 0x8f } };
class CCircularMemory : public CUnknownEx
{
protected:
	IMemoryPool* m_pMemoryPool;
public:
	CCircularMemory()
		:m_pMemoryPool(0)
	{

	}

	virtual ~CCircularMemory()
	{
		m_pMemoryPool = 0;
	}

	__Addref
	{
		long lResult = _SYNC_ADD(&m_dwRef);
// 		if (lResult > 40)
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
			if (m_pMemoryPool)
			{
				m_pMemoryPool->Free(this);
			}
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
		if (lResult <= 0)
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
			if(m_pMemoryPool)
			{
				m_pMemoryPool->Free(this);
			}
		}
		return lResult;
	}

	virtual bool QueryInterface( const GUID& guid, void **ppvObject)
	{
		QUERYINTERFACE(CCircularMemory);
		IF_TRUE(QUERYINTERFACE_PARENT(CUnknownEx));

		return false;
	}

	virtual void SetMemoryPool(IMemoryPool* p)
	{
		m_pMemoryPool = p;
	}
};

	


#endif