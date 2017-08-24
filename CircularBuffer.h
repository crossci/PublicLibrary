#pragma once
#include "string.h"

class CCircularBuffer
{
protected:
	// 缓存
	char* m_pBuffer;
	// 缓存大小
	int m_iBuferSize;
	// 头位置
	int m_iHeadPos;
	// 尾位置
	int m_iTailPos;
protected:
	// 倍数增加缓存容量
	virtual void BufferResize(void);
	// 检查益处
	virtual bool IsOverflow(int Length);
	// 判断是否超过缓存
	virtual bool IsIndexOverflow(int Length);
	// 整理内存
	virtual void Sort(void);
public:
	CCircularBuffer(void)
	{
		m_iBuferSize = 1024;
		m_pBuffer = new char[m_iBuferSize];
		m_iHeadPos = 0;
		m_iTailPos = 0;
	}

	virtual ~CCircularBuffer(void)
	{
		m_iBuferSize = 0;
		if(m_pBuffer)
		{
			delete m_pBuffer;
			m_pBuffer = 0;
		}
	}
	// 初始头尾化位置
	virtual void SetEmpty(void);
	// 设置废弃字节
	virtual bool HeadIncrease(int increasement=1); 
	//
	virtual void SetSize(int Length);
	//
	virtual int GetSize(void);
	// 得到数据大小
	virtual int GetCount(void);
	//获得缓存
	virtual const char* GetBuffer(void);
	//查找字符
	virtual int Find(char c,int start=0);
	//
	virtual bool Copy(CCircularBuffer* pCircularBuffer,int Length);
	virtual bool Append(CCircularBuffer* pCircularBuffer,int Length);
public:
	// 写缓存
	virtual bool Write(const char* lpBuffer, int Length);
	// 读缓存
	virtual bool Read(char* lpBuffer, int Length);
	virtual bool Read(char* lpBuffer, int Length, int& RetLength);
	virtual bool Read(int pos, char* lpBuffer, int Length);
	//unsigned char
	virtual bool WriteChar(unsigned char n);
	virtual bool ReadChar(unsigned char& n);
	//Short
	virtual bool WriteShort(unsigned short n);
	virtual bool ReadShort(unsigned short& n);
	//unsigned int
	virtual bool WriteInt(unsigned int n);
	virtual bool ReadInt(unsigned int& n);
	//int64
	virtual bool WriteLonglong(long long n);
	virtual bool ReadLonglong(long long& n);
	//float
	virtual bool WriteFloat(float n);
	virtual bool ReadFloat(float& n);
	//double
	virtual bool WriteDouble(double n);
	virtual bool ReadDouble(double& n);
	//string
	virtual bool ReadString(char* p,int len);
};

//倍数增加缓存容量
inline void CCircularBuffer::BufferResize(void)
{
	//当前缓存大小
	int prevBuferSize = m_iBuferSize;
	//扩大一倍
	m_iBuferSize <<= 1;
	//申请缓存
	char *pNewData = new char[m_iBuferSize];
	//拷贝到新缓存
	memcpy(pNewData, m_pBuffer, prevBuferSize);
	//如果循环使用
	if (m_iTailPos<m_iHeadPos) 
	{
		//拷贝循环缓存
		memcpy(pNewData+prevBuferSize, m_pBuffer, m_iTailPos);
		m_iTailPos += prevBuferSize;
	}
	delete [] m_pBuffer;
	m_pBuffer = pNewData;
}

// 检查益处
inline bool CCircularBuffer::IsOverflow(int Length)
{
	return (Length>=(m_iBuferSize-GetCount()));
}

// 判断是否超过缓存
inline bool CCircularBuffer::IsIndexOverflow(int Length)
{
	return ((Length+m_iTailPos)>=m_iBuferSize);
}

// 整理内存
inline void CCircularBuffer::Sort(void)
{
    if (m_iTailPos<m_iHeadPos) 
    {
		//需要整理
		//申请缓存
		char *pNewData = new char[m_iBuferSize];
		int count = m_iBuferSize-m_iHeadPos;
		//拷贝到新缓存
		memcpy(pNewData, m_pBuffer+m_iHeadPos, count);

		//拷贝循环缓存
		memcpy(pNewData+count, m_pBuffer, m_iTailPos);
		m_iTailPos += count;
		m_iHeadPos = 0;

		delete [] m_pBuffer;
		m_pBuffer = pNewData;
    }
}

//得到数据大小
inline int CCircularBuffer::GetCount(void)
{
	//尾减去头
	int count = m_iTailPos-m_iHeadPos;
	//如果负数则缓存循环使用，加上循环缓存
	if (count<0) count = m_iBuferSize+count;
	return count;
}

