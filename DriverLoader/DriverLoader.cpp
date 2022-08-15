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
#ifndef EXIT_SCM_ERROR
#define EXIT_SCM_ERROR 2
#endif
#ifndef EXIT_NOT_ADMIN
#define EXIT_NOT_ADMIN 3
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
#define MAX_NUM 24
#endif
#ifndef NAME_SIZE
#define NAME_SIZE 32
#endif
#ifndef InstDrv
#define InstDrv "InstDrv.exe"
#endif
#endif//_DriverLoader_H
using namespace std;
//#pragma comment(linker,"/subsystem:\"windows\" /entry:\"wmainCRTStartup\"")//不显示窗口
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
	if (0 == tmp)
		return FALSE;
	for (int i = 0; i < lstrlen(lpcPath) - tmp - 1; ++i)
		tpPath[i] = lpcPath[i + tmp + 1];
	for (tmp = 0; tmp < lstrlen(tpPath); ++tmp)//定位'.'
		if (tpPath[tmp] == '.')
			break;
	if (0 == tmp)
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

APITYPE isAdmin()//是否具备管理员权限
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


/* 驱动相关操作 */
KMDFAPI testsigningon(APITYPE bRet)
{
	if (bRet)
		return 0 == system("echo bcdedit /set testsigning on & bcdedit /set testsigning on & pause") ? TRUE : FALSE;
	else
		return 0 == system("echo bcdedit /set testsigning off & bcdedit /set testsigning off & pause") ? TRUE : FALSE;
}

KMDFAPI openSCM(SC_HANDLE& schSCManager)//打开服务控制管理器数据库
{
	schSCManager = OpenSCManager(// 打开服务控制管理器数据库
		NULL,                    // 目标计算机的名称,NULL：连接本地计算机上的服务控制管理器
		NULL,                    // 服务控制管理器数据库的名称，NULL：打开 SERVICES_ACTIVE_DATABASE 数据库
		SC_MANAGER_ALL_ACCESS   // 所有权限
	);
	return schSCManager ? TRUE : FALSE;
}

NTSTATUS queryDvr(SC_HANDLE schSCManager, CONST WCHAR serviceName[NAME_SIZE])//查询驱动状态
{
	SC_HANDLE hs = OpenService(// 打开服务
		schSCManager,          // 服务控件管理器数据库的句柄
		serviceName,           // 要打开的服务名
		SERVICE_QUERY_STATUS   // 服务访问权限
	);
	DWORD errorlevel = GetLastError();
	SERVICE_STATUS status;
	if (hs)// 驱动有效
	{
		if (QueryServiceStatus(hs, &status) == 0)// 请求服务信息
		{
			CloseServiceHandle(hs);
			return NULL;
		}
		CloseServiceHandle(hs);
		return status.dwCurrentState;
	}
	else
		return errorlevel;
}

KMDFAPI WaitForPendingServiceChanged(SC_HANDLE schSCManager, CONST WCHAR serviceName[NAME_SIZE], DWORD targetStatus)//等待过程结束
{
	SC_HANDLE hs = OpenService(// 打开服务
		schSCManager,          // 服务控件管理器数据库的句柄
		serviceName,           // 要打开的服务名
		SERVICE_QUERY_STATUS   // 服务访问权限
	);
	if (!hs)
		return FALSE;
	SERVICE_STATUS status;
	for (INT timeOut = 0; timeOut < loopTime; ++timeOut)// 判断超时
	{
		Sleep(gapTime);
		if (!QueryServiceStatus(hs, &status))
		{
			CloseServiceHandle(hs);
			return FALSE;
		}
		else if (targetStatus == status.dwCurrentState)// 用 else if 会快些
		{
			CloseServiceHandle(hs);
			return TRUE;
		}
	}
	CloseServiceHandle(hs);
	return FALSE;
}

