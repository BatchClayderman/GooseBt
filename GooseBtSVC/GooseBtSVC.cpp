#include <iostream>
#ifdef WIN32
#include <Windows.h>
#include <winsvc.h>
#else
#ifdef _WIN64
#include <Windows.h>
#include <winsvc.h>
#ifndef WIN32
#define WIN32 1
#endif
#endif
#endif//<winsvc.h>必须在<Windows.h>后
#include <TlHelp32.h>
#include <WtsApi32.h>
#include <tchar.h>
#include <thread>
#include <vector>
#ifndef _GooseBtSVC_H
#define _GooseBtSVC_H
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
#ifndef gapTime
#define gapTime 60000
#endif
#ifndef VOID
#define VOID void
#endif
#ifdef UNICODE
#ifndef GetParentDirectory
#define GetParentDirectory GetParentDirectoryW
#endif
#ifndef CatChildDirectory
#define CatChildDirectory CatChildDirectoryW
#endif
#else
#ifndef GetParentDirectory
#define GetCurrentDirectory GetParentDirectoryA
#endif
#ifndef CatChildDirectory
#define CatChildDirectory CatChildDirectoryA
#endif
#endif // !UNICODE
#ifdef SVCName
#undef SVCName
#endif//最多 20 个字符
#define SVCName "GooseBtSVC"
#ifndef LogName
#define LogName "GooseBtSVC.log"
#endif
#ifndef COMMAND1
#define COMMAND1 "wmip.exe"
#endif
#ifndef COMMAND2
#define COMMAND2 "GooseBtMonitor.exe"
#endif
#ifndef COMMAND3
#define COMMAND3 "SnowDream.exe"
#endif
#ifndef COMMAND4
#define COMMAND4 "GooseBtTray.exe"
#endif
#ifndef OPTION1
#define OPTION1 "Goose"
#endif
#ifndef OPTION2
#define OPTION2 ""
#endif
#ifndef OPTION3
#define OPTION3 ""
#endif
#endif//_GooseBtSVC_H
#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")//不显示窗口
#pragma comment(lib, "WtsApi32.lib")
using namespace std;


/* 全局变量 */
void KillService();//提前声明相互调用的函数
typedef int Status;
typedef DWORD(CALLBACK* NTTERMINATEPROCESS)(HANDLE, UINT);
typedef BOOL APIType;
NTTERMINATEPROCESS NtTerminateProcess;//必须使用全局变量
HANDLE hServiceThread, killServiceEvent;
_TCHAR strServiceName[20] = _T(SVCName);//标识服务的内部名
SERVICE_STATUS_HANDLE nServiceStatusHandle;//存储调用 RegisterServiceCtrlHandler 返回的句柄
BOOL nServiceRunning;
DWORD nServiceCurrentStatus, CID[4] = { NULL };
char _0[MAX_PATH], buffer[MAX_PATH];
unsigned long long int cTimer = 0;


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

string getLocalTime()//获取本地系统时间
{
#ifdef WIN32
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	char tmpStr[MAX_PATH] = { 0 };
	_itoa_s(sys.wYear, tmpStr, 10);
	string TimeStr = string(tmpStr) + "年";
	_itoa_s(sys.wMonth, tmpStr, 10);
	TimeStr += string(tmpStr) + "月";
	_itoa_s(sys.wDay, tmpStr, 10);
	TimeStr += string(tmpStr) + "日";
	switch (sys.wDayOfWeek)
	{
	case 1:
		TimeStr += "星期一";
		break;
	case 2:
		TimeStr += "星期二";
		break;
	case 3:
		TimeStr += "星期三";
		break;
	case 4:
		TimeStr += "星期四";
		break;
	case 5:
		TimeStr += "星期五";
		break;
	case 6:
		TimeStr += "星期六";
		break;
	case 0:
		TimeStr += "星期日";
		break;
	default:
		break;
	}
	_itoa_s(sys.wHour, tmpStr, 10);
	TimeStr += (sys.wHour < 10 ? "0" : "") + string(tmpStr) + ":";
	_itoa_s(sys.wMinute, tmpStr, 10);
	TimeStr += (sys.wMinute < 10 ? "0" : "") + string(tmpStr) + ":";
	_itoa_s(sys.wSecond, tmpStr, 10);
	TimeStr += (sys.wSecond < 10 ? "0" : "") + string(tmpStr) + ".";
	_itoa_s(sys.wMilliseconds, tmpStr, 10);
	if (sys.wMilliseconds < 10)
		TimeStr += "00";
	else if (sys.wMilliseconds < 100)
		TimeStr += "0";
	TimeStr += string(tmpStr) + "\t";
	return TimeStr;
#else
	return "";
#endif
}

