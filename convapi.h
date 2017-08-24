#pragma once
#include <string>
#include "iconv.h"

#pragma comment(lib,"libiconv.lib")

namespace ConvAPI
{
	inline int strlen(const char16_t* str)
	{
		int i = 0;
		while (str[i] != 0)
		{
			i++;

		}
		return i;
	}

	inline bool Utf8ToUnicode(char* str, char16_t* buffer, int len)
	{
		iconv_t iconv_handle = iconv_open("UCS-2LE", "UTF-8");
		size_t  str_len = ::strlen(str);
		size_t buffer_len = len*sizeof(char16_t);
		size_t csize = iconv(iconv_handle, &str, &str_len, (char**)&buffer, &buffer_len);
		iconv_close(iconv_handle);

		return false;
	}

	inline bool UnicodeToUtf8(const char16_t* str, char* buffer, int len)
	{
		iconv_t iconv_handle = iconv_open("UTF-8", "UCS-2LE");
		size_t  str_len = strlen(str)*sizeof(char16_t);
		size_t buffer_len = len;
		size_t csize = iconv(iconv_handle, (char**)&str, &str_len, (char**)&buffer, &buffer_len);
		iconv_close(iconv_handle);

		return true;
	}

	inline bool StringToUnicode(const char* str, char16_t* buffer, int len)
	{
		iconv_t iconv_handle = iconv_open("UCS-2LE", "ASCII");
		size_t  str_len = ::strlen(str);
		size_t buffer_len = len*sizeof(char16_t);
		size_t csize = iconv(iconv_handle, (char**)&str, &str_len, (char**)&buffer, &buffer_len);
		iconv_close(iconv_handle);

		return false;
	}

	inline bool UnicodeToString(const char16_t* str, char* buffer, int len)
	{
		iconv_t iconv_handle = iconv_open("ASCII", "UCS-2LE");
		size_t  str_len = strlen(str)*sizeof(char16_t);
		size_t buffer_len = len;
		size_t csize = iconv(iconv_handle, (char**)&str, &str_len, (char**)&buffer, &buffer_len);
		iconv_close(iconv_handle);

		return false;
	}

	inline bool Utf8ToGB(const char* str, char* buffer, int len)
	{
// 		char16_t buf[1024];
// 		ZERO_MEMORYQ(buf);
// 		Utf8ToUnicode((char*)str, (char16_t*)buf, sizeof(buf));
// 		UnicodeToString((char16_t*)buf, buffer, len);

		iconv_t iconv_handle = iconv_open("GB2312", "UTF-8");
		size_t  str_len = ::strlen(str);
		size_t buffer_len = len;
		size_t csize = iconv(iconv_handle, (char**)&str, &str_len, (char**)&buffer, &buffer_len);
		iconv_close(iconv_handle);

		return false;
	}
}