KMDFAPI installDrv(SC_HANDLE schSCManager, CONST WCHAR drvPath[MAX_PATH], CONST WCHAR serviceName[NAME_SIZE])//驱动安装
{
	NTSTATUS drvStatus = queryDvr(schSCManager, serviceName);
	if (NULL == drvStatus
		|| SERVICE_RUNNING == drvStatus
		|| SERVICE_START_PENDING == drvStatus
		|| SERVICE_STOPPED == drvStatus
		|| SERVICE_STOP_PENDING == drvStatus
	)// 驱动已存在
		return TRUE;
	else if (SERVICE_PAUSED == drvStatus
		|| SERVICE_PAUSE_PENDING == drvStatus
		|| SERVICE_CONTINUE_PENDING == drvStatus
	)// 非法驱动
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
	if (!schService)
		return FALSE;
	CloseServiceHandle(schService);
	return TRUE;
}

KMDFAPI startDrv(SC_HANDLE schSCManager, CONST WCHAR serviceName[NAME_SIZE])//驱动启动
{
	NTSTATUS drvStatus = queryDvr(schSCManager, serviceName);
	if (SERVICE_RUNNING == drvStatus)
		return TRUE;
	else if (SERVICE_START_PENDING == drvStatus)
		return WaitForPendingServiceChanged(schSCManager, serviceName, SERVICE_RUNNING);
	
	SC_HANDLE hs = OpenService(// 打开服务
		schSCManager,          // 服务控件管理器数据库的句柄
		serviceName,           // 要打开的服务名
		SERVICE_START          // 服务访问权限
	);
	if (!hs)// 打开服务失败
		return FALSE;

	if (!StartService(hs, NULL, NULL))// 启动服务
	{
		CloseServiceHandle(hs);
		return FALSE;
	}
	CloseServiceHandle(hs);
	return WaitForPendingServiceChanged(schSCManager, serviceName, SERVICE_RUNNING);
}

KMDFAPI stopDrv(SC_HANDLE schSCManager, CONST WCHAR serviceName[NAME_SIZE])//驱动停止
{
	NTSTATUS drvStatus = queryDvr(schSCManager, serviceName);
	if (SERVICE_STOPPED == drvStatus)
		return TRUE;
	else if (SERVICE_STOP_PENDING == drvStatus)
		return WaitForPendingServiceChanged(schSCManager, serviceName, SERVICE_STOPPED);
	
	SC_HANDLE hs = OpenService(// 打开服务
		schSCManager,          // 服务控件管理器数据库的句柄
		serviceName,           // 要打开的服务名
		SERVICE_STOP           // 服务访问权限：所有权限
	);
	
	SERVICE_STATUS status;
	if (0 == ControlService(      // 发送关闭服务请求
		hs,                       // 服务句柄
		SERVICE_CONTROL_STOP,     // 控制码：通知服务应该停止
		&status                   // 接收最新的服务状态信息
	))
	{
		CloseServiceHandle(hs);
		return FALSE;
	}
	CloseServiceHandle(hs);
	return WaitForPendingServiceChanged(schSCManager, serviceName, SERVICE_STOPPED);
}

KMDFAPI uninstallDrv(SC_HANDLE schSCManager, CONST WCHAR serviceName[NAME_SIZE])//驱动卸载
{
	NTSTATUS drvStatus = queryDvr(schSCManager, serviceName);
	if (ERROR_SERVICE_DOES_NOT_EXIST == drvStatus)
		return TRUE;

	//if (IDNO == MessageBox(NULL, TEXT("卸载驱动存在一定风险，您确定要卸载吗？"), TEXT("GooseBt 驱动加载程序"), MB_YESNO | MB_ICONQUESTION | MB_TOPMOST))
	//	return FALSE;

	SC_HANDLE hs = OpenService(// 打开服务
		schSCManager,          // 服务控件管理器数据库的句柄
		serviceName,           // 要打开的服务名
		SERVICE_ALL_ACCESS     // 服务访问权限：所有权限
	);

	KMDFAPI bRet = (DeleteService(hs) ? TRUE : FALSE);// 删除服务
	CloseServiceHandle(hs);
	return bRet;
}

