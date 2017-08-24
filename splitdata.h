#ifndef __SPLIT_DATA__
#define __SPLIT_DATA__

#include "vector"
#include "string"
#include <stdlib.h>

class CSplitData
{
private:
	std::vector<std::string> m_Datas;
public:
	//·Ö¸î×Ö·û´®
	virtual void Split(std::string str, std::string pattern, bool IncludeEnd = false)
	{
		m_Datas.clear();
		std::string::size_type pos, oldpos = 0;
		std::string s;
		str += pattern;//À©Õ¹×Ö·û´®ÒÔ·½±ã²Ù×÷
		size_t length = str.length();
		size_t plen = pattern.length();

		pos = str.find(pattern, oldpos);
		while (pos < length)
		{
			s = str.substr(oldpos, pos - oldpos);
			m_Datas.push_back(s);
			oldpos = pos + 1;
			pos = str.find(pattern, oldpos);
		}
		if (IncludeEnd)
		{
			s = str.substr(oldpos);
			m_Datas.push_back(s);
		}
	}

	std::string& operator[](unsigned int index)
	{
		return m_Datas[index];
	}

	const char* ToString(unsigned int index)
	{
		if (index<m_Datas.size())
		{
			return m_Datas.at(index).c_str();
		}
		return "";		
	}

	std::vector<std::string>& Get()
	{
		return m_Datas;
	}

	int ToInteger(unsigned int index)
	{
		return atoi(ToString(index));
	}

	int Size()
	{
		return m_Datas.size();
	}
};


#endif
