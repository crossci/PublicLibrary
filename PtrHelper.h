#pragma once

template <class T> class CPtrHelper
{
private:
	// Pointer variable
	T* m_Ptr;
public:
	// Constructors
	CPtrHelper() 
	{	
		m_Ptr = new T;
	}

	CPtrHelper(T* lp)
	{
		m_Ptr = lp ;
		if ( m_Ptr != NULL)
		{
			m_Ptr->AddRef() ;
		}
	}

	// Destructor
	virtual ~CPtrHelper() 
	{
		Release() ;
	}

	// Reset
	void Release()
	{
		if (m_Ptr != NULL)
		{
			T* pOld = m_Ptr ;
			m_Ptr = NULL ;
			pOld->Release() ;
		}
	}

	// Conversion
	operator T*() { return m_Ptr ;}

	// Pointer operations
	T& operator*()  { return *m_Ptr ;}
	T** operator&() { return &m_Ptr ;}
	T* operator->() { return m_Ptr ;}

	// Assignment from the same interface
	T* operator=(T* pI)
	{
		if (m_Ptr != pI)
		{			
			IUnknown* pOld = m_Ptr ;    // Save current value.
			m_Ptr = pI ;                // Assign new value.
			if (m_Ptr != NULL)
			{
				m_Ptr->AddRef() ;
			}
			if (pOld != NULL)
			{
				pOld->Release() ;       // Release the old interface.
			}
		}
		return m_Ptr ;
	}

	// Boolean functions
	BOOL operator!() { return (m_Ptr == NULL) ? TRUE : FALSE ;}

	// Requires a compiler that supports BOOL
	operator BOOL() const
	{ 
		return (m_Ptr != NULL) ? TRUE : FALSE ; 
	}
};