#include <iostream>
#ifdef WIN32
#include <Windows.h>
#else
#ifdef _WIN64
#include <Windows.h>
#ifndef WIN32
#define WIN32 1
#endif
#endif
#endif
#include <atlstr.h>
#include <shellapi.h>
#include <time.h>
#ifndef _SVCLauncher_H
#define _SVCLauncher_H
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#ifndef NULL
#define NULL 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef EOF
#define EOF (-1)
#endif
#ifndef ERROR_OPEN_CONSOLE
#define ERROR_OPEN_CONSOLE 2
#endif
#ifndef ERROR_OPEN_SERVICE
#define ERROR_OPEN_SERVICE 3
#endif
#ifndef ERROR_QUERY_VALUE
#define ERROR_QUERY_VALUE 4
#endif
#ifndef ERROR_STOP_DRIVER
#define ERROR_STOP_DRIVER 5
#endif
#ifndef ERROR_START_DRIVER
#define ERROR_START_DRIVER 6
#endif
#ifndef ERROR_START_SERVICE
#define ERROR_START_SERVICE 11
#endif
#ifndef ERROR_STOP_SERVICE
#define ERROR_STOP_SERVICE 21
#endif
#ifndef ERROR_INSTALL_SERVICE
#define ERROR_INSTALL_SERVICE 31
#endif
#ifndef ERROR_DELETE_SERVICE
#define ERROR_DELETE_SERVICE 41
#endif
#ifndef ERROR_CHANGE_SERVICE
#define ERROR_CHANGE_SERVICE 51
#endif
#ifndef EXIT_OUT_OF_SCHEDULE
#define EXIT_OUT_OF_SCHEDULE 9009
#endif
#ifndef OK
#ifdef TRUE
#define OK TRUE
#else
#define OK 1
#endif
#endif
#ifndef ERROR
#if defined FALSE
#define ERROR FALSE
#elif defined NULL
#define ERROR NULL
#else
#define FALSE 0
#endif
#endif
#ifndef VOID
#define VOID void
#endif
#ifndef DefaultTime
#define DefaultTime 3000
#endif
#ifdef ServiceName
#undef ServiceName
#endif
#define ServiceName "GooseBtSVC"
#ifdef SVCExeName
#undef SVCExeName
#endif
#define SVCExeName "GooseBtSVC.exe"
#ifdef DriverName
#undef DriverName
#endif
#define DriverName "KMDFProcessProtector.sys"
#ifdef InstDrv
#undef InstDrv
#endif
#define InstDrv "InstDrv.exe"
#ifdef TitleText
#undef TitleText
#endif
#define TitleText TEXT("GooseBt 服务控制台")
#endif//_SVCLauncher_H
//#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")//不显示窗口
#pragma comment(linker,"/subsystem:\"windows\" /entry:\"wmainCRTStartup\"")//不显示窗口
using namespace std;
typedef BOOL APIType;
typedef int ServiceType;


/* API 子函数 */
APIType IsAboveVistaVersion(DWORD Win)//判断系统版本是否为 Win 或以上
{
	if (Win <= _WIN32_WINNT)//返回的是 bool
		return TRUE;
	else
		return FALSE;
}

APIType IsAdmin()//是否具备管理员权限
{
	BOOL b;
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup;
	b = AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &AdministratorsGroup);
	if (b)
	{
		if (!CheckTokenMembership(NULL, AdministratorsGroup, &b))
			b = FALSE;
		FreeSid(AdministratorsGroup);
	}
	return b;
}

APIType GetAdminPermission(_TCHAR* tchar, _TCHAR* _option)//以管理员权限启动
{
	if (!IsAboveVistaVersion(_WIN32_WINNT_WIN7))
		return FALSE;
	SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
	sei.lpVerb = TEXT("runas");
	sei.lpFile = tchar;
	sei.lpParameters = _option;
	sei.nShow = SW_SHOWNORMAL;
	if (!ShellExecuteEx(&sei))
		return FALSE;
	return TRUE;
}

wstring getExeFullFilename()
{
	_TCHAR buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	int i = lstrlen(buffer);
	for (; buffer[i] != '\\' && i > 0; --i)
		buffer[i] = 0;
	lstrcat(buffer, _T(SVCExeName));
	return buffer;
}

LPWSTR ConvertCharToLPWSTR(const char* szString)//字符转型函数
{
	size_t dwLen = strlen(szString) + 1;//记得 +1（'\0'）
	int nwLen = MultiByteToWideChar(CP_ACP, 0, szString, (int)dwLen, NULL, 0);//算出合适的长度
	LPWSTR lpszPath = new WCHAR[dwLen];
	MultiByteToWideChar(CP_ACP, 0, szString, (int)dwLen, lpszPath, nwLen);
	return lpszPath;
}