BOOL LogDown(string str)//Log
{
	FILE* fp;
	fopen_s(&fp, (GF_GetEXEPath() + LogName).c_str(), "at+");
	if (fp == NULL)
		return FALSE;
	if (str[str.length() - 1] == '\n')
		fprintf_s(fp, (getLocalTime() + str).c_str());
	else
		fprintf_s(fp, (getLocalTime() + str + "\n").c_str());
	fclose(fp);
	return TRUE;
}


/* 目录处理 */
Status GetParentDirectoryW(_TCHAR* source, int _size_, _TCHAR* buffer)//获取父目录
{
	if (!source || !buffer)
		return ERROR;
	int i = lstrlen(source) - 1;
	if (source[i] == '\"')
		i--;
	if (source[i] == '\\')
		i--;
	for (; source[i] != '\\' && i >= 0; --i);
	if (i <= EOF || i + 1 > _size_)
		return ERROR;// buffer 太小
	for (int j = (source[0] == '\"' ? 1 : 0); j < i; ++j)
		buffer[j] = source[j];
	return OK;
}

Status GetParentDirectoryA(char* source, size_t _size_, char* buffer)//获取父目录
{
	if (!source || !buffer)
		return ERROR;
	size_t i = strlen(source) - 1;
	if (source[i] == '\"')
		i--;
	if (source[i] == '\\')
		i--;
	for (; source[i] != '\\' && i >= 0; --i);
	if (i <= ERROR /* 一定不能用 EOF */ || i + 1 > _size_)
		return ERROR;// buffer 太小
	for (size_t j = (source[0] == '\"' ? 1 : 0); j < i; ++j)
		buffer[j] = source[j];
	return OK;
}

Status CatChildDirectoryW(_TCHAR* source1, _TCHAR* source2, int _size_, _TCHAR* buffer)//连接子目录
{
	if (!source1 || !source2 || !buffer)
		return ERROR;
	if (lstrlen(source1) + lstrlen(source2) >= _size_)
		return ERROR;// buffer 太小
	lstrcpy(buffer, source1);
	lstrcat(buffer, L"\\");
	lstrcat(buffer, source2);
	return OK;
}

Status CatChildDirectoryA(char* source1, const char* source2, size_t _size_, char* buffer)//连接子目录
{
	if (!source1 || !source2 || !buffer)
		return ERROR;
	if (strlen(source1) + strlen(source2) >= _size_)
		return ERROR;// buffer 太小
	strcpy_s(buffer, _size_, source1);
	strcat_s(buffer, _size_, "\\");
	strcat_s(buffer, _size_, source2);
	return OK;
}


/* APIType */
APIType SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)//调整远程进程权限
{
	BOOL bRet = FALSE;
	LUID luid;
	TOKEN_PRIVILEGES tp;
	bRet = LookupPrivilegeValue(NULL, lpszPrivilege, &luid);
	if (!bRet)
		return bRet;
	tp.PrivilegeCount = 1;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = NULL;
	bRet = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL);
	if (!bRet)
		return bRet;
	return TRUE;
}

APIType EnableDebugPriv(const _TCHAR* name)//本地进程提权函数
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))//打开进程令牌环
		return FALSE;
	if (!LookupPrivilegeValue(NULL, name, &luid))//获取进程 token 权限信息
		return FALSE;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = luid;
	if (!AdjustTokenPrivileges(hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))//调整权限
		return FALSE;
	return TRUE;
}