inline void CCircularBuffer::SetSize(int Length)
{
	// 判断是否需要申请缓存
	while (IsOverflow(Length))
	{
		//重新申请缓存
		BufferResize();
	}
}

inline int CCircularBuffer::GetSize(void)
{
	return m_iBuferSize-GetCount();
}

// 设置废弃字节
inline bool CCircularBuffer::HeadIncrease(int increasement)
{
	m_iHeadPos += increasement;
	m_iHeadPos %= m_iBuferSize;
	return m_iHeadPos!=m_iTailPos;
}

// 初始头尾化位置
inline void CCircularBuffer::SetEmpty(void)
{
	m_iHeadPos=0;
	m_iTailPos=0;
}

//获得缓存
inline const char* CCircularBuffer::GetBuffer(void)
{
	Sort();
	return m_pBuffer+m_iHeadPos;
}

//查找字符
inline int CCircularBuffer::Find(char c,int start)
{
	//判断是否超过最大数据
	int Count = GetCount();
	int len = -1;
	int i = 0;
	char* pData = 0;
	//如果循环使用
	if (m_iTailPos<m_iHeadPos)
	{
		//一次
		int fc, sc;
		fc = m_iBuferSize-m_iHeadPos;
		sc = Count - fc;

		pData = m_pBuffer+m_iHeadPos;
		for(i=start;i<fc;i++)
		{
			if(pData[i]==c)
			{
				len = i+1;
				break;
			}
		}
		if(sc&&(len==-1))
		{
			if(start<fc)
			{
				start = 0;
			}
			else
			{
				start = start-fc;
			}
			pData = m_pBuffer;
			for(i=start;i<sc;i++)
			{
				if(pData[i]==c)
				{
					len = fc+i+1;
					break;
				}
			}
		}
	}
	else
	{
		//一次
		pData = m_pBuffer+m_iHeadPos;
		for(i=start;i<Count;i++)
		{
			if(pData[i]==c)
			{
				len = i+1;
				break;
			}
		}
	}
	return len;
}

//
inline bool CCircularBuffer::Copy(CCircularBuffer* pCircularBuffer,int Length)
{
	SetEmpty();
	// 判断是否需要申请缓存
	while (IsOverflow(Length))
	{
		//重新申请缓存
		BufferResize();
	}
	if(pCircularBuffer->Read(m_pBuffer,Length))
	{
		m_iTailPos = Length;
		return true;
	}
	return false;
}

//
inline bool CCircularBuffer::Append(CCircularBuffer* pCircularBuffer,int Length)
{
	// 判断是否需要申请缓存
	while (IsOverflow(Length))
	{
		//重新申请缓存
		BufferResize();
	}
	//判断是否需要循环使用
	if (IsIndexOverflow(Length))
	{
		//缓存
		int FirstCopyLen = m_iBuferSize-m_iTailPos;
		//循环缓存
		int SecondCopyLen = Length-FirstCopyLen;
		//拷贝缓存
		if(pCircularBuffer->Read(m_pBuffer+m_iTailPos,FirstCopyLen))
		{
			//循环缓存
			if (SecondCopyLen)
			{
				//拷贝到循环缓存
				if(pCircularBuffer->Read(FirstCopyLen,m_pBuffer,SecondCopyLen))
				{
					//增加大小
					m_iTailPos = SecondCopyLen;
					return true;
				}
			}
			else
			{
				//正常
				m_iTailPos = 0;
				return true;
			}
		}
	}
	else
	{
		if(pCircularBuffer->Read(m_pBuffer+m_iTailPos,Length))
		{
			m_iTailPos += Length;
			return true;
		}
	}


	return false;
}

//写缓存
inline bool CCircularBuffer::Write(const char* lpBuffer, int Length)
{
	// 如果长度0返回
	if (Length==0)
	{
		return false;
	}
	// 判断是否需要申请缓存
	while (IsOverflow(Length))
	{
		//重新申请缓存
		BufferResize();
	}
	//判断是否需要循环使用
	if (IsIndexOverflow(Length))
	{
		//缓存
		int FirstCopyLen = m_iBuferSize-m_iTailPos;
		//循环缓存
		int SecondCopyLen = Length-FirstCopyLen;
		//拷贝缓存
		memcpy(m_pBuffer+m_iTailPos, lpBuffer, FirstCopyLen);
		//循环缓存
		if (SecondCopyLen)
		{
			//拷贝到循环缓存
			memcpy(m_pBuffer, lpBuffer+FirstCopyLen, SecondCopyLen);
			//增加大小
			m_iTailPos = SecondCopyLen;
		}
		else
		{
			//正常
			m_iTailPos = 0;
		}
	}
	else
	{
		//拷贝缓存
		memcpy(m_pBuffer+m_iTailPos, lpBuffer, Length);
		//增加大小
		m_iTailPos += Length;
	}

	return true;
}

