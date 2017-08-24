#ifndef __WEAK_REFERENCE__
#define __WEAK_REFERENCE__

#include "Lock.h"

///////////////////////////////////////////////////////////////////
class IWeakReference
{
	friend class CWeakReferenceList;
public:
	virtual IWeakReference* SetPrevWeak(IWeakReference* pWeakReference) = 0;
	virtual IWeakReference* SetNextWeak(IWeakReference* pWeakReference) = 0;
	virtual void WeakRelease() = 0;
};

class CWeakReferenceList
{
private:
	IWeakReference* m_WeakReferenceList;
public:
	CWeakReferenceList()
		:m_WeakReferenceList(0)
	{

	}
	virtual void WeakRelease()
	{
		LockWeak(true);
		while (m_WeakReferenceList)
		{
			m_WeakReferenceList->WeakRelease();
		}
		LockWeak(false);
	}
	virtual IWeakReference* SetNextWeak(IWeakReference* pWeakReference)
	{
		IWeakReference* p;
		LockWeak(true);
		p = m_WeakReferenceList;
		m_WeakReferenceList = pWeakReference;
		LockWeak(false);
		return p;
	}

	static void LockWeak(bool islock)
	{
		static CLock lock;
		if (islock)
		{
			lock.Lock();
		}
		else
		{
			lock.Unlock();
		}
	}
};

template<typename T>
class CWeakReference : public IWeakReference
{
private:
	T* m_target;
	IWeakReference* m_previous;
	IWeakReference* m_next;
public:
	CWeakReference()
		:m_target(0)
		,m_previous(0)
		,m_next(0)
	{

	}

	virtual ~CWeakReference()
	{
		WeakRelease();
	}
private:
	virtual IWeakReference* SetPrevWeak(IWeakReference* pWeakReference)
	{
		IWeakReference* p = 0;
		CWeakReferenceList::LockWeak(true);
		if (m_target)
		{
			p = m_previous;
			m_previous = pWeakReference;
		}
		CWeakReferenceList::LockWeak(false);
		return p;
	}

	virtual IWeakReference* SetNextWeak(IWeakReference* pWeakReference)
	{
		IWeakReference* p = 0;
		CWeakReferenceList::LockWeak(true);
		if (m_target)
		{
			p = m_next;
			m_next = pWeakReference;
		}
		CWeakReferenceList::LockWeak(false);
		return p;
	}

	virtual IWeakReference* GetNextWeak()
	{
		return m_next;
	}

	virtual void WeakRelease()
	{
		CWeakReferenceList::LockWeak(true);
		if (m_target)
		{
			if (m_previous)
			{
				m_previous->SetNextWeak(m_next);
			}
			else if(m_target)
			{
				m_target->SetNextWeak(m_next);
			}
			if (m_next)
			{
				m_next->SetPrevWeak(m_previous);
			}
			m_target = 0;
			m_previous = 0;
			m_next = 0;
		}
		CWeakReferenceList::LockWeak(false);
	}
public:
	T* Get()
	{
		T* target = 0;
		CWeakReferenceList::LockWeak(true);
		if (m_target)
		{
			target = m_target;
			target->AddRef();
		}
		CWeakReferenceList::LockWeak(false);
		return target;
	}
	void operator= (CWeakReference& value)
	{
		WeakRelease();
		CWeakReferenceList::LockWeak(true);
		m_target = value;
		if (m_target)
		{
			IWeakReference* next = m_target->SetNextWeak(this);
			if (next)
			{
				next->SetPrevWeak(this);
			}
			SetNextWeak(next);
		}
		CWeakReferenceList::LockWeak(false);
	}

	T*& operator = (T*& value)
	{
		WeakRelease();
		CWeakReferenceList::LockWeak(true);
		m_target = value;
		if (m_target)
		{
			IWeakReference* next = m_target->SetNextWeak(this);
			if (next)
			{
				next->SetPrevWeak(this);
			}
			SetNextWeak(next);
		}
		CWeakReferenceList::LockWeak(false);
		return m_target;
	}

	operator T*() 
	{
		return m_target;
	}
};

#endif