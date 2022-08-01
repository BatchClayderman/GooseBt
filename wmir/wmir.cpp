#include <iostream>
#ifdef WIN32
#include <Windows.h>
#include <Winternl.h>
#elif (defined _WIN64 || defined WIN64)
#include <Windows.h>
#include <Winternl.h>
#ifndef _WIN32
#define _WIN32 1
#endif
#ifndef WIN32
#define WIN32 1
#endif
#endif//_Windows_H
#include <tchar.h>
#include <shlwapi.h>
#include <tchar.h>
#include <atlbase.h>
#include <vector>
#include <map>
#include <string>
#include <psapi.h>
#ifndef _wmif_H
#define _wmif_H
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EOF
#define EOF (-1)
#endif
#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof((array)[0]))
#endif 
#ifdef UNICODE
#define GetFilePathFromHandle GetFilePathFromHandleW
#else
#define GetFilePathFromHandle GetFilePathFromHandleA
#endif
#ifdef NT_SUCCESS
#undef NT_SUCCESS
#endif
#define NT_SUCCESS(status)                   (status == (NTSTATUS)0x00000000L)
#define STATUS_INFO_LENGTH_MISMATCH          ((NTSTATUS)0xC0000004L)
#define STATUS_BUFFER_OVERFLOW               ((NTSTATUS)0x80000005L)
#define FileNameInformation                  ((FILE_INFORMATION_CLASS)9)
#define SystemHandleInformation              ((SYSTEM_INFORMATION_CLASS)16)
#endif//_whif_H
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shlwapi")
#pragma warning(disable: 6031)
#pragma warning(disable: 6258)
#pragma warning(disable: 6387)
using namespace std;
typedef BOOL APIType;
typedef BOOL(_stdcall* WOW64_DISABLE_FSDIR)(PVOID*);
typedef BOOL(_stdcall* WOW64_REVERT_FSDIR) (PVOID);
typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR>> tstring;
EXTERN_C BOOL GetFilePathFromHandleW(HANDLE hFile, LPWSTR lpszPath, UINT cchMax);
EXTERN_C BOOL GetFilePathFromHandleA(HANDLE hFile, LPSTR  lpszPath, UINT cchMax);


/* 驱动通讯预处理 */
#ifndef DriverConnection_H
#define DriverConnection_H
#ifndef CTL_CODE
#define CTL_CODE(DeviceType, Function, Method, Access) (((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))
#endif
#ifdef CWK_DEV_SYM_R//重要
#undef CWK_DEV_SYM_R
#endif
#define CWK_DEV_SYM_R L"\\\\.\\GooseBtZwOpReg"

/* 从应用层给驱动发送一个字符串 */
#define CWK_DVC_SEND_STR     \
	(ULONG)CTL_CODE(         \
	0x00000022,              \
	0x911,0,                 \
	FILE_WRITE_DATA          \
)

/* 从驱动读取一个字符串 */
#define CWK_DVC_RECV_STR       \
	(ULONG)CTL_CODE(           \
	FILE_DEVICE_UNKNOWN,       \
	0x912,METHOD_BUFFERED,     \
	FILE_READ_DATA             \
)
#endif//DriverConnection_H


/* 与驱动通信函数 */
WCHAR* CCharToLpcwstr(const char* strings)//转换宽字符
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

int DriverConnector(_TCHAR* msg, const _TCHAR* PipeName)
{
	HANDLE device = NULL;
	ULONG ret_len;
	int ret = EXIT_SUCCESS;

	/* 打开设备，每次要操作驱动对象时，先以此为例子打开设备 */
	device = CreateFileW(PipeName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
	if (device == INVALID_HANDLE_VALUE)
	{
		wcout << L"错误：与驱动通讯时发生异常！（" << msg << L"）" << endl;
		return EXIT_FAILURE;
	}

	/* 发送指令 */
	char tmp[MAX_PATH] = { 0 };
	TcharToChar(msg, tmp);
	if (DeviceIoControl(device, CWK_DVC_SEND_STR, tmp, (DWORD)(strlen(tmp)), NULL, 0, &ret_len, 0))
		wcout << L"向驱动发送操作文件“" << msg << L"”请求成功！" << endl;
	else
	{
		cout << "\a错误：向驱动发送信息时发生异常！（" << msg << "）" << endl;
		ret = EXIT_FAILURE;
	}

	CloseHandle(device);//记得关闭
	return ret;
}


/* 帮助 */
void addHelp()
{
	return;
}

void delHelp()
{
	return;
}

void setHelp()
{
	return;
}

void queryHelp()
{
	return;
}

void showHelp()
{
	wcout << endl << L"描述：综合注册表管理器。" << endl;
	wcout << endl << L"wmir [首参数] [注册表路径] [子项或子键] ..." << endl << endl;
	wcout << L"可用首参数列表：" << endl;
	wcout << L"\tadd\t\t增" << endl;
	wcout << L"\tdel\t\t删" << endl;
	wcout << L"\tset\t\t改" << endl;
	wcout << L"\tquery\t\t查" << endl << endl;
	wcout << L"更多信息，请键入“wmir [首参数] /?”" << endl << endl;
	return;
}



/* 遵循 Microsoft Windows 命令行规则的 main 函数 */
int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_CTYPE, "");
	wcout.imbue(locale("chs", LC_CTYPE));
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);//更改优先级
	SetProcessPriorityBoost(GetCurrentProcess(), FALSE);//锁定优先级

	if (argc >= 4)//参数数量正确
	{
		if (0 == _wcsicmp(argv[1], L"add")
			|| 0 == _wcsicmp(argv[1], L"del")
			|| 0 == _wcsicmp(argv[1], L"set")
			|| 0 == _wcsicmp(argv[1], L"query")
		)//攻击类
		{
			;
		}
	}
	else if (3 == argc
		&& (
			0 == _wcsicmp(argv[1], L"/?") || 0 == _wcsicmp(argv[1], L"-?")
			|| 0 == _wcsicmp(argv[1], L"/help") || 0 == _wcsicmp(argv[1], L"-help")
		)
	)//子帮助类
	{
		if (0 == _wcsicmp(argv[1], L"add"))
			addHelp();
		else if (0 == _wcsicmp(argv[1], L"del"))
			delHelp();
		else if (0 == _wcsicmp(argv[1], L"set"))
			setHelp();
		else if (0 == _wcsicmp(argv[1], L"query"))
			queryHelp();
		else
		{
			wcout << L"\a错误：命令行参数不正确！" << endl;
			return EOF;
		}
		return EXIT_SUCCESS;
	}
	else if (1 == argc
		|| (2 == argc
			&& (
				0 == _wcsicmp(argv[1], L"/?") || 0 == _wcsicmp(argv[1], L"-?")
				|| 0 == _wcsicmp(argv[1], L"/help") || 0 == _wcsicmp(argv[1], L"-help")
			)
		)
	)//帮助类
	{
		showHelp();
		return EXIT_SUCCESS;
	}
	else if (2 == argc && 0 == _wcsicmp(argv[1], L"test"))//测试类
	{
		DriverConnector(CCharToLpcwstr("test"), CWK_DEV_SYM_R);
		return EXIT_SUCCESS;
	}
	else
	{
		wcout << L"\a错误：命令行参数不正确！" << endl;
		return EOF;
	}
}