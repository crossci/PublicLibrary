#pragma once
#include <stdlib.h>
#include "Lock.h"
#include "Macro.h"

/////////////////////////////////////////////////////////////////////////
//CIDKey
class CIDKey
{
public:
	CIDKey(void);
	CIDKey(int id);
	virtual ~CIDKey(void);
public:
	DEFINE_VALUE(ID,int);
};

inline CIDKey::CIDKey(void)
	:m_ID(-1)
{

}

inline CIDKey::CIDKey(int id)
	:m_ID(id)
{

}

inline CIDKey::~CIDKey(void)
{

}

////////////////////////////////////////////////////////////////////////////////////////////
//CRandomKey
class CRandomKey
{
private:
	union 
	{
		long long key64;
		struct  
		{
			int key32l;
			int key32h;
		};
	}m_Key;
public:
	CRandomKey(void);
	virtual ~CRandomKey(void);
public:
	//Ëæ»úÉú³Ékey
	virtual long long RandKey();
	//
	virtual void SetKey(long long key);
	virtual long long GetKey();
	virtual bool CheckKey(long long key);
	//
	virtual void SetLowKey(int key);
	virtual int GetLowKey();
	virtual bool CheckLowKey( int key );
	//
	virtual void SetHighKey(int key);
	virtual int GetHighKey();
	virtual bool CheckHighKey( int key );
};

inline CRandomKey::CRandomKey(void)
{
	RandKey();
}

inline CRandomKey::~CRandomKey(void)
{
	RandKey();
}

inline long long CRandomKey::RandKey()
{
	int r1 = rand();
	int r2 = rand();

	while ((r1 == m_Key.key32h) || (r2 == m_Key.key32l))
	{
		r1 = rand();
		r2 = rand();
	}
	m_Key.key32h = r1;
	m_Key.key32l = r2;
	return m_Key.key64;
}

inline void CRandomKey::SetKey( long long key )
{
	m_Key.key64 = key;
}

inline long long CRandomKey::GetKey()
{
	return m_Key.key64;
}

inline bool CRandomKey::CheckKey( long long key )
{
	return m_Key.key64==key;
}

inline void CRandomKey::SetLowKey( int key )
{
	m_Key.key32l = key;
}

inline int CRandomKey::GetLowKey()
{
	return m_Key.key32l;
}

inline bool CRandomKey::CheckLowKey( int key )
{
	return m_Key.key32l==key;
}

inline void CRandomKey::SetHighKey( int key )
{
	m_Key.key32h = key;
}

inline int CRandomKey::GetHighKey()
{
	return m_Key.key32h;
}

inline bool CRandomKey::CheckHighKey( int key )
{
	return m_Key.key32h==key;
}

//////////////////////////////////////////////////////////////////
//CVerifyID
class CVerifyID : public CIDKey, public CRandomKey
{

};