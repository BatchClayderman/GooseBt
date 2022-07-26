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
#include <tchar.h>
#ifndef _DriverLoader_H
#define _DriverLoader_H
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef EOF
#define EOF (-1)
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#ifndef gapTime
#define gapTime 50
#endif
#ifndef loopTime
#define loopTime 80
#endif
#ifndef MAX_NUM
#define MAX_NUM 20
#endif
#ifndef InstDrv
#define InstDrv "InstDrv.exe"
#endif
#endif//_DriverLoader_H
using namespace std;
#pragma comment(linker,"/subsystem:\"windows\" /entry:\"wmainCRTStartup\"")//不显示窗口
typedef BOOL APITYPE;
typedef BOOL KMDFAPI;


/* 字符转型函数 */
WCHAR* CCharToLpcwstr(const char* strings)//转换宽字符
{
	int ASize = MultiByteToWideChar(CP_ACP, 0, strings, -1, NULL, 0);
	WCHAR* PwszUnicode = (wchar_t*)malloc(ASize * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, strings, -1, PwszUnicode, ASize);
	return PwszUnicode;
}

WCHAR* CharToLpcwstr(char* strings)//转换宽字符
{
	int ASize = MultiByteToWideChar(CP_ACP, 0, strings, -1, NULL, 0);
	WCHAR* PwszUnicode = (wchar_t*)malloc(ASize * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, strings, -1, PwszUnicode, ASize);
	return PwszUnicode;
}

void TcharToChar(_TCHAR* tchar, char* _char)//宽字符转换
{
	int iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
	return;
}


/* 文件相关操作 */
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

APITYPE FindFirstFileExists(LPCTSTR lpPath, BOOL dwFilter)//检查文件是否存在
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(lpPath, &fd);
	BOOL bFilter = (FALSE == dwFilter) ? TRUE : fd.dwFileAttributes & dwFilter;
	BOOL RetValue = ((hFind != INVALID_HANDLE_VALUE) && bFilter) ? TRUE : FALSE;
	if (RetValue == FALSE)
	{
		TCHAR SystemPath[MAX_PATH];
		GetSystemDirectory(SystemPath, MAX_PATH);
		TCHAR* _str = new TCHAR[(size_t)lstrlen(SystemPath) + (size_t)lstrlen(L"\\") + (size_t)lstrlen(lpPath) + 1];
		if (_str)
		{
			_str[0] = _T('\0');
			lstrcat(_str, SystemPath);
			lstrcat(_str, L"\\");
			lstrcat(_str, lpPath);
			WIN32_FIND_DATA _fd;
			HANDLE _hFind = FindFirstFile(_str, &_fd);
			BOOL _bFilter = (FALSE == dwFilter) ? TRUE : _fd.dwFileAttributes & dwFilter;
			BOOL _RetValue = ((_hFind != INVALID_HANDLE_VALUE) && _bFilter) ? TRUE : FALSE;
			RetValue = _RetValue;
			delete[] _str;
			FindClose(_hFind);
		}
	}
	FindClose(hFind);
	return RetValue;
}

APITYPE getName(_TCHAR* lpcPath, _TCHAR* tpPath)
{
	int tmp = NULL;
	for (tmp = lstrlen(lpcPath) - 1; tmp >= 0; --tmp)
		if (lpcPath[tmp] == '\\')
			break;
	if (tmp == 0)
		return FALSE;
	for (int i = 0; i < lstrlen(lpcPath) - tmp - 1; ++i)
		tpPath[i] = lpcPath[i + tmp + 1];
	for (tmp = 0; tmp < lstrlen(tpPath); ++tmp)//定位'.'
		if (tpPath[tmp] == '.')
			break;
	if (tmp == 0)
		return TRUE;
	else
		for (int i = lstrlen(tpPath) - 1; i >= tmp - 1; --i)//清空'.'
			tpPath[tmp] = 0;
	return TRUE;
}