APIType KillProcess(DWORD PID)//结束进程
{
	if (!EnableDebugPriv(L"SeDebugPrivilege"))
		return FALSE;
	HANDLE hdle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, PID);
	if (hdle && TerminateProcess(hdle, NULL) == 1)
	{
		CloseHandle(hdle);
		return TRUE;
	}
	else
		return FALSE;
}

APIType GetTokenByName(HANDLE& hToken, LPSTR lpName)
{
	if (!lpName)
		return FALSE;
	HANDLE hProcessSnap = NULL;
	BOOL bRet = FALSE;
	PROCESSENTRY32 pe32 = { 0 };
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return (FALSE);
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			char tmpChars[MAX_PATH] = { 0 };
			TcharToChar(pe32.szExeFile, tmpChars);
			_strupr_s(tmpChars);
			if (!strcmp(tmpChars, lpName))
			{
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
					FALSE, pe32.th32ProcessID);
				bRet = OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
				CloseHandle(hProcessSnap);
				return (bRet);
			}
		} while (Process32Next(hProcessSnap, &pe32));
		bRet = TRUE;
	}
	else
		bRet = FALSE;
	CloseHandle(hProcessSnap);
	return (bRet);
}

APIType RunProcessAsCUser(LPCSTR lpImage, LPCSTR commandLine)
{
	if (!lpImage && !commandLine)
		return FALSE;
	HANDLE hToken;
	char tmpChars[MAX_PATH] = "EXPLORER.EXE";//必须大写
	char tmpLine[MAX_PATH] = { 0 };
	if (!GetTokenByName(hToken, tmpChars))
		return FALSE;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop = CCharToLpcwstr("winsta0\\default");
	if (lpImage)
		strcpy_s(tmpChars, lpImage);
	if (commandLine)
		strcpy_s(tmpLine, commandLine);
	BOOL bResult = CreateProcessAsUser(
		hToken,
		lpImage ? CharToLpcwstr(tmpChars) : NULL,
		commandLine ? CharToLpcwstr(tmpLine) : NULL,
		NULL,
		NULL,
		FALSE,
		SW_HIDE | BELOW_NORMAL_PRIORITY_CLASS,
		NULL,
		CCharToLpcwstr(GF_GetEXEPath().c_str()),
		&si,
		&pi
	);
	if (lpImage)//是否为八月飞雪启动
		CID[2] = pi.dwProcessId;
	CloseHandle(hToken);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return bResult;
}

APIType isEnoughUser()//是否拥有足够的非 SYSTEM 用户
{
	DWORD dwReserved = 0;
	DWORD dwVersion = 1;
	WTS_SESSION_INFO* ppSessionInfo = NULL;
	DWORD dwSessionCount;
	DWORD UserCount = 0;

	BOOL result = WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE,
		dwReserved,
		dwVersion,
		&ppSessionInfo,
		&dwSessionCount
	);
	for (DWORD nSessionIndex = 0; nSessionIndex < dwSessionCount; ++nSessionIndex)
	{
		BOOL bResult = FALSE;
		LPWSTR  pBuffer = NULL;
		PWTSINFO  pWtsinfo = NULL;
		DWORD dwBufferLen = 0;
		bResult = WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE,
			ppSessionInfo[nSessionIndex].SessionId,
			WTSUserName,
			&pBuffer,
			&dwBufferLen);
		if (bResult)
		{
			if (wcslen(pBuffer) != 0 && lstrcmp(pBuffer, L"SYSTEM") != 0)
				UserCount++;
			WTSFreeMemory(pBuffer);
		}
	}
	if (UserCount >= 1)
		return TRUE;
	else
		return FALSE;
}


/* 多线程部分 */
void Timing()
{
	while (nServiceRunning)
	{
		Sleep(gapTime);
		cTimer++;
	}
	return;
}

