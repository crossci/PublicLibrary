#pragma once
#include "convapi.h"

#define LOAD_CONFIG_INT(element,name) \
	int name = -1; \
	element->Attribute(#name,&name); \
	if (name>=0){ \
		GetLogger()->log(_LOG_NOTICE,"%s[%d]\n",#name,name); \
	}else{ \
		GetLogger()->log(_LOG_ERROR,"config %s is null\n",#name); \
	}

#define LOAD_CONFIG_LONGLONG(element,name) \
	long long name = -1; \
	const char* str##name = element->Attribute(#name); \
	if (str##name!=0){ \
		name = atoll(str##name); \
		GetLogger()->log(_LOG_NOTICE,"%s[%lld]\n",#name,name); \
			}else{ \
		GetLogger()->log(_LOG_ERROR,"config %s is null\n",#name); \
			}

#define LOAD_CONFIG_DOUBLE(element,name) \
	double name = -1; \
	element->Attribute(#name, &name); \
	if (name >= 0){ \
	GetLogger()->log(_LOG_NOTICE, "%s[%lf]\n", #name, name); \
	}else{ \
			GetLogger()->log(_LOG_ERROR, "config %s is null\n", #name); \
	}

#define LOAD_CONFIG_STRING(element,name) \
	const char* name = element->Attribute((#name)); \
	if (name){ \
		char buf[1024]; \
		ZERO_MEMORYQ(buf); \
		int len = sizeof(buf); \
		ConvAPI::Utf8ToGB(name,buf,len); \
		GetLogger()->log(_LOG_NOTICE,"%s[%s]\n",#name,buf); \
	}else{ \
		GetLogger()->log(_LOG_ERROR,"config %s is null\n",#name); \
	}