APITYPE FillPath(_TCHAR* lpcPath, _TCHAR* tpPath)
{
	_TCHAR szBuf[MAX_PATH << 1] = { 0 };
	LPCTSTR tmp = CCharToLpcwstr(GF_GetEXEPath().c_str());
	swprintf_s(szBuf, MAX_PATH << 1, L"%s%s", tmp, lpcPath);//变成绝对路径
	if (FindFirstFileExists(szBuf, FALSE))//原路径为相对路径且存在
	{
		getName(szBuf, tpPath);
		swprintf_s(szBuf, MAX_PATH << 1, L"\\??\\%s%s", tmp, lpcPath);//加"\\??\\"
		lstrcpy(lpcPath, szBuf);
		return TRUE;
	}
	if (FindFirstFileExists(lpcPath, FALSE))//尝试作为绝对路径检查
	{
		getName(lpcPath, tpPath);
		swprintf_s(szBuf, MAX_PATH << 1, L"\\??\\%s", lpcPath);//加"\\??\\"
		lstrcpy(lpcPath, szBuf);
		return TRUE;
	}
	lstrcpy(lpcPath, L"");
	return FALSE;
}

APITYPE testsigningon(APITYPE bRet)
{
	if (bRet)
		system("echo bcdedit /set testsigning on & bcdedit /set testsigning on & pause");
	else
		system("echo bcdedit /set testsigning off & bcdedit /set testsigning off & pause");
	return TRUE;
}


/* 驱动相关操作 */
KMDFAPI installDvr(CONST WCHAR drvPath[], CONST WCHAR serviceName[])//驱动安装
{
	SC_HANDLE schSCManager = OpenSCManager(// 打开服务控制管理器数据库
		NULL,                              // 目标计算机的名称,NULL：连接本地计算机上的服务控制管理器
		NULL,                              // 服务控制管理器数据库的名称，NULL：打开 SERVICES_ACTIVE_DATABASE 数据库
		SC_MANAGER_ALL_ACCESS              // 所有权限
	);
	if (schSCManager == NULL)
		return FALSE;
	SC_HANDLE schService = CreateService(// 创建服务对象，添加至服务控制管理器数据库
		schSCManager,                    // 服务控件管理器数据库的句柄
		serviceName,                     // 要安装的服务的名称
		serviceName,                     // 用户界面程序用来标识服务的显示名称
		SERVICE_ALL_ACCESS,              // 对服务的访问权限：所有全权限
		SERVICE_KERNEL_DRIVER,           // 服务类型：内核驱动支持服务
		SERVICE_DEMAND_START,            // 服务启动选项：进程调用 StartService 时启动
		SERVICE_ERROR_IGNORE,            // 如果无法启动：忽略错误继续运行
		drvPath,                         // 驱动文件绝对路径，如果包含空格需要在最前面加"\\??\\"
		NULL,                            // 服务所属的负载订购组：服务不属于某个组
		NULL,                            // 接收订购组唯一标记值：不接收
		NULL,                            // 服务加载顺序数组：服务没有依赖项
		NULL,                            // 运行服务的账户名：使用 LocalSystem 账户
		NULL                             // LocalSystem 账户密码
	);
	if (schService == NULL)
	{
		CloseServiceHandle(schSCManager);
		return FALSE;
	}
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return TRUE;
}

KMDFAPI startDvr(CONST WCHAR serviceName[20])//驱动启动
{
	SC_HANDLE schSCManager = OpenSCManager(// 打开服务控制管理器数据库
		NULL,                              // 目标计算机的名称,NULL：连接本地计算机上的服务控制管理器
		NULL,                              // 服务控制管理器数据库的名称，NULL：打开 SERVICES_ACTIVE_DATABASE 数据库
		SC_MANAGER_ALL_ACCESS              // 所有权限
	);
	if (schSCManager == NULL)
		return FALSE;
	SC_HANDLE hs = OpenService(// 打开服务
		schSCManager,          // 服务控件管理器数据库的句柄
		serviceName,           // 要打开的服务名
		SERVICE_ALL_ACCESS     // 服务访问权限：所有权限
	);
	if (hs == NULL)
	{
		CloseServiceHandle(schSCManager);
		return FALSE;
	}
	if (StartService(hs, 0, 0) == 0)
	{
		CloseServiceHandle(hs);
		CloseServiceHandle(schSCManager);
		return FALSE;
	}
	CloseServiceHandle(hs);
	CloseServiceHandle(schSCManager);
	return TRUE;
}