#ifdef WIN32
string GF_GetEXEPath()
{
	char FilePath[MAX_PATH];
	GetModuleFileNameA(NULL, FilePath, MAX_PATH);
	(strrchr(FilePath, '\\'))[1] = 0;
	return string(FilePath);
}
#else
string GF_GetEXEPath()
{
	int rval;
	char link_target[4096];
	char* last_slash;
	size_t result_Length;
	char* result;
	string strExeDir;
	rval = readlink("/proc/self/exe", link_target, 4096);
	if (rval < 0 || rval >= 1024)
		return "";
	link_target[rval] = 0;
	last_slash = strrchr(link_target, '/');
	if (last_slash == 0 || last_slash == link_target)
		return "";
	result_Length = last_slash - link_target;
	result = (char*)malloc(result_Length + 1);
	strncpy(result, link_target, result_Length);
	result[result_Length] = 0;
	strExeDir.append(result);
	strExeDir.append("/");
	free(result);
	return strExeDir;
}
#endif

APIType getStart(const char* ImagePath, const char* param)
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(STARTUPINFOA);
	char cmd[MAX_PATH << 2] = { 0 };
	strcpy_s(cmd, ImagePath);
	strcat_s(cmd, " ");
	strcat_s(cmd, param);
	BOOL working = ::CreateProcessA(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW/*NORMAL_PRIORITY_CLASS*/, NULL, NULL, &si, &pi);
	if (working == 0)
	{
		CloseHandle(pi.hProcess);//记得关闭句柄
		CloseHandle(pi.hThread);
		return FALSE;
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	unsigned long Result = NULL;//恒 >= 0
	GetExitCodeProcess(pi.hProcess, &Result);
	CloseHandle(pi.hProcess);//记得关闭句柄
	CloseHandle(pi.hThread);
	return TRUE;//运行成功即默认成功
}


/* 服务控制函数 */
ServiceType DoSvcStartType(SC_HANDLE scmHandle)//修改服务
{
	SC_HANDLE schService;
	schService = OpenService(scmHandle, _T(ServiceName), SERVICE_CHANGE_CONFIG);
	if (schService == NULL)
		return ERROR_OPEN_SERVICE;

	/* 更改服务信息 */
	SC_LOCK sclLock;
	sclLock = LockServiceDatabase(schService);
	LPWSTR lpDescription = ConvertCharToLPWSTR("此服务由 GooseBt 团队创建，是 GooseBt 第四代反病毒软件的支持服务。");
	SERVICE_DESCRIPTION description = { lpDescription };
	ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, & description);
	if (!ChangeServiceConfig(
		schService,             // handle of service
		SERVICE_NO_CHANGE,      // service type: no change
		SERVICE_AUTO_START,     // service start type
		SERVICE_NO_CHANGE,      // error control: no change
		NULL,                   // binary path: no change
		NULL,                   // load order group: no change
		NULL,                   // tag ID: no change
		NULL,                   // dependencies: no change
		NULL,                   // account name: no change
		NULL,                   // password: no change
		NULL                    // display name: no change
	))
	{
		UnlockServiceDatabase(sclLock);
		CloseServiceHandle(schService);
		MessageBox(NULL, TEXT("错误：修改服务信息失败！"), TitleText, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return ERROR_CHANGE_SERVICE;
	}
	UnlockServiceDatabase(sclLock);
	CloseServiceHandle(schService);
	return EXIT_SUCCESS;
}