void StartChild1()//wmip goose
{
	Sleep(gapTime);//开机停滞（为迎合驱动保护交由 wmip 接管）
	//return;
	while (!isEnoughUser())
		Sleep(gapTime >> 1);
	STARTUPINFOA si1;
	PROCESS_INFORMATION pi1;
	ZeroMemory(&pi1, sizeof(pi1));
	ZeroMemory(&si1, sizeof(si1));
	si1.cb = sizeof(STARTUPINFOA);
	char cmd1[MAX_PATH << 2] = { 0 };
	strcpy_s(cmd1, ("\"" + GF_GetEXEPath() + COMMAND1 + "\" " + OPTION1).c_str());
	BOOL Working1 = ::CreateProcessA(NULL, cmd1, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, GF_GetEXEPath().c_str(), &si1, &pi1);
	if (Working1)
	{
		CID[0] = pi1.dwProcessId;
		string tmpStr = "子进程 GooseBtMonitor.exe 已启动，其进程 ID 为 ";
		char tmpChars[MAX_PATH] = { 0 };
		_itoa_s(pi1.dwProcessId, tmpChars, 10);
		tmpStr += tmpChars;
		LogDown(tmpStr);
		WaitForSingleObject(pi1.hProcess, INFINITE);
		LogDown("子进程 wmip.exe 已退出！");
		CID[0] = NULL;//短暂归还
		if (nServiceRunning)
		{
			Sleep(gapTime);
			StartChild1();
		}
	}
	else
		MessageBox(NULL, TEXT("进程监控程序未启动，进程防御未开启！"), TEXT("GooseBt 第四代反病毒软件"), MB_OK | MB_ICONERROR | MB_TOPMOST);
	CloseHandle(pi1.hProcess);//记得关闭句柄
	CloseHandle(pi1.hThread);
	return;
}

void StartChild2()//GooseBtMonitor
{
	STARTUPINFOA si2;
	PROCESS_INFORMATION pi2;
	ZeroMemory(&pi2, sizeof(pi2));
	ZeroMemory(&si2, sizeof(si2));
	si2.cb = sizeof(STARTUPINFOA);
	char cmd2[MAX_PATH << 2] = { 0 };
	strcpy_s(cmd2, ("\"" + GF_GetEXEPath() + COMMAND2 + "\" " + OPTION2).c_str()); 
	BOOL Working2 = ::CreateProcessA(NULL, cmd2, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, GF_GetEXEPath().c_str(), &si2, &pi2);
	if (Working2)
	{
		CID[1] = pi2.dwProcessId;
		string tmpStr = "子进程 GooseBtMonitor.exe 已启动，其进程 ID 为 ";
		char tmpChars[MAX_PATH] = { 0 };
		_itoa_s(pi2.dwProcessId, tmpChars, 10);
		tmpStr += tmpChars;
		LogDown(tmpStr);
		WaitForSingleObject(pi2.hProcess, INFINITE);
		LogDown("子进程 GooseBtMonitor.exe 已退出！");
		CID[1] = NULL;//短暂归还
		if (nServiceRunning)
		{
			Sleep(gapTime);
			StartChild2();
		}
	}
	else
		MessageBox(NULL, TEXT("文件监控程序未启动，文件防御未开启！"), TEXT("GooseBt 第四代反病毒软件"), MB_OK | MB_ICONERROR | MB_TOPMOST);
	CloseHandle(pi2.hProcess);//记得关闭句柄
	CloseHandle(pi2.hThread);
	return;
}

void StartChild3()// SnowDream
{
	while (!isEnoughUser())
		Sleep(gapTime >> 1);
	if (RunProcessAsCUser((GF_GetEXEPath() + COMMAND3).c_str(), NULL))
	{
		string tmpStr = "视觉效果 SnowDream 已启动，其进程 ID 为 ";
		char tmpChars[MAX_PATH] = { 0 };
		_itoa_s(CID[2], tmpChars, 10);
		tmpStr += tmpChars;
		tmpStr += "，此进程不作退出记录。";
		LogDown(tmpStr);
	}
	return;
}

void StartChild4()// Tray
{
	while (!isEnoughUser())
		Sleep(gapTime >> 1);
	if (RunProcessAsCUser((GF_GetEXEPath() + COMMAND4).c_str(), NULL))
	{
		string tmpStr = "托盘图标已启动，其进程 ID 为 ";
		char tmpChars[MAX_PATH] = { 0 };
		_itoa_s(CID[3], tmpChars, 10);
		tmpStr += tmpChars;
		tmpStr += "，此进程不作退出记录。";
		LogDown(tmpStr);
	}
	return;
}