/* 显示帮助 */
void showHelp()
{
	wcout << endl << L"描述：GooseBt 驱动加载程序" << endl << endl;
	wcout << L"DriverLoader [首选项] [驱动文件路径] [...]" << endl << endl;
	wcout << L"可用首参数列表：" << endl;
	wcout << L"\tquery\t\t请求" << endl;
	wcout << L"\tinstall\t\t安装" << endl;
	wcout << L"\tstart\t\t启动" << endl;
	wcout << L"\tstop\t\t停止" << endl;
	wcout << L"\tuninstall\t卸载" << endl;
	wcout << L"\tdeploy\t\t部署" << endl;
	wcout << L"\tclean\t\t清理" << endl;
	wcout << L"\t/? 或 -?\t显示此帮助" << endl << endl;
	wcout << L"返回值说明：" << endl;
	wcout << L"\t" << EXIT_NOT_ADMIN << L"\t未处于提升权限下" << endl;
	wcout << L"\t" << EXIT_SCM_ERROR << L"\t控制台打开失败" << endl;
	wcout << L"\t" << EXIT_FAILURE << L"\t一个或多个驱动请求失败" << endl;
	wcout << L"\t" << EXIT_SUCCESS << L"\t操作成功结束" << endl;
	wcout << L"\t" << EOF << L"\t命令行参数不正确" << endl << endl << endl;
	wcout << L"注意事项：" << endl;
	wcout << L"\t本工具需要管理员权限，并需要获取服务控制台权限；" << endl;
	wcout << L"\t本工具以驱动文件的主文件名作为驱动名进行一切操作；" << endl;
	wcout << L"\t如驱动名存在，本工具将不会检查驱动的路径和文件内容；" << endl;
	wcout << L"\t若驱动已处于目标状态，对驱动的请求将一律返回操作成功。" << endl << endl;
	return;
}