// 读缓存
inline bool CCircularBuffer::Read(char* lpBuffer, int Length)
{
	//判断是否超过最大数据
	int Count = GetCount();
	if(Length>Count)
	{
		return false;
	}
	//
	if (Length < m_iBuferSize-m_iHeadPos)
	{
		memcpy(lpBuffer, m_pBuffer+m_iHeadPos, Length);
	}
	else
	{
		int fc, sc;
		fc = m_iBuferSize-m_iHeadPos;
		sc = Length - fc;
		memcpy(lpBuffer, m_pBuffer+m_iHeadPos, fc);
		if (sc) memcpy(lpBuffer+fc, m_pBuffer, sc);
	}

	return true;
}

// 读缓存
inline bool CCircularBuffer::Read(char* lpBuffer, int Length, int& RetLength)
{
	//判断是否超过最大数据
	int Count = GetCount();
	if(Length>Count)
	{
		Length = Count;
	}
	//
	if (Length < m_iBuferSize-m_iHeadPos)
	{
		memcpy(lpBuffer, m_pBuffer+m_iHeadPos, Length);
	}
	else
	{
		int fc, sc;
		fc = m_iBuferSize-m_iHeadPos;
		sc = Length - fc;
		memcpy(lpBuffer, m_pBuffer+m_iHeadPos, fc);
		if (sc) memcpy(lpBuffer+fc, m_pBuffer, sc);
	}
	RetLength = Length;

	return true;
}

//检测word
inline bool CCircularBuffer::Read(int pos, char* lpBuffer, int Length)
{
	//判断是否超过最大数据
	int Count = GetCount();
	if(Length+pos>Count)
	{
		return false;
	}
	//
	if( (pos+Length) <= (m_iBuferSize-m_iHeadPos) )
	{
		memcpy(lpBuffer, m_pBuffer+m_iHeadPos+pos, Length);
	}
	else
	{
		int fc;
		fc = m_iBuferSize-m_iHeadPos;
		if(fc>pos)
		{
			int l = Length-(fc-pos);
			memcpy(lpBuffer, m_pBuffer+m_iHeadPos+pos, l);
			memcpy(lpBuffer+l, m_pBuffer, Length-l);
		}
		else
		{
			pos -= fc;
			memcpy(lpBuffer, m_pBuffer+pos, Length);
		}
	}

	return true;
}

inline bool CCircularBuffer::WriteChar(unsigned char n)
{
	return Write((const char*)&n,sizeof(unsigned char));
}

inline bool CCircularBuffer::ReadChar(unsigned char& n)
{
	if(Read((char*)&n,sizeof(unsigned char)))
	{
		HeadIncrease(sizeof(unsigned char));
		return true;
	}
	return false;
}

inline bool CCircularBuffer::WriteShort(unsigned short n)
{
	return Write((const char*)&n,sizeof(unsigned short));
}

inline bool CCircularBuffer::ReadShort(unsigned short& n)
{
	if(Read((char*)&n,sizeof(unsigned short)))
	{
		HeadIncrease(sizeof(unsigned short));
		return true;
	}
	return false;
}

inline bool CCircularBuffer::WriteInt(unsigned int n)
{
	return Write((const char*)&n,sizeof(unsigned int));
}

inline bool CCircularBuffer::ReadInt(unsigned int& n)
{
	if(Read((char*)&n,sizeof(unsigned int)))
	{
		HeadIncrease(sizeof(unsigned int));
		return true;
	}
	return false;
}

inline bool CCircularBuffer::WriteLonglong(long long n)
{
	return Write((const char*)&n,sizeof(long long));
}

inline bool CCircularBuffer::ReadLonglong(long long& n)
{
	if(Read((char*)&n,sizeof(long long)))
	{
		HeadIncrease(sizeof(long long));
		return true;
	}
	return false;
}

inline bool CCircularBuffer::WriteFloat(float n)
{
	return Write((const char*)&n,sizeof(float));
}

inline bool CCircularBuffer::ReadFloat(float& n)
{
	if(Read((char*)&n,sizeof(float)))
	{
		HeadIncrease(sizeof(float));
		return true;
	}
	return false;
}

inline bool CCircularBuffer::WriteDouble(double n)
{
	return Write((const char*)&n,sizeof(double));
}

inline bool CCircularBuffer::ReadDouble(double& n)
{
	if(Read((char*)&n,sizeof(double)))
	{
		HeadIncrease(sizeof(double));
		return true;
	}
	return false;
}

inline bool CCircularBuffer::ReadString(char* p,int len)
{
	if(Read(p,len))
	{
		HeadIncrease(len);
		return true;
	}
	return false;
}