KMDFAPI stopDvr(CONST WCHAR serviceName[20])//驱动停止
{
	SC_HANDLE schSCManager = OpenSCManager(// 打开服务控制管理器数据库
		NULL,                              // 目标计算机的名称，NULL：连接本地计算机上的服务控制管理器
		NULL,                              // 服务控制管理器数据库的名称，NULL：打开 SERVICES_ACTIVE_DATABASE 数据库
		SC_MANAGER_ALL_ACCESS              // 所有权限
	);
	if (schSCManager == NULL)
		return FALSE;
	SC_HANDLE hs = OpenService(// 打开服务
		schSCManager,          // 服务控件管理器数据库的句柄
		serviceName,           // 要打开的服务名
		SERVICE_ALL_ACCESS     // 服务访问权限：所有权限
	);
	if (hs == NULL)
	{
		CloseServiceHandle(schSCManager);
		return FALSE;
	}
	SERVICE_STATUS status;
	if (QueryServiceStatus(hs, &status) == 0)// 如果服务正在运行
	{
		CloseServiceHandle(hs);
		CloseServiceHandle(schSCManager);
		return FALSE;
	}
	if (status.dwCurrentState != SERVICE_STOPPED &&
		status.dwCurrentState != SERVICE_STOP_PENDING
		)
	{
		if (ControlService(           // 发送关闭服务请求
			hs,                       // 服务句柄
			SERVICE_CONTROL_STOP,     // 控制码：通知服务应该停止
			&status                   // 接收最新的服务状态信息
		) == 0)
		{
			CloseServiceHandle(hs);
			CloseServiceHandle(schSCManager);
			return FALSE;
		}
		INT timeOut = 0;
		while (status.dwCurrentState != SERVICE_STOPPED)// 判断超时
		{
			timeOut++;
			if (!QueryServiceStatus(hs, &status))
				return FALSE;
			Sleep(gapTime);
		}
		if (timeOut > loopTime)
		{
			CloseServiceHandle(hs);
			CloseServiceHandle(schSCManager);
			return FALSE;
		}
	}
	CloseServiceHandle(hs);
	CloseServiceHandle(schSCManager);
	return TRUE;
}

BOOL unloadDvr(CONST WCHAR serviceName[20])//驱动卸载
{
	SC_HANDLE schSCManager = OpenSCManager(// 打开服务控制管理器数据库
		NULL,                              // 目标计算机的名称，NULL：连接本地计算机上的服务控制管理器
		NULL,                              // 服务控制管理器数据库的名称，NULL：打开 SERVICES_ACTIVE_DATABASE 数据库
		SC_MANAGER_ALL_ACCESS              // 所有权限
	);
	if (schSCManager == NULL)
		return FALSE;
	SC_HANDLE hs = OpenService(// 打开服务
		schSCManager,          // 服务控件管理器数据库的句柄
		serviceName,           // 要打开的服务名
		SERVICE_ALL_ACCESS     // 服务访问权限：所有权限
	);
	if (hs == NULL)
	{
		CloseServiceHandle(schSCManager);
		return FALSE;
	}
	if (DeleteService(hs) == 0)// 删除服务
	{
		CloseServiceHandle(hs);
		CloseServiceHandle(schSCManager);
		return FALSE;
	}
	CloseServiceHandle(hs);
	CloseServiceHandle(schSCManager);
	return TRUE;
}