/* 遵循 Microsoft Windows 命令行规则的 main 函数 */
int _tmain(int argc, _TCHAR* argv[])//主函数
{
	setlocale(LC_CTYPE, "");
	wcout.imbue(locale("chs", LC_CTYPE));
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);//更改优先级
	SetProcessPriorityBoost(GetCurrentProcess(), FALSE);//锁定优先级

	if (argc == 1)
	{
		if (FindFirstFileExists(CCharToLpcwstr((GF_GetEXEPath() + InstDrv).c_str()), FALSE))
		{
			if (!isAdmin())
				wcout << L"警告：可能没有足够的权限。" << endl;

			char cmd[MAX_PATH] = { 0 };
			strcpy_s(cmd, ("\"" + GF_GetEXEPath() + InstDrv + "\"").c_str());
			if (system(cmd) != 2)
				MessageBox(NULL, TEXT("初始化主面板失败！"), TEXT("GooseBt 驱动加载程序"), MB_OK | MB_ICONERROR | MB_TOPMOST);
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
	else if (argc == 2 && (
			0 == _wcsicmp(argv[1], L"/?")
			|| 0 == _wcsicmp(argv[1], L"-?")
			|| 0 == _wcsicmp(argv[1], L"/help")
			|| 0 == _wcsicmp(argv[1], L"-help")
		)
	)
	{
		showHelp();
		return EXIT_SUCCESS;
	}
	else if (argc <= 2)
	{
		wcout << L"错误：命令行参数太少！" << endl;
		return EOF;
	}

	if (!isAdmin())
	{
		wcout << L"访问被拒绝，因为你没有足够的权限。" << endl;
		wcout << L"你必须在提升模式运行本工具，并确保可获取服务管理控制台权限。" << endl;
		return EXIT_NOT_ADMIN;
	}

	SC_HANDLE schSCManager;
	if (!openSCM(schSCManager))
	{
		wcout << L"打开服务控件管理器失败！" << endl;
		MessageBox(NULL, TEXT("打开服务控件管理器失败！"), TEXT("GooseBt 驱动加载程序"), MB_OK | MB_ICONERROR | MB_TOPMOST);
		return EXIT_SCM_ERROR;
	}
	_TCHAR ptName[MAX_PATH] = { 0 }, args[MAX_NUM][MAX_PATH] = { { 0 } };
	int bRet = EXIT_SUCCESS;
	for (int i = 0; i < argc - 2 && i < MAX_NUM; ++i)
		lstrcpy(args[i], argv[i + 2]);
	if (_wcsicmp(argv[1], L"query") == 0 || _wcsicmp(argv[1], L"/query") == 0 || _wcsicmp(argv[1], L"-query") == 0)
		for (int i = 0; i < argc - 2; ++i)
		{
			if (!FillPath(args[i], ptName))
			{
				wcout << (i + 1) << L" -> " << L"错误：系统找不到指定文件——“" << argv[i + 2] << L"”。" << endl;
				bRet = EXIT_FAILURE;
				continue;
			}
			wcout << (i + 1) << L" -> " << args[i] << endl;
			NTSTATUS status = queryDvr(schSCManager, ptName);
			switch (status)
			{
			case ERROR_SERVICE_DOES_NOT_EXIST:
				wcout << (i + 1) << L" -> " << L"驱动未安装！" << endl;
				break;
			case NULL:
				wcout << (i + 1) << L" -> " << L"驱动存在，但打开驱动控制器失败！" << endl;
				break;
			case SERVICE_RUNNING:
				wcout << (i + 1) << L" -> " << L"驱动正在运行！" << endl;
				break;
			case SERVICE_START_PENDING:
				wcout << (i + 1) << L" -> " << L"驱动启动中！" << endl;
				break;
			case SERVICE_STOPPED:
				wcout << (i + 1) << L" -> " << L"驱动已停止！" << endl;
				break;
			case SERVICE_STOP_PENDING:
				wcout << (i + 1) << L" -> " << L"驱动停止中！" << endl;
				break;
			case SERVICE_PAUSED:
			case SERVICE_PAUSE_PENDING:
			case SERVICE_CONTINUE_PENDING:
				wcout << (i + 1) << L" -> " << L"所安装的服务不是驱动，因为它支持暂停和恢复运行！" << endl;
				break;
			default:
				wcout << (i + 1) << L" -> " << status << endl;
				break;
			}
		}
	else if (_wcsicmp(argv[1], L"install") == 0 || _wcsicmp(argv[1], L"/install") == 0 || _wcsicmp(argv[1], L"-install") == 0)
		for (int i = 0; i < argc - 2; ++i)
		{
			if (!FillPath(args[i], ptName))
			{
				wcout << (i + 1) << L" -> " << L"错误：系统找不到指定文件——“" << argv[i + 2] << L"”。" << endl;
				bRet = EXIT_FAILURE;
				continue;
			}
			wcout << (i + 1) << L" -> " << args[i] << endl;
			if (installDrv(schSCManager, args[i], ptName))
				wcout << (i + 1) << L" -> " << L"安装驱动 " << ptName << L" 成功！" << endl;
			else
			{
				wcout << (i + 1) << L" -> " << L"安装驱动 " << ptName << L" 失败！" << endl;
				bRet = EXIT_FAILURE;
			}
		}
	else if (_wcsicmp(argv[1], L"start") == 0 || _wcsicmp(argv[1], L"/start") == 0 || _wcsicmp(argv[1], L"-start") == 0)
		for (int i = 0; i < argc - 2; ++i)
		{
			if (!FillPath(args[i], ptName))
			{
				wcout << (i + 1) << L" -> " << L"错误：系统找不到指定文件——“" << argv[i + 2] << L"”。" << endl;
				bRet = EXIT_FAILURE;
				continue;
			}
			wcout << (i + 1) << L" -> " << args[i] << endl;
			if (startDrv(schSCManager, ptName))
				wcout << (i + 1) << L" -> " << L"启动驱动 " << ptName << L" 成功！" << endl;
			else
			{
				wcout << (i + 1) << L" -> " << L"启动驱动 " << ptName << L" 失败，您可以尝试重启后重新加载此驱动！" << endl;
				bRet = EXIT_FAILURE;
			}
		}
	else if (_wcsicmp(argv[1], L"stop") == 0 || _wcsicmp(argv[1], L"/stop") == 0)
		for (int i = 0; i < argc - 2; ++i)
		{
			if (!FillPath(args[i], ptName))
			{
				wcout << (i + 1) << L" -> " << L"错误：系统找不到指定文件——“" << argv[i + 2] << "”。" << endl;
				bRet = EXIT_FAILURE;
				continue;
			}
			wcout << (i + 1) << L" -> " << args[i] << endl;
			if (stopDrv(schSCManager, ptName))
				wcout << (i + 1) << L" -> " << L"停止驱动 " << ptName << L" 成功！" << endl;
			else
			{
				wcout << (i + 1) << L" -> " << L"停止驱动 " << ptName << L" 失败！" << endl;
				bRet = EXIT_FAILURE;
			}
		}
	else if (
		_wcsicmp(argv[1], L"uninstall") == 0 || _wcsicmp(argv[1], L"/uninstall") == 0 || _wcsicmp(argv[1], L"-uninstall") == 0
		|| _wcsicmp(argv[1], L"remove") == 0 || _wcsicmp(argv[1], L"/remove") == 0 || _wcsicmp(argv[1], L"-remove") == 0
		|| _wcsicmp(argv[1], L"delete") == 0 || _wcsicmp(argv[1], L"/delete") == 0 || _wcsicmp(argv[1], L"-delete") == 0
		|| _wcsicmp(argv[1], L"del") == 0 || _wcsicmp(argv[1], L"/del") == 0 || _wcsicmp(argv[1], L"-del") == 0
	)
		for (int i = 0; i < argc - 2; ++i)
		{
			if (!FillPath(args[i], ptName))
			{
				wcout << (i + 1) << L" -> " << L"错误：系统找不到指定文件——“" << argv[i + 2] << "”。" << endl;
				bRet = EXIT_FAILURE;
				continue;
			}
			wcout << (i + 1) << L" -> " << args[i] << endl;
			if (uninstallDrv(schSCManager, ptName))
				wcout << (i + 1) << L" -> " << L"卸载驱动 " << ptName << L" 成功！" << endl;
			else
			{
				wcout << (i + 1) << L" -> " << L"卸载驱动 " << ptName << L" 失败！" << endl;
				bRet = EXIT_FAILURE;
			}
		}
	else if (_wcsicmp(argv[1], L"deploy") == 0 || _wcsicmp(argv[1], L"/deploy") == 0 || _wcsicmp(argv[1], L"-deploy") == 0)
		for (int i = 0; i < argc - 2; ++i)
		{
			if (!FillPath(args[i], ptName))
			{
				wcout << (i + 1) << L" -> " << L"错误：系统找不到指定文件——“" << argv[i + 2] << L"”。" << endl;
				bRet = EXIT_FAILURE;
				continue;
			}
			wcout << (i + 1) << L" -> " << args[i] << endl;
			if (startDrv(schSCManager, ptName))
				wcout << (i + 1) << L" -> " << L"部署驱动环境 " << ptName << L" 成功！" << endl;
			else
			{
				stopDrv(schSCManager, ptName);
				uninstallDrv(schSCManager, ptName);
				installDrv(schSCManager, args[i], ptName);
				if (startDrv(schSCManager, ptName))
					wcout << (i + 1) << L" -> " << L"部署驱动环境 " << ptName << L" 成功！" << endl;
				else
				{
					wcout << (i + 1) << L" -> " << L"部署驱动环境 " << ptName << L" 失败！" << endl;
					bRet = EXIT_FAILURE;
				}
			}
		}
	else if (_wcsicmp(argv[1], L"clean") == 0 || _wcsicmp(argv[1], L"/clean") == 0 || _wcsicmp(argv[1], L"-clean") == 0)
		for (int i = 0; i < argc - 2; ++i)
		{
			if (!FillPath(args[i], ptName))
			{
				wcout << (i + 1) << L" -> " << L"错误：系统找不到指定文件——“" << argv[i + 2] << L"”。" << endl;
				bRet = EXIT_FAILURE;
				continue;
			}
			wcout << (i + 1) << L" -> " << args[i] << endl;
			stopDrv(schSCManager, ptName);
			if (uninstallDrv(schSCManager, ptName))
				wcout << (i + 1) << L" -> " << L"清理驱动环境 " << ptName << L" 成功！" << endl;
			else
			{
				wcout << (i + 1) << L" -> " << L"清理驱动环境 " << ptName << L" 失败！" << endl;
				bRet = EXIT_FAILURE;
			}
		}
	else if (_wcsicmp(argv[1], L"test") == 0 || _wcsicmp(argv[1], L"/test") == 0 || _wcsicmp(argv[1], L"-test") == 0)
		bRet = testsigningon((_wcsicmp(argv[2], L"on") == 0) ? TRUE : FALSE) ? EXIT_SUCCESS : EXIT_FAILURE;
	else
	{
		wcout << L"错误：命令行参数不正确！" << endl;
		bRet = EOF;
	}
	CloseServiceHandle(schSCManager);
	return bRet;
}