/*!
@author 	winterma
@par		Email: mahuaguan@126.com
@par		Copyright (c) 2014
@date		2014-7-8
@brief		Incredibuild�����������ƽ����
*/

#include <fstream>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <time.h>
#include <sstream>
#include <afxdisp.h>
#define MAX_PATH 1000
#define RETARTID 129
#define DELAY 5000
using namespace std;


SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hStatus;

void ServiceMain(int argc, char** argv);
void ControlHandler(DWORD request);
/* �ƽ����������� */
char serviceName[] = "IncrediTrailDelay";
char targetService[] = "IncrediBuild_Agent";
bool stopTargetService();
bool startTargetService();
void writeToLog(string);
string itos(int i);
void setKeyValue(string keyValue);
string GetIncrediBuildLicKey(DATE date);

int main()
{
	SERVICE_TABLE_ENTRY ServiceTable[2];
	ServiceTable[0].lpServiceName = serviceName;
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;

	ServiceTable[1].lpServiceName = NULL;
	ServiceTable[1].lpServiceName = NULL;
	
	StartServiceCtrlDispatcher(ServiceTable);
	return 0;
}


void ControlHandler(DWORD request)
{
	switch (request)
	{
	case SERVICE_CONTROL_STOP:
		OutputDebugString("TestApp stopped.");
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(hStatus, &ServiceStatus);
		return;
	case SERVICE_CONTROL_SHUTDOWN:
		OutputDebugString("Test App Sropped");
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(hStatus, &ServiceStatus);
		return;
	default:
		break;
	}

	SetServiceStatus(hStatus, &ServiceStatus);
	return;
}

void ServiceMain(int argc, char** argv)
{
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
	
	//��SCM��������״̬
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hStatus, &ServiceStatus);
	
	while (ServiceStatus.dwCurrentState == SERVICE_RUNNING)
	{
		COleDateTime curTime = COleDateTime::GetCurrentTime();

		/* �Ӻ�ʱ����� */
		COleDateTimeSpan interval;

		/*���Ե�ʱ���Ӻ������Ϊ��ǰ������һ�룬�պò��ᳬ��30��,ע�����Ϊ0�����*/
		/*
		int days = destTime.GetSecond() / 2;
		if (days == 0)
		{
			days = 1;
		}*/

		/* �����汾���Ӻ�20�죬ע������ʮ��Ļ�ÿ�α���ᵯ�������ڽ����ľ��� */
		interval.SetDateTimeSpan( 20, 0, 0, 0);
		
		if (curTime.GetHour() == 1 && curTime.GetMinute() == 0 && curTime.GetSecond() == 0)
		{
			COleDateTime destTime;
			destTime = curTime + interval;

			COleDateTime dateTime(destTime.GetYear(), destTime.GetMonth(), destTime.GetDay(), destTime.GetHour(), destTime.GetMinute(), destTime.GetSecond());
			DATE Date = (DATE)dateTime;
			string dateStr = GetIncrediBuildLicKey(Date);
			setKeyValue(dateStr);
		}
	}
	
	/*
	SYSTEMTIME lpsystemtime;
	GetLocalTime(&lpsystemtime);
	while (ServiceStatus.dwCurrentState == SERVICE_RUNNING)
	{
		//Sleep(1000*60*59);
		if (lpsystemtime.wHour == 1)
		{
			bool res = stopTargetService();
			if (!res)
			{
				OutputDebugString("Can not stop service.");
			}
			
			res = startTargetService();
			while (true)
			{

			} 
			if (!res)
			{
				OutputDebugString("Can not start service.");
			}
		}
		
	}
	*/

}

string GetIncrediBuildLicKey(DATE date)
{
	/*
	IncrediBuild �������ƺ��㷨
	ע�⣺Incredibuild�жϹ����ǰ��հ�װ����������ʱ�����ľ���ֵ�����м���ģ�������Ҫ���ϸ��°�װʱ������֤������

	��ͬ�汾�������ʱ���ע������λ���ǲ�ͬ��:
	2.40: HKCR\Interface\{E9B0227F-437C-4F7A-86D9-2676B83F359F}\ProxyStubClsid32 = {M1-M2-M3-T1-T2}
	3.20: HKCR\Interface\{B7348B5D-B65D-4BF5-AF63-A3135249ACA7}\ProxyStubClsid32 = {M1-M2-M3-T1-T2}
	3.31: HKCR\Interface\{6F8793A7-3226-4B51-98C1-476A0D1A7AFC}\ProxyStubClsid32
	3.40: HKCR\Interface\{48C6296A-4F4C-4238-A9E1-60E9A544A863}\ProxyStubClsid32 

	�����ʱ Date ��ֵ�� 39598.999988425923 (0x37BA E7FFDF55E340)

	T1 = 37BA
	T2 = E7FFDF55E340
	M1 = 37 * BA * E7 * FF = 23EAEB06
	M2 = DF * 55 = 4A0B
	M3 = E3 * 40 = 38C0
	*/
	BYTE * pByte = (BYTE *)&date;

	BYTE * T1 = (BYTE *) (pByte);
	BYTE * T2 = (BYTE *) (pByte + 2);

	DWORD dwM1 = (DWORD) *pByte;
	dwM1 *= (DWORD) *(pByte + 1);
	dwM1 *= (DWORD) *(pByte + 2);
	dwM1 *= (DWORD) *(pByte + 3);

	WORD wM2 = (WORD) *(pByte + 4);
	wM2 *= (WORD) *(pByte + 5);

	WORD wM3 = (WORD) *(pByte + 6);
	wM3 *= (WORD) *(pByte + 7);

	char ch[1000];
	/************************************************************************/
	/* {%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}  ע����ֵ�ĸ�ʽ   */
	/************************************************************************/
	sprintf_s(ch,"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", dwM1, wM2, wM3, *T1, *(T1+1), *T2, *(T2+1), *(T2+2), *(T2+3), *(T2+4), *(T2+5));
	string tempStr(ch);
	return tempStr;
}

