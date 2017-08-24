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
		case CTRL_C_EVENT: //- ���û�������CTRL+C,������GenerateConsoleCtrlEvent API����. 
			break;
		case CTRL_BREAK_EVENT: //- �û�����CTRL+BREAK, ������GenerateConsoleCtrlEvent API����.
			break;
		case CTRL_CLOSE_EVENT: //- ����ͼ�رտ���̨����ϵͳ���͹ر���Ϣ��
		case CTRL_LOGOFF_EVENT: //- �û��˳�ʱ�����ǲ��ܾ������ĸ��û�. 
		case CTRL_SHUTDOWN_EVENT: //- ��ϵͳ���ر�ʱ.
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