/* 遵循 Microsoft Windows 命令行规则的 main 函数 */
int _tmain(int argc, _TCHAR* argv[])//主函数
{
	if (argc == 1)
	{
		if (FindFirstFileExists(CCharToLpcwstr((GF_GetEXEPath() + InstDrv).c_str()), FALSE))
		{
			char cmd[MAX_PATH] = { 0 };
			strcpy_s(cmd, ("\"" + GF_GetEXEPath() + InstDrv + "\"").c_str());
			if (system(cmd) != 2)
				MessageBox(NULL, TEXT("初始化主面板失败！"), TEXT("Goosebt 驱动加载程序"), MB_OK | MB_ICONERROR | MB_TOPMOST);
			/*
			STARTUPINFOA si;
			PROCESS_INFORMATION pi;
			ZeroMemory(&pi, sizeof(pi));
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(STARTUPINFOA);
			if (::CreateProcessA(
					NULL, cmd, NULL, NULL,
					FALSE, SW_SHOW, NULL,
					GF_GetEXEPath().c_str(), &si, &pi)
				)
				WaitForSingleObject(pi.hProcess, INFINITE);
			else
				MessageBox(NULL, TEXT("初始化主面板失败！"), TEXT("Goosebt 驱动加载程序"), MB_OK | MB_ICONERROR | MB_TOPMOST);
			CloseHandle(pi.hProcess);//记得关闭句柄
			CloseHandle(pi.hThread);
			*/
			return EXIT_SUCCESS;
		}
		else
			return EXIT_FAILURE;
	}
	else if (argc <= 2)
	{
		cout << "错误：命令行参数太少！" << endl;
		return EOF;
	}
	_TCHAR ptName[MAX_PATH] = { 0 }, args[MAX_NUM][MAX_PATH] = { {0} };
	for (int i = 0; i < argc - 2 && i < MAX_NUM; ++i)
		lstrcpy(args[i], argv[i + 2]);
	if (_wcsicmp(argv[1], L"/install") == 0)
		for (int i = 0; i < argc - 2; ++i)
		{
			if (!FillPath(args[i], ptName))
			{
				wcout << "错误：系统找不到指定文件——" << argv[i + 2] << "。" << endl;
				continue;
			}
			wcout << args[i] << endl;
			if (installDvr(args[i], ptName) == TRUE)
				wcout << "安装驱动 " << ptName << " 成功！" << endl;
			else
				wcout << "安装驱动 " << ptName << " 失败！" << endl;
		}
	else if (_wcsicmp(argv[1], L"/start") == 0)
		for (int i = 0; i < argc - 2; ++i)
		{
			if (!FillPath(args[i], ptName))
			{
				wcout << "错误：系统找不到指定文件——" << argv[i + 2] << "。" << endl;
				continue;
			}
			wcout << args[i] << endl;
			if (startDvr(ptName) == TRUE)
				wcout << "启动驱动 " << ptName << " 成功！" << endl;
			else
				wcout << "启动驱动 " << ptName << " 失败，您可以尝试重启后重新加载此驱动！" << endl;
		}
	else if (_wcsicmp(argv[1], L"/stop") == 0)
		for (int i = 0; i < argc - 2; ++i)
		{
			if (!FillPath(args[i], ptName))
			{
				wcout << "错误：系统找不到指定文件——" << argv[i + 2] << "。" << endl;
				continue;
			}
			wcout << args[i] << endl;
			if (stopDvr(ptName) == TRUE)
				wcout << "停止驱动 " << ptName << " 成功！" << endl;
			else
				wcout << "停止驱动 " << ptName << " 失败！" << endl;
		}
	else if (_wcsicmp(argv[1], L"/uninstall") == 0)
		for (int i = 0; i < argc - 2; ++i)
		{
			if (!FillPath(args[i], ptName))
			{
				wcout << "错误：系统找不到指定文件——" << argv[i + 2] << "。" << endl;
				continue;
			}
			wcout << args[i] << endl;
			if (unloadDvr(ptName) == TRUE)
				wcout << "卸载驱动 " << ptName << " 成功！" << endl;
			else
				wcout << "卸载驱动 " << ptName << " 失败！" << endl;
		}
	else if (_wcsicmp(argv[1], L"/uis") == 0)
		for (int i = 0; i < argc - 2; ++i)
		{
			if (!FillPath(args[i], ptName))
			{
				wcout << "错误：系统找不到指定文件——" << argv[i + 2] << "。" << endl;
				continue;
			}
			wcout << args[i] << endl;
			stopDvr(ptName);
			unloadDvr(ptName);
			installDvr(args[i], ptName);
			if (startDvr(ptName) == TRUE)
				wcout << "部署驱动环境 " << ptName << " 成功！" << endl;
			else
				wcout << "部署驱动环境 " << ptName << " 失败！" << endl;
		}
	else if (_wcsicmp(argv[1], L"/rm") == 0)
		for (int i = 0; i < argc - 2; ++i)
		{
			if (!FillPath(args[i], ptName))
			{
				wcout << "错误：系统找不到指定文件——" << argv[i + 2] << "。" << endl;
				continue;
			}
			wcout << args[i] << endl;
			stopDvr(ptName);
			if (unloadDvr(ptName) == TRUE)
				wcout << "清理驱动环境 " << ptName << " 成功！" << endl;
			else
				wcout << "清理驱动环境 " << ptName << " 失败！" << endl;
		}
	else if (_wcsicmp(argv[1], L"/test") == 0)
		testsigningon((_wcsicmp(argv[1], L"on") == 0) ? TRUE : FALSE);
	else
	{
		cout << "错误：命令行参数不正确！" << endl;
		return EOF;
	}
	return EXIT_SUCCESS;
}