#ifndef __TP_LOG_H__
#define __TP_LOG_H__

#include <iostream>
#include <time.h>
#include <cstring>
#include  <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "Lock.h"
#include "Macro.h"

#ifdef WIN32
#define strcasecmp stricmp
#define localtime_r(x,y) localtime_s(y,x)
#endif 

typedef enum _log_level { 
	_LOG_NONE = 0,
	_LOG_DEBUG, 
	_LOG_NOTICE, 
	_LOG_WARN, 
	_LOG_ERROR, 
	_LOG_FATAL, 
	_LOG_MAX
} _log_level;

class CLogger
{
public:
	CLogger()
	{
		logfp = NULL;
		logfile = NULL;
		loglevel = _LOG_DEBUG;
	}

	virtual ~CLogger()
	{
		this->close();
		if(NULL!=logfile){
			free(logfile);
			logfile = NULL;
		}
	}

	int init(char *file,_log_level level)
	{
		if(NULL == file){
			return -1;
		}
		if(level <= _LOG_NONE|| level >= _LOG_MAX){
			return -1;
		}
		if(logfile == NULL){
			logfile = strdup(file);
		}
		if(NULL == logfile){
			return -1;
		}
		loglevel = level;
		if(file[strlen(file)-1] == '-'){
			logfp = stdout;
		}else{
			logfp = fopen(file,"a+");
			if(NULL == logfp){
				logfp = stdout;
				return -1;
			}else{
				setvbuf(logfp, (char *)NULL, _IOLBF, 1024);
			}
		}
		return 0;
	}
	int close()
	{
		if(logfp && logfp!=stdout){
			fclose(logfp);
		}
		logfp = stdout;
		return 0;
	}
	int log(_log_level level,const char *fmt, ...)
	{
		va_list ap;
		time_t t;
		struct tm tminfo; 

		if(level < loglevel){
			return 0;
		}
		t = time(NULL);
		localtime_r(&t,&tminfo);
		m_Lock.Lock();
		if(logfp==NULL){
			logfp = stdout;
		}
		va_start(ap, fmt);
		fprintf(logfp, "[%.4d-%.2d-%.2d %.2d:%.2d:%.2d] [%s]\n",1900+tminfo.tm_year,
			tminfo.tm_mon+1,tminfo.tm_mday,tminfo.tm_hour,tminfo.tm_min,tminfo.tm_sec,level2str(level));
		vfprintf(logfp, fmt, ap);
		va_end(ap);
		m_Lock.Unlock();

		return 0;
	}
	_log_level str2level(char *str)
	{
		if(strcasecmp(str,"debug")==0){
			return _LOG_DEBUG;
		}else if(strcasecmp(str,"notice")==0){
			return _LOG_NOTICE;
		}else if(strcasecmp(str,"warn")==0){
			return _LOG_WARN;
		}else if(strcasecmp(str,"error")==0){
			return _LOG_ERROR;
		}else if(strcasecmp(str,"fatal")==0){
			return _LOG_FATAL;
		}else{
			return _LOG_NONE;
		}
	}
	const char * level2str(_log_level level)
	{
		switch(level){
		case _LOG_DEBUG:
			return "debug";
		case _LOG_NOTICE:
			return "notice";
		case _LOG_WARN:
			return "warn";
		case _LOG_ERROR:
			return "error";
		case _LOG_FATAL:
			return "fatal";
		default:
			return "";
		}
	}
	int rotate()
	{
		int ret;
		char newfn[1024];

		if(logfp == stdout){
			return 0;
		}
		ret = snprintf_safe(newfn,sizeof(newfn),"%s.%lld",logfile,time(NULL));
		if(ret >= (int)sizeof(newfn)){
			return -1;
		}
		m_Lock.Lock();
		fclose(logfp);
		logfp = NULL;
		if (rename(logfile, newfn) == 0)
		{
			this->init(logfile, loglevel);
		}
		m_Lock.Unlock();

		return 0;
	}
private:
	FILE *logfp;
	char *logfile;
	_log_level loglevel;
	CLock m_Lock;
};

inline CLogger* GetLogger()
{
	static CLogger g_logger;
	return &g_logger;
}


#endif