void setKeyValue(string keyValue)
{
	HKEY hKey;

	/************************************************************************/
	/* data_set��5.0�汾��¼���ΰ�װʱ���ֵ����·����ÿ���汾��λ�ö���һ����Ҫ���� */
	/* ��ֵ��λ��Ҳ���ѣ�ʹ��һ���� Process Monitor��΢���Ʒ���ϲ��Ǿ�Ʒ���������� */
	/* ���õ�^_^�����������Incredibuild�ڰ�װ�����и��ĵ�ע����ֵ������         */
	/* ProxyStubClsid32�����ֱȽ϶����һ�������								*/
	/************************************************************************/
	LPCTSTR data_Set = "Wow6432Node\\Interface\\{1E4B9266-71AC-499E-8875-C22E4D301367}\\ProxyStubClsid32";
	if (::RegOpenKeyEx(HKEY_CLASSES_ROOT, data_Set, 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
	{
		writeToLog("Open Key Error");
		return;
	}

	TCHAR res[MAX_PATH];//=new BYTE[MAX_PATH];
	DWORD type_1 = REG_SZ;
	DWORD cbData_1 = MAX_PATH;
	/*   ��ȡ����

	if (RegQueryValueEx(hKey, L"", NULL, &type_1, (LPBYTE)res, &cbData_1) != ERROR_SUCCESS)
	{
		cout << "read error" << endl;
		return;
	}
	*/

	LPCTSTR data = keyValue.c_str();


	/* ��ʹ�޸ĳɹ�Ҳ�Ƿ��ش����޷��жϣ����������о��� */
	::RegSetValueEx(hKey, "", 0, REG_SZ, (LPBYTE)data, 100) != ERROR_SUCCESS;

	if (RegQueryValueEx(hKey, "", NULL, &type_1, (LPBYTE)res, &cbData_1) != ERROR_SUCCESS)
	{
		writeToLog("Read value error");
		return;
	}
	
	string tempStr(res);
	writeToLog("Set value: " + tempStr);

	::RegCloseKey(hKey);
	return;
}

/* ��5.0֮ǰ�İ汾�ƽ��в�û��ʹ�ô˺�����������һ����Ƴ���Գ��Ϊ�ο�^_^ */
bool stopTargetService()
{
	SC_HANDLE hSCM, TargetService;
	SERVICE_STATUS targetServiceStatus;
	bool success = false;

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE | SC_MANAGER_CREATE_SERVICE);
	if (!hSCM)
	{
		OutputDebugString("Open SCManager failed.");
		return false;
	}

	//AppService = CreateService(hSCM, serviceName, serviceName, SERVICE_ALL_ACCESS | SERVICE_STOP, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, )
	TargetService = OpenService(hSCM, targetService, SERVICE_ALL_ACCESS);
	QueryServiceStatus(TargetService, &targetServiceStatus);
	bool res = ControlService(TargetService, SERVICE_CONTROL_STOP, &targetServiceStatus);

	/* ���Ŀ�����״̬ */
	QueryServiceStatus(TargetService, &targetServiceStatus);
	

	if (!res)
	{
		OutputDebugString("Can not control target service.");
		writeToLog("Stop Failed!\n");
		return false;
	}

	/* ����res���֮�󣬱�����ѭ�� */
	while (targetServiceStatus.dwCurrentState != SERVICE_STOPPED)
	{
		writeToLog("This is itos -- " + itos(targetServiceStatus.dwCurrentState));
		Sleep(1);
		QueryServiceStatus(TargetService, &targetServiceStatus);

	}

	writeToLog("Stop success!\n");
	
	return true;
}

/* ��5.0֮ǰ�İ汾�ƽ��в�û��ʹ�ô˺�����������һ����Ƴ���Գ��Ϊ�ο�^_^ */
bool startTargetService()
{
	SC_HANDLE hSCM, TargetService;
	SERVICE_STATUS targetServiceStatus;
	bool success = false;

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE | SC_MANAGER_CREATE_SERVICE);
	if (!hSCM)
	{
		writeToLog("Open SCManager failed.");
		return false;
	}

	//AppService = CreateService(hSCM, serviceName, serviceName, SERVICE_ALL_ACCESS | SERVICE_STOP, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, )
	TargetService = OpenService(hSCM, targetService, SERVICE_ALL_ACCESS);
	QueryServiceStatus(TargetService, &targetServiceStatus);
	bool res = StartService(TargetService, 0, NULL);

	

	if (!res)
	{
		writeToLog("Start failed");
		return false;
	}

	/* ���Ŀ�����״̬��ͬ��д�ڼ����ƽ��֮�� */
	QueryServiceStatus(TargetService, &targetServiceStatus);
	while (targetServiceStatus.dwCurrentState != SERVICE_RUNNING)
	{
		writeToLog("Starting\n");
		Sleep(1);
		QueryServiceStatus(TargetService, &targetServiceStatus);
	}
	writeToLog("Start success!");
	return true;
}


void writeToLog(string msg)
{
	return;
	time_t tick;
	struct tm tm;
	char s[100];
	tick = time(NULL);
	tm = *localtime(&tick);
	strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &tm);
	ofstream outfile("D:\\log.txt", ios::app);
	outfile << "[" << s << "]: " << msg.c_str() << endl;
	outfile.close();
	return;
}

string itos(int i)
{
	stringstream s;
	s << i;
	return s.str();
}
