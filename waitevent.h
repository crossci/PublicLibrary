#ifndef __WAIT_EVENT__
#define __WAIT_EVENT__
#include "UnknownEx.h"
#include <string>
#include "log.h"

#ifdef WIN32
#include <windows.h>
#else
#include <semaphore.h>
#include <sys/time.h>
#endif

#if !defined(ANDROID)
#define _OPEN_SEM
#endif


// {1B194FB3-6D9B-4625-B10D-66E042B9A488}
static const GUID GUID_OF(CWaitEvent) = { 0x1b194fb3, 0x6d9b, 0x4625, { 0xb1, 0xd, 0x66, 0xe0, 0x42, 0xb9, 0xa4, 0x88 } };
class CWaitEvent : public CUnknownEx
{
private:
#ifdef WIN32
	HANDLE m_handle;
#else
	sem_t m_sem;
	sem_t* m_psem;
    std::string m_name;
    pthread_cond_t m_handle;
    pthread_mutex_t mtx;
#endif
public:
    CWaitEvent(const char* Name);
	virtual ~CWaitEvent(void);

	__QueryInterface;
public:
	//�ȴ�
	int Wait(int dwMilliseconds);
	//
	int Post();
};

inline CWaitEvent::CWaitEvent(const char* name)
{
#ifdef WIN32
	m_handle = CreateEvent(NULL,false,false,NULL);
	if (m_handle==NULL)
	{
		GetLogger()->log(_LOG_FATAL,"sem_init failed\n");
	}
#else
	//
	m_name = name;
	if (m_name.empty()) {
		pthread_mutex_init(&mtx,NULL);
		pthread_cond_init(&m_handle,NULL);
	}
	else
	{
#ifdef _OPEN_SEM
		m_psem = sem_open(name, O_CREAT, 0644, 0);

#else
		m_psem = NULL;

		if(sem_init(&m_sem,0,0)==0)
		{
			m_psem = &m_sem;
		}
#endif
		if (m_psem==NULL)
		{
			GetLogger()->log(_LOG_FATAL,"sem_init failed\n");
		}
	}
#endif
}

inline CWaitEvent::~CWaitEvent(void)
{
#ifdef WIN32
	if (m_handle)
	{
		CloseHandle(m_handle);
		m_handle = NULL;
	}
#else
	if (m_name.empty()) {
		pthread_cond_destroy(&m_handle);
		pthread_mutex_destroy(&mtx);
	}
	else
	{
#ifdef _OPEN_SEM
		sem_unlink(m_name.c_str());
#else
		sem_destroy(m_psem);
#endif
	}
#endif
}

inline bool CWaitEvent::QueryInterface( const GUID& guid, void **ppvObject)
{
	QUERYINTERFACE(CWaitEvent);
	IF_TRUE(QUERYINTERFACE_PARENT(CUnknownEx))

	return false;
}

inline int CWaitEvent::Wait(int dwMilliseconds)
{
#ifdef WIN32
	WaitForSingleObject(m_handle,dwMilliseconds);
#else
	struct timespec ts;
	struct timeval now;
	gettimeofday(&now, NULL);
	ts.tv_sec = now.tv_sec + dwMilliseconds/1000;
	ts.tv_nsec = now.tv_usec + (dwMilliseconds % 1000) * 1000;
	ts.tv_sec += ts.tv_nsec / 1000000;
	ts.tv_nsec = ts.tv_nsec % 1000000;
	if (m_name.empty()) {
		pthread_cond_timedwait(&m_handle,&mtx,&ts);
	}
	else
	{
#ifdef _OPEN_SEM
		sem_wait(m_psem);
#else
		if (m_psem)
		{
			sem_timedwait(m_psem,&ts);
		}
#endif
	} 
#endif
	return 0;
}

inline int CWaitEvent::Post()
{
#ifdef WIN32
	if (m_handle!=NULL)
	{
		SetEvent(m_handle);
	}
#else
	if (m_name.empty()) {
		pthread_cond_signal(&m_handle);
	}
	else{
		sem_post(m_psem);
	}
#endif
	return 0;
}

#endif