/* 服务部分 */
//向 SCM 报告服务状态信息，可以说是更新信息，它接受的参数都是 SERVICE_STATUS 结构成员
BOOL ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwServiceSpecificExitCode, DWORD dwCheckPoint, DWORD dwWaitHint)
{
	BOOL success;
	SERVICE_STATUS nServiceStatus;//定义一个 SERVICE_STATUS 类型结构 nServiceStatus
	nServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;//表示我们的服务是独占一个进程的服务
	nServiceStatus.dwCurrentState = dwCurrentState; //当前服务状态
	if (dwCurrentState == SERVICE_START_PENDING)
		nServiceStatus.dwControlsAccepted = 0; //服务的初始化没有完成
	else
		nServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN; //通知 SCM 服务接受哪个域。这里允许 STOP 和SHUTDOWN 请求
	//dwServiceSpecificExitCode在你终止服务并报告退出细节时很有用。初始化服务时并不退出，因此值为 0
	if (dwServiceSpecificExitCode == 0)
		nServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
	else
		nServiceStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
	nServiceStatus.dwServiceSpecificExitCode = dwServiceSpecificExitCode;
	nServiceStatus.dwCheckPoint = dwCheckPoint;
	nServiceStatus.dwWaitHint = dwWaitHint;
	//设置好nServiceStatus后,向SCM报告服务状态
	success = SetServiceStatus(nServiceStatusHandle, &nServiceStatus);
	if (!success)
	{
		KillService();
		return success;
	}
	else
		return success;
}

void KillService()//停止服务
{
	nServiceRunning = false;
	SetEvent(killServiceEvent);
	if (CID[2])
	{
		string closeWOp = ("mshta vbscript:createobject(\"wscript.shell\").run(\"\"\"" + GF_GetEXEPath() + "wmip.exe\"\" Terminate /w /pid ").c_str();
		//string closeWOp = "mshta vbscript:createobject(\"wscript.shell\").run(\"taskkill /pid ";
		//string closeWOp = "taskkill /pid ";
		//string closeWOp = ("\"" + GF_GetEXEPath() + "wmip.exe\" Terminate /w /pid ").c_str();
		char tmpPID[20] = { 0 };
		_itoa_s(CID[2], tmpPID, 10);
		closeWOp += tmpPID;
		closeWOp += "\",0)(window.close)";
		RunProcessAsCUser(NULL, closeWOp.c_str());
		//CID[2] = NULL;
	};
	Sleep(gapTime >> 5);
	for (int i = 0; i < 3; ++i)
		if (CID[i])
			KillProcess(CID[i]);
#ifndef _WIN64
	system("taskkill /im cmd.exe /im conhost.exe /im GooseBtMonitor.exe /im SnowDream.exe /f /t");
#endif
	LogDown("服务已停止！");
	ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0, 0, 0);
	return;
}

DWORD GooseBtSVC(LPDWORD param)//我们这个服务所要完成的任务
{
	/* 初始化文件指针并创建线程 */
	FILE* Fces;
	fopen_s(&Fces, buffer, "at+");//全局写入
	if (Fces)
	{
		fprintf(Fces, "服务进程启动，其 PID 为 %u。\n", (unsigned int)GetCurrentProcessId());
		fclose(Fces);
	}
	else
	{
		MessageBox(NULL, TEXT("服务无法记录日志，请检查服务状态。"), TEXT("服务故障"), MB_OK | MB_ICONWARNING | MB_TOPMOST);
		return EOF;
	}
	
	/* 运行两个子进程并获得 PID */
	thread t0(Timing), t1(StartChild1), t2(StartChild2), t3(StartChild3);

	/* 服务正常运行时 */
	while (nServiceRunning)
	{
		Sleep(gapTime);
		if (cTimer)
			fprintf(Fces, "服务正在运行，当前服务已运行 %llu 分钟。\n", cTimer);
		else
			fprintf(Fces, "从服务控制台返回了一个状态信息：服务已启动。\n");
	}
	t0.join();
	t1.join();
	t2.join();
	t3.join();
	return EXIT_SUCCESS;
}

