#pragma once
#include <sstream>
#include <unordered_map>

class Uri
{
private:
	DEFINE_STRING(Protocol);
	DEFINE_STRING(Host);
	std::string m_Path;
	DEFINE_STRING(Param);
public:
	Uri(const char* host)
		:m_Protocol("http")
		,m_Host(host)
	{
	}

	void SetPath(const char* path)
	{
		const char* p = path;
		while (*p == '/')
		{
			p++;
			break;
		}
		m_Path = p;
	}

	std::string GetUrl()
	{
		std::stringstream out;

		out << m_Protocol << "://" << m_Host << "/" << m_Path;
		if (m_Param.length()>0)
		{
			out << "?" << m_Param;
		}
		return out.str();
	}

	static std::string ConvParam(std::unordered_map<std::string, std::string>& params)
	{
		std::string n;
		std::stringstream out;
		for (auto iter : params)
		{
			out << n << iter.first << "=" << iter.second;
			if (n.empty())
			{
				n = "&";
			}
		}
		return out.str();
	}
};