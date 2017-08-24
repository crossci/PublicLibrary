#ifndef __LOCK__
#define __LOCK__

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#define _SYNC_ADD(p) InterlockedIncrement(p)
	#define _SYNC_DEC(p) InterlockedDecrement(p)
#else
	#include <pthread.h>
	#define _SYNC_ADD(p) __sync_add_and_fetch(p,1)
	#define _SYNC_DEC(p) __sync_sub_and_fetch(p,1)
#endif // WIN32

class CLock
{
private:
#ifdef WIN32
	CRITICAL_SECTION m_Critical;
#else
	pthread_mutexattr_t lock_attr;
	pthread_mutex_t lock;
#endif
public:
	CLock(void)
	{
#ifdef WIN32
		InitializeCriticalSection( &m_Critical );
#else
		pthread_mutexattr_init(&lock_attr);
		pthread_mutexattr_settype(&lock_attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&lock,&lock_attr);
#endif
	}

	virtual ~CLock(void)
	{
#ifdef WIN32
		DeleteCriticalSection( &m_Critical );
#else
		pthread_mutex_destroy(&lock);
		pthread_mutexattr_destroy(&lock_attr);
#endif
	}
public:
	//Ëø
	void Lock()
	{
#ifdef WIN32
		EnterCriticalSection( &m_Critical );
#else
		pthread_mutex_lock(&lock);
#endif
	}

	//½âËø
	void Unlock()
	{
#ifdef WIN32
		LeaveCriticalSection( &m_Critical );
#else
		pthread_mutex_unlock(&lock);
#endif
	}
};

class CAutoLock
{
private:
	CLock& m_lock;
public:
	CAutoLock(CLock& lock)
		:m_lock(lock)
	{
		m_lock.Lock();
	}
	virtual ~CAutoLock()
	{
		m_lock.Unlock();
	}
};

#define _AUTO_LOCK(x) CAutoLock lock(x)

#endif