ServiceType startService(SC_HANDLE& scmHandle)//启动服务
{
	/* 获得服务句柄 */
	SC_HANDLE schService = OpenService(scmHandle, _T(ServiceName), SERVICE_ALL_ACCESS | DELETE);
	if (schService == NULL)
	{
		CloseServiceHandle(scmHandle);//记得关闭
		MessageBox(NULL, TEXT("错误：打开服务失败！"), TitleText, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return ERROR_OPEN_SERVICE;
	}
	SERVICE_STATUS status;
	if (!QueryServiceStatus(schService, &status))//获得服务的当前状态
	{
		CloseServiceHandle(scmHandle);
		CloseServiceHandle(schService);
		MessageBox(NULL, TEXT("错误：获取服务状态失败！"), TitleText, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return ERROR_QUERY_VALUE;
	}

	/* 服务控制 */
	if (status.dwCurrentState != SERVICE_RUNNING && status.dwCurrentState != SERVICE_START_PENDING)
		StartService(schService, 0, NULL);//启动服务

	/* 再判断 */
	if (!QueryServiceStatus(schService, &status))//再次获得服务的当前状态
	{
		CloseServiceHandle(scmHandle);
		CloseServiceHandle(schService);
		MessageBox(NULL, TEXT("错误：获取服务状态失败！"), TitleText, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return ERROR_QUERY_VALUE;
	}

	if (status.dwCurrentState == SERVICE_RUNNING || status.dwCurrentState == SERVICE_START_PENDING)
	{
		CloseServiceHandle(scmHandle);
		CloseServiceHandle(schService);

		/* 通知驱动开启守护 */
		string DriEvent = "\"", DriParam = "/s /i \"";
		DriEvent += GF_GetEXEPath() + InstDrv + "\"";
		DriParam += GF_GetEXEPath() + DriverName + "\"";
		if (!getStart(DriEvent.c_str(), DriParam.c_str()))
		{
			//MessageBox(NULL, TEXT("错误：开启驱动守护失败，或驱动通讯不成功！"), TitleText, MB_OK | MB_ICONERROR | MB_TOPMOST);
			return ERROR_START_DRIVER;
		}

		return EXIT_SUCCESS;
	}
	else
	{
		CloseServiceHandle(scmHandle);
		CloseServiceHandle(schService);
		MessageBox(NULL, TEXT("错误：启动服务失败！"), TitleText, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return ERROR_START_SERVICE;
	}
}

ServiceType stopService(SC_HANDLE& scmHandle)//停止服务
{
	/* 获得服务句柄 */
	SC_HANDLE schService = OpenService(scmHandle, _T(ServiceName), SERVICE_ALL_ACCESS | DELETE);
	if (schService == NULL)
	{
		CloseServiceHandle(scmHandle);//记得关闭
		MessageBox(NULL, TEXT("错误：打开服务失败！"), TitleText, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return ERROR_OPEN_SERVICE;
	}
	SERVICE_STATUS status;
	if (!QueryServiceStatus(schService, &status))//获得服务的当前状态
	{
		CloseServiceHandle(scmHandle);
		CloseServiceHandle(schService);
		MessageBox(NULL, TEXT("错误：获取服务状态失败！"), TitleText, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return ERROR_QUERY_VALUE;
	}

	/* 服务控制 */
	if (status.dwCurrentState != SERVICE_STOPPED && status.dwCurrentState != SERVICE_STOP_PENDING)
		::ControlService(schService, SERVICE_CONTROL_STOP, &status);//停止服务

	/* 再判断 */
	if (!QueryServiceStatus(schService, &status))//再次获得服务的当前状态
	{
		CloseServiceHandle(scmHandle);
		CloseServiceHandle(schService);
		MessageBox(NULL, TEXT("错误：获取服务状态失败！"), TitleText, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return ERROR_QUERY_VALUE;
	}

	if (status.dwCurrentState == SERVICE_STOPPED || status.dwCurrentState == SERVICE_STOP_PENDING)
	{
		CloseServiceHandle(scmHandle);
		CloseServiceHandle(schService);
		return EXIT_SUCCESS;
	}
	else
	{
		CloseServiceHandle(scmHandle);
		CloseServiceHandle(schService);
		MessageBox(NULL, TEXT("错误：停止服务失败！"), TitleText, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return ERROR_STOP_SERVICE;
	}
}

ServiceType installService(SC_HANDLE &scmHandle)//安装服务
{
	SC_HANDLE schService = OpenService(scmHandle, _T(ServiceName), SERVICE_ALL_ACCESS | DELETE);
	if (schService)//说明服务已安装
	{
		CloseServiceHandle(scmHandle);//记得关闭
		CloseServiceHandle(schService);//记得关闭
		return EXIT_SUCCESS;
	}

	SC_HANDLE serviceHandle = CreateServiceW(scmHandle,
		_T(ServiceName),
		_T(ServiceName),
		SERVICE_ALL_ACCESS | SERVICE_ACCEPT_STOP,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START,
		SERVICE_ERROR_NORMAL,
		getExeFullFilename().c_str(),
		NULL, NULL, L"", NULL, L""
	);
	if (!serviceHandle)
	{
		CloseServiceHandle(scmHandle);
		MessageBox(NULL, TEXT("错误：安装服务失败！"), TitleText, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return ERROR_INSTALL_SERVICE;
	}
	DoSvcStartType(scmHandle);
	CloseServiceHandle(scmHandle);
	CloseServiceHandle(serviceHandle);
	return EXIT_SUCCESS;
}

ServiceType uninstallService(SC_HANDLE& scmHandle)//删除服务
{
	/* 如果服务在运行，则需要停止服务。 */
	SC_HANDLE schService = OpenService(scmHandle, _T(ServiceName), SERVICE_ALL_ACCESS | DELETE);
	if (schService == NULL)
	{
		CloseServiceHandle(scmHandle);//记得关闭
		MessageBox(NULL, TEXT("错误：打开服务失败！"), TitleText, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return ERROR_OPEN_SERVICE;
	}
	SERVICE_STATUS status;
	if (!QueryServiceStatus(schService, &status))//获得服务的当前状态
	{
		CloseServiceHandle(scmHandle);
		CloseServiceHandle(schService);
		MessageBox(NULL, TEXT("错误：获取服务状态失败！"), TitleText, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return ERROR_QUERY_VALUE;
	}
	if (status.dwCurrentState != SERVICE_STOPPED && status.dwCurrentState != SERVICE_STOP_PENDING)
		::ControlService(schService, SERVICE_CONTROL_STOP, &status);//停止服务
	CloseServiceHandle(schService);

	/* 删除服务 */
	schService = OpenService(scmHandle, _T(ServiceName), DELETE);
	if (schService == NULL)
	{
		CloseServiceHandle(scmHandle);
		MessageBox(NULL, TEXT("错误：以删除形式打开服务失败！"), TitleText, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return ERROR_OPEN_SERVICE;
	}
	if (DeleteService(schService))
	{
		CloseServiceHandle(scmHandle);
		CloseServiceHandle(schService);
		return EXIT_SUCCESS;
	}
	else
	{
		CloseServiceHandle(scmHandle);
		CloseServiceHandle(schService);
		MessageBox(NULL, TEXT("错误：删除服务失败！"), TitleText, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return ERROR_DELETE_SERVICE;
	}
}

ServiceType Fix(SC_HANDLE &scmHandle)//修复服务
{
	SC_HANDLE schService = OpenService(scmHandle, _T(ServiceName), SERVICE_ALL_ACCESS | DELETE);
	if (schService == NULL)
		return installService(scmHandle);
	else
	{
		CloseServiceHandle(schService);
		DoSvcStartType(scmHandle);
		return startService(scmHandle);
	}
}


/* 配合命令行的 main 函数 */
int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_CTYPE, "");
	if (argc != 2 || (!((_wcsicmp(argv[1], L"/Start") == 0) || (_wcsicmp(argv[1], L"/Stop") == 0) || (_wcsicmp(argv[1], L"/Install") == 0) || (_wcsicmp(argv[1], L"/Uninstall") == 0) || (_wcsicmp(argv[1], L"/Delete") == 0))))
		return EOF;

	/* 检查是否以管理员权限启动 */
	if (!IsAdmin())
	{
		/*
		_TCHAR _0[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, _0, MAX_PATH);
		cout << "非管理员权限，尝试以管理员权限启动。" << endl;
		Sleep(DefaultTime);
		if (!GetAdminPermission(_0, argv[1]))
		{
			MessageBox(NULL, TEXT("提示：请以管理员权限启动本程序。"), TitleText, MB_OK | MB_ICONWARNING | MB_TOPMOST);
			Sleep(DefaultTime << 1);
		}
		*/
		return EXIT_FAILURE;
	}

	/* 打开服务控制管理数据库并返回服务控制管理数据库的句柄 */
	SC_HANDLE scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (scmHandle == NULL)
	{
		MessageBox(NULL, TEXT("错误：打开系统服务控制器失败！"), TitleText, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return ERROR_OPEN_CONSOLE;
	}
	
	/* 通知驱动停止守护 */
	string DriEvent = "\"", DriParam = "/s /u \"";
	DriEvent += GF_GetEXEPath() + InstDrv + "\"";
	DriParam += GF_GetEXEPath() + DriverName + "\"";
	if (!getStart(DriEvent.c_str(), DriParam.c_str()))
	{
		//MessageBox(NULL, TEXT("错误：与驱动通讯过程发生异常，或驱动停止失败！"), TitleText, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return ERROR_STOP_DRIVER;
	}

	/* 处理命令行参数 */
	if (_wcsicmp(argv[1], L"/Start") == 0)
		return startService(scmHandle);
	else if (_wcsicmp(argv[1], L"/Stop") == 0)
		return stopService(scmHandle);
	else if (_wcsicmp(argv[1], L"/Install") == 0)
		return installService(scmHandle);
	else if (_wcsicmp(argv[1], L"/Uninstall") == 0)
		return uninstallService(scmHandle);
	else if (_wcsicmp(argv[1], L"/Fix") == 0)
		return Fix(scmHandle);
	else
	{
		//MessageBox(NULL, TEXT("警告：运行参数有误，请检查运行参数！"), TitleText, MB_OK | MB_ICONWARNING | MB_TOPMOST);
		return EOF;
	}
	return EXIT_OUT_OF_SCHEDULE;
}