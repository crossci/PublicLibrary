#pragma once
#include "UnknownEx.h"
#include "log.h"
#include <vector>
#include <algorithm>
#include <cerrno>
#include "Macro.h"

#ifndef WIN32
#include <pthread.h>
#endif

// {3EF9CC35-E566-4EBF-954D-066D9ADEC742}
static const GUID GUID_OF(CThreadManage) = { 0x3ef9cc35, 0xe566, 0x4ebf, { 0x95, 0x4d, 0x6, 0x6d, 0x9a, 0xde, 0xc7, 0x42 } };
class CThreadManage : public CUnknownEx
{
private:
	//线程
#ifdef WIN32
	typedef DWORD _THREAD_ID;
#else
	typedef pthread_t _THREAD_ID;
#endif
	typedef std::vector<_THREAD_ID> _THREAD_IDS;
	_THREAD_IDS m_Threads;
public:
	CThreadManage(void);
	virtual ~CThreadManage(void);

	__QueryInterface;
public:
	//创建线程
	virtual int CreateThread(int WorkerNum);
public:
	//删除线程
	virtual void OnExitCurrentThread(void);
public:
	//检测是否全部退出
    virtual bool IsEmptyThread(void);
public:
	//工作线程
	virtual void DoWorkThread(void) = 0;
};

inline CThreadManage::CThreadManage(void)
{
}

inline CThreadManage::~CThreadManage(void)
{
}

inline bool CThreadManage::QueryInterface( const GUID& guid, void **ppvObject)
{
	QUERYINTERFACE(CThreadManage);
	IF_TRUE(QUERYINTERFACE_PARENT(CUnknownEx));

	return false;
}

//线程
#ifdef WIN32
inline DWORD WINAPI _thread(LPVOID arg)
#else
inline void* _thread(void *arg)
#endif
{
	srand((unsigned int)time(0));
	//
	CThreadManage* pThreadManage = (CThreadManage*)arg;
	if (pThreadManage)
	{
		pThreadManage->DoWorkThread();
	}
	else
	{
		GetLogger()->log(_LOG_ERROR,"_thread arg is null");
	}
	pThreadManage->OnExitCurrentThread();
	return 0;
}

inline int CThreadManage::CreateThread(int WorkerNum)
{
	//创建成功数
	int ThreadNum = 0;
	_THREAD_ID thrd;
	//创建线程
	for(int i=0;i<WorkerNum;i++)
	{
		AddRef();
#ifdef WIN32
		::CreateThread(0,0,_thread,this,0,&thrd);
        if (thrd>0)
#else
		int tid = 0;
		tid = pthread_create(&thrd,0,_thread,this);
        if (tid==0)
#endif
		{
			//累计创建成功数
			ThreadNum++;
			//添加工作线程
			m_Threads.push_back(thrd);
		}
		else
		{
			GetLogger()->log(_LOG_ERROR,"CreateThread failed(%d).",ERRNO);
		}
	}
	//返回成功数
	return ThreadNum;
}

inline bool CThreadManage::IsEmptyThread(void)
{
	return m_Threads.empty();
}

//删除线程
inline void CThreadManage::OnExitCurrentThread(void)
{
#ifdef WIN32
	_THREAD_IDS::iterator iter = find(m_Threads.begin(),m_Threads.end(),GetCurrentThreadId());
#else
	_THREAD_IDS::iterator iter = find(m_Threads.begin(),m_Threads.end(),pthread_self());
#endif
	if (iter!=m_Threads.end())
	{
		m_Threads.erase(iter);
	}
}

