#include <windows.h>
#include <stdio.h>
#include <afx.h>
SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hStatus;

void ServiceMain(int argc, char** argv);
void ControlHandler(DWORD request);
int InitService();
char serviceName[] = "TestApp";

int main()
{
	SERVICE_TABLE_ENTRY ServiceTable[2];
	ServiceTable[0].lpServiceName = (LPWSTR)(CString(serviceName).AllocSysString());
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;

	ServiceTable[1].lpServiceName = NULL;
	ServiceTable[1].lpServiceName = NULL;

	StartServiceCtrlDispatcher(ServiceTable);
	return 0;
}

int InitService()
{
	OutputDebugString("Monitoring started.");
	return 0;
}

void ControlHandler(DWORD request)
{
	switch (request)
	{
	case SERVICE_CONTROL_STOP:
		OutputDebugString("TestApp stopped.");

	default:
		break;
	}
	return;
}

void ServiceMain(int argc, char** argv)
{
	int error;
	ServiceStatus.dwServiceType = SERVICE_WIN32;
	ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;
	ServiceStatus.dwWin32ExitCode = 0;
	ServiceStatus.dwServiceSpecificExitCode = 0;
	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 0;
	
	/* ע�������ƺ��� */
	hStatus = RegisterServiceCtrlHandler(serviceName, (LPHANDLER_FUNCTION)ControlHandler);

	/* ���ע��ʧ�ܣ�����ֱ���˳� */
	if (hStatus == (SERVICE_STATUS_HANDLE)0)
	{
		OutputDebugString("Register Service Control Handler");
		return;
	}


}