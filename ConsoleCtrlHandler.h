#pragma once
#include "waitevent.h"
#include "log.h"

namespace ConsoleCtrlHandler
{
	CWaitEvent m_WaitEvent("Console");

	static BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
	{
		switch(dwCtrlType)
		{
		case CTRL_C_EVENT: //- 当用户按下了CTRL+C,或者由GenerateConsoleCtrlEvent API发出. 
			break;
		case CTRL_BREAK_EVENT: //- 用户按下CTRL+BREAK, 或者由GenerateConsoleCtrlEvent API发出.
			break;
		case CTRL_CLOSE_EVENT: //- 当试图关闭控制台程序，系统发送关闭消息。
		case CTRL_LOGOFF_EVENT: //- 用户退出时，但是不能决定是哪个用户. 
		case CTRL_SHUTDOWN_EVENT: //- 当系统被关闭时.
			m_WaitEvent.Post();
			break;
		}
		return true;
	}

	void WaitConsoleCtrlHandler()
	{
		if(SetConsoleCtrlHandler((PHANDLER_ROUTINE)HandlerRoutine,TRUE))
		{
			m_WaitEvent.Wait(INFINITE);
			SetConsoleCtrlHandler((PHANDLER_ROUTINE)HandlerRoutine,FALSE);
		}
	}
};