BOOL InitThread()//创建线程来运行我们的任务
{
	DWORD id;
	hServiceThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)GooseBtSVC, 0, 0, &id);
	if (hServiceThread == 0)
		return FALSE;
	else
	{
		nServiceRunning = TRUE;
		return TRUE;
	}
}

void ServiceCtrlHandler(DWORD dwControlCode)
{
	BOOL success;
	switch (dwControlCode)
	{
	case SERVICE_CONTROL_SHUTDOWN:
		break;
	case SERVICE_CONTROL_STOP:
		nServiceCurrentStatus = SERVICE_STOP_PENDING;
		success = ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0, 1, 3000);//先更新服务状态为 SERVICDE_STOP_PENDING 再停止服务
		KillService();
		return;
	default:
		break;
	}
	ReportStatusToSCMgr(nServiceCurrentStatus, NO_ERROR, 0, 0, 0);
	return;
}

void ServiceMain(DWORD argc, LPTSTR* argv)//服务主函数
{
	BOOL success;
	
	/* 把 ServiceCtrlHandler 注册为服务控制器以接受来自 SCM 的请求并做出处理 */
	nServiceStatusHandle = RegisterServiceCtrlHandler(strServiceName, (LPHANDLER_FUNCTION)ServiceCtrlHandler);
	if (!nServiceStatusHandle)//判断是否注册成功
		return;

	/* 注册成功后向 SCM 报告服务状态信息，因为服务还没初始化完成，所以当前服务状态为 SERVICE_START_PENDING */
	success = ReportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 0, 1, 3000);
	if (!success)
		return;
	
	/* 创建一个事件，在函数的最后将用该事件来保持函数的运行直到SCM发出停止请求才返回 */
	killServiceEvent = CreateEvent(0, TRUE, FALSE, 0);
	if (killServiceEvent == NULL)
		return;
	success = ReportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 0, 2, 1000);//向 SCM 报告服务状态信息
	if (!success)
		return;
	nServiceCurrentStatus = SERVICE_RUNNING;//服务正式运行，当前状态设置为 SERVICE_RUNNING
	success = ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0, 0, 0);
	if (!success)
		return;

	/* 创建线程以支持 GooseBtSVC() 函数的运行 */
	success = InitThread();
	if (!success)
		return;
	
	/* GooseBtSVC() 函数运行完了之后返回 ServiceMain()，ServiceMain() 调用 WaitForSingleObject，因为服务被停止之前 ServiceMain() 不会结束 */
	WaitForSingleObject(killServiceEvent, INFINITE);
	CloseHandle(killServiceEvent);
	return;
}


/* main 函数 */
int main(int argc, char* argv[])
{
	setlocale(LC_CTYPE, "");
	GetModuleFileNameA(NULL, _0, MAX_PATH);
	GetParentDirectoryA(_0, MAX_PATH, buffer);
	if (CatChildDirectoryA(buffer, LogName, MAX_PATH, buffer) == ERROR)
	{
		MessageBox(NULL, TEXT("错误：获取 log 绝对路径失败，请检查服务，并按任意键退出。"), TEXT("服务"), MB_OK | MB_ICONERROR | MB_TOPMOST);
		return EXIT_FAILURE;
	}
	SERVICE_TABLE_ENTRY servicetable[] = { {strServiceName,(LPSERVICE_MAIN_FUNCTION)ServiceMain},{NULL,NULL} };
	BOOL success = StartServiceCtrlDispatcher(servicetable);// StartServiceCtrlDispatcher 函数负责把程序主线程连接到服务控制管理程序
	if (!success)
	{
		MessageBox(NULL, TEXT("错误：从服务启动程序失败，程序仅允许从服务控制台启动。"), TEXT("服务"), MB_OK | MB_ICONERROR | MB_TOPMOST);
		return EOF;
	}
	return EXIT_SUCCESS;
}