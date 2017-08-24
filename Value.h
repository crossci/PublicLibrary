#pragma once
#include "string"
#include "..\MemoryStream\IMemoryStream.h"

namespace _VALUE_TYPE
{
	enum 
	{
		UNKNOWN = 0,
		CHAR,
		SHORT,
		INT,
		LONGLONG,
		FLOAT,
		DOUBLE,
		STRING,
	};
}


#pragma pack(push,1)
typedef struct _VALUE 
{
	unsigned char type;
	union
	{
		char charValue;
		short shortValue;
		long long longlongValue;
		float floatValue;
		double doubleValue;
	};
}*_PVALUE;
#pragma pack(pop)

class CValue : public _VALUE
{
protected:
	std::string stringValue;
public:
	CValue(void);
	CValue(const CValue& value);
	virtual ~CValue(void);
public:
	void operator=(const CValue& value);
	//_VALUE

	void operator=(const _VALUE& value);
	operator _VALUE&();
	//char
	/*void operator=(const char value);
	operator char();*/
	//short
	void operator=(const short value);
	operator short();
	//int
	void operator=(const int value);
	operator int();
	////long long
	void operator=(const long long value);
	operator long long();
	//float
	/*void operator=(const float value);
	operator float();*/
	//double
	/*void operator=(const double value);
	operator double();*/
	//string
	void operator=(const std::string& value);
	operator std::string&();
	//const char*
	void operator=(const char* value);
	operator const char*();
	//
	void memcpy(const void* data,int length);
	//
	int get_data(void* data,int length);
	int size();
	void write(IMemoryStream* pPacketData);
	void read(IMemoryStream* pPacketData);
};

inline CValue::CValue( void )
{
	type = _VALUE_TYPE::UNKNOWN;
	longlongValue = 0;
}

inline CValue::~CValue( void )
{

}
inline CValue::CValue(const CValue& value)
{
	this->type = value.type;
	this->longlongValue = value.longlongValue;
	if (this->type == _VALUE_TYPE::STRING)
	{
		this->stringValue = value.stringValue;
	}
}

inline void CValue::operator=(const CValue& value)
{
	this->type = value.type;
	this->longlongValue = value.longlongValue;
	if (this->type == _VALUE_TYPE::STRING)
	{
		this->stringValue = value.stringValue;
	}
}

inline void CValue::operator=( const _VALUE& value )
{
	_PVALUE p = static_cast<_PVALUE>(this);
	::memcpy(p,&value,sizeof(value));
}

inline CValue::operator _VALUE&()
{
	_PVALUE p = static_cast<_PVALUE>(this);
	return *p;
}

//inline void CValue::operator=( const char value )
//{
//	type = _VALUE_TYPE::CHAR;
//	charValue = value;
//}
//
//inline CValue::operator char()
//{
//	return charValue;
//}

inline void CValue::operator=( const short value )
{
	type = _VALUE_TYPE::SHORT;
	shortValue = value;
}

inline CValue::operator short()
{
	return shortValue;
}

inline void CValue::operator=( const int value )
{
	type = _VALUE_TYPE::INT;
	longlongValue = value;
}

inline CValue::operator int()
{
	return longlongValue;
}

inline void CValue::operator=( const long long value )
{
	type = _VALUE_TYPE::LONGLONG;
	longlongValue = value;
}

inline CValue::operator long long()
{
	return longlongValue;
}
//
//inline void CValue::operator=( const float value )
//{
//	type = _VALUE_TYPE::FLOAT;
//	floatValue = value;
//}
//
//inline CValue::operator float()
//{
//	return floatValue;
//}
//
//inline void CValue::operator=( const double value )
//{
//	type = _VALUE_TYPE::DOUBLE;
//	doubleValue = value;
//}
//
//inline CValue::operator double()
//{
//	return doubleValue;
//}

inline void CValue::operator=(const std::string& value)
{
	type = _VALUE_TYPE::STRING;
	stringValue = value;
	longlongValue = stringValue.length();
}

inline CValue::operator std::string&()
{
	return stringValue;
}

inline void CValue::operator=(const char* value)
{
	type = _VALUE_TYPE::STRING;
	stringValue = value;
	longlongValue = stringValue.length();
}

inline CValue::operator const char*()
{
	return stringValue.c_str();
}

inline void CValue::memcpy( const void* data,int length )
{
	const char* p = (const char*)data;
	type = _VALUE_TYPE::STRING;
	stringValue.clear();
	stringValue.append(p,length);
	longlongValue = stringValue.length();
}

inline int CValue::get_data( void* data,int length )
{
	int b = -1;
	if ((type == _VALUE_TYPE::STRING) && (length >= longlongValue))
	{
		::memcpy(data, stringValue.data(), longlongValue);
		b = longlongValue;
	}
	return b;
}
inline int CValue::size()
{
	int size = sizeof(short);
	if (type == _VALUE_TYPE::STRING)
	{
		size += stringValue.length() + sizeof(short);
	}
	else if (type == _VALUE_TYPE::SHORT)
	{
		size += sizeof(short);
	}
	else if (type == _VALUE_TYPE::INT)
	{
		size += sizeof(int);
	}
	else if (type == _VALUE_TYPE::LONGLONG)
	{
		char temp[100] = {0};
		_i64toa_s(longlongValue, temp, 100, 10);
		size += strlen(temp) + sizeof(short);
	}
	return size;
}
inline void CValue::write(IMemoryStream* pPacketData)
{
	pPacketData->WriteShort(type);
	if (type == _VALUE_TYPE::STRING)
	{
		pPacketData->WriteString(stringValue.c_str(), stringValue.length());
	}
	else if (type == _VALUE_TYPE::SHORT)
	{
		pPacketData->WriteShort(longlongValue);
	}
	else if (type == _VALUE_TYPE::INT)
	{
		pPacketData->WriteInt(longlongValue);
	}
	else if (type == _VALUE_TYPE::LONGLONG)
	{
		char temp[100] = { 0 };
		_i64toa_s(longlongValue, temp, 100, 10);
		pPacketData->WriteString(temp, strlen(temp));
	}
}

inline void CValue::read(IMemoryStream* pPacketData)
{
	unsigned short shortType = _VALUE_TYPE::SHORT;
	pPacketData->ReadShort(shortType);
	type = shortType;
	if (type == _VALUE_TYPE::STRING)
	{
		stringValue = pPacketData->ReadString();
	}
	else if (type == _VALUE_TYPE::SHORT)
	{
		pPacketData->ReadShort((unsigned short&)longlongValue);
	}
	else if (type == _VALUE_TYPE::INT)
	{
		pPacketData->ReadInt((unsigned int&)longlongValue);
	}
}
