#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <time.h>
#include <tchar.h>

#ifdef WIN32
#include <Windows.h>
#else
#ifdef _WIN64
#include <Windows.h>
#ifndef WIN32
#define WIN32 1
#endif
#endif
#endif//Windows.h

#ifndef _Scanner_H
#define _Scanner_H
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
#ifndef OK
#ifdef TRUE
#define OK TRUE
#else
#define OK 1
#endif
#endif
#ifndef ERROR
#ifdef FALSE
#define ERROR FALSE
#else
#define ERROR 0
#endif
#endif
#ifdef PLF
#undef PLF
#endif
#define PLF TRUE//平台检测
#ifndef IDTRYAGAIN
#undef PLF
#define PLF FALSE
#endif
#ifndef IDCONTINUE
#undef PLF
#define PLF FALSE
#endif
#ifndef MB_CANCELTRYCONTINUE
#undef PLF
#define PLF FALSE
#endif
#if (PLF == FALSE)
#undef IDTRYAGAIN
#undef IDCONTINUE
#undef MB_CANCELTRYCONTINUE//去除所有声明
#ifdef IDABORT
#define IDTRYAGAIN IDRETRY
#else
#define IDTRYAGAIN 10
#endif
#ifdef IDRETRY
#define IDCONTINUE IDIGNORE
#else
#define IDCONTINUE 11
#endif
#ifdef MB_ABORTRETRYIGNORE
#define MB_CANCELTRYCONTINUE MB_ABORTRETRYIGNORE
#else
#define MB_CANCELTRYCONTINUE 0x00000006L
#endif
#endif//如果运行平台不支持 MB_CANCELTRYCONTINUE 及其衍生物，转用 MB_ABORTRETRYIGNORE
#ifdef PLF
#undef PLF
#endif
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#ifndef LIMIT
#define LIMIT 10
#endif//最大文件数
#ifndef LOW
#define LOW 60
#endif//最低值
#ifndef DEFAULTLOW
#define DEFAULTLOW 80
#endif//默认低风险值
#ifndef DEFAULTMIDDLE
#define DEFAULTMIDDLE 90
#endif//默认高风险值
#ifndef DEFAULTHIGH
#define DEFAULTHIGH 95
#endif//默认自动清除值
#ifndef MaxLen
#define MaxLen 30
#endif
#ifndef pause
#define pause 3000
#endif//暂停时间
#ifndef White
#define White 0xF
#endif//默认输出
#ifndef Gray
#define Gray 0x8
#endif//时间输出
#ifndef Blue
#define Blue 0x9
#endif//未发现风险
#ifndef Orange
#define Orange 0x6
#endif//低风险播报
#ifndef Yellow
#define Yellow 0xE
#endif//高风险播报
#ifndef Green
#define Green 0xA
#endif//已自动清除
#ifndef Red
#define Red 0xC
#endif//清除失败！
#ifndef F2_MemReq
#define F2_MemReq 1700
#endif//功能二需要的内存（单位：MB）
#ifndef F6_MemReq
#define F6_MemReq 600
#endif//功能六需要的内存（单位：MB）
#ifndef OBJ_CASE_INSENSITIVE
#define OBJ_CASE_INSENSITIVE 0x00000040L
#endif
#ifndef InitializeObjectAttributes
#define InitializeObjectAttributes( p, n, a, r, s )     \
{                                                       \
	(p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
	(p)->RootDirectory = r;                             \
	(p)->Attributes = a;                                \
	(p)->ObjectName = n;                                \
	(p)->SecurityDescriptor = s;                        \
	(p)->SecurityQualityOfService = NULL;               \
}
#endif
#if defined(_MSC_VER)
#define JL_SIZE_T_SPECIFIER "%Iu"
#define JL_SSIZE_T_SPECIFIER "%Id"
#define JL_PTRDIFF_T_SPECIFIER "%Id"
#elif defined(__GNUC__)
#define JL_SIZE_T_SPECIFIER "%zu"
#define JL_SSIZE_T_SPECIFIER "%zd"
#define JL_PTRDIFF_T_SPECIFIER "%zd"
#else//计算出使用的 size_t 及其衍生物应该使用的 % 符
#if NUMBITS == 32
#define JL_SIZE_T_SPECIFIER something_unsigned
#define JL_SSIZE_T_SPECIFIER something_signed
#define JL_PTRDIFF_T_SPECIFIER something_signed
#else
#define JL_SIZE_T_SPECIFIER something_bigger_unsigned
#define JL_SSIZE_T_SPECIFIER something_bigger_signed
#define JL_PTRDIFF_T_SPECIFIER something-bigger_signed
#endif
#endif
/* 驱动通讯预处理 */
#ifndef DriverConnection_H
#define DriverConnection_H
#ifndef CTL_CODE
#define CTL_CODE(DeviceType, Function, Method, Access) (  ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))
#endif
#ifdef CWK_DEV_SYM//重要
#undef CWK_DEV_SYM
#endif
#define CWK_DEV_SYM L"\\\\.\\GooseBtZwDeleteFile"

/* 从应用层给驱动发送一个字符串 */
#define  CWK_DVC_SEND_STR     \
    (ULONG)CTL_CODE(          \
    0x00000022,               \
    0x911,0,                  \
    FILE_WRITE_DATA)

/* 从驱动读取一个字符串 */
#define  CWK_DVC_RECV_STR     \
    (ULONG)CTL_CODE(          \
    FILE_DEVICE_UNKNOWN,      \
    0x912,METHOD_BUFFERED,    \
    FILE_READ_DATA)
#endif//DriverConnection_H
#endif//_Scanner_H
using namespace std;//挂载命名空间 std

/* 抽象数据类型和全局变量 */
typedef void Function;
typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
typedef BOOL(_stdcall* WOW64_DISABLE_FSDIR)(PVOID*);
typedef BOOL(_stdcall* WOW64_REVERT_FSDIR) (PVOID);
typedef int Status;
typedef char ElemType;
bool Outing = false;//是否在输出中
char Nowpath[MAX_PATH] = { 0 };//当前路径
char save_path[MAX_PATH] = { 0 };//结果输出路径;
char szPath[MAX_PATH] = { 0 };
char result[MAX_PATH] = { 0 };
size_t isVirus[3] = { DEFAULTLOW,DEFAULTMIDDLE,DEFAULTHIGH };//低风险  高风险  自动清除
Status Scanning = 0;//扫描状态    停止  暂停  运行  完成
FILE* pf_path_file;//全局文件指针变量
vector<char> _buf;//对比样本二进制
HHOOK hHook;


/* 时间结构体 */
struct HMS//时间结构体
{
	int hour;//时
	int minute;//分
	int second;//秒
};
HMS Tk;

HMS Translate()//转录为时间结构体
{
	Tk.hour += int(Tk.second / 3600);//时
	Tk.minute += int(Tk.second % 3600 / 60);//分
	Tk.second = Tk.second % 60;//秒
	return Tk;
}

void InitTimer()//初始化计时器
{
	Tk.hour = 0;
	Tk.minute = 0;
	Tk.second = 1;//并发编程滞后性
}


/* 输出与显示 */
void echo(string str, short int x)//输出流
{
	while (Outing);
	Outing = true;
	if (x >= 0x0 && x <= 0xF)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), x);
	cout << str << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), White);//归还默认色
	Outing = false;
	return;
}

void TimeUpdate()//更新输出
{
	string str = "—— ";
	if (Tk.hour != 0)
		str += (Tk.hour < 10 ? "0" : "") + to_string(Tk.hour) + ":";
	str += (Tk.minute < 10 ? "0" : "") + to_string(Tk.minute) + ":" + (Tk.second < 10 ? "0" : "") + to_string(Tk.second) + " ——";
	echo(str, Gray);
}


/* 文件路径录入 */
BOOL ifExistFile(LPCTSTR lpPath, BOOL dwFilter)//检查文件是否存在
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(lpPath, &fd);
	BOOL bFilter = (FALSE == dwFilter) ? TRUE : fd.dwFileAttributes & dwFilter;
	BOOL RetValue = ((hFind != INVALID_HANDLE_VALUE) && bFilter) ? TRUE : FALSE;
	FindClose(hFind);
	return RetValue;
}

string getPath(string tips, bool allowdir)
{
	string str;
	for (;;)//无条件循环
	{
		cout << "提示：路径两端有无引号均被本程序支持，若想取消输入，请直接回车。" << endl;
		cout << tips << endl;
		rewind(stdin);
		getline(cin, str);
		if (str.length() == 0)
		{
			rewind(stdin);
			return "";
		}
		while (str.find("\"") != str.npos)
			str.erase(remove(str.begin(), str.end(), '\"'));
		if (str[str.length() - 1] == '\\')
			str.pop_back();
		if (allowdir && str == "*")
			return str;
		_TCHAR wc[MAX_PATH] = { 0 };
#ifdef UNICODE
		_stprintf_s(wc, MAX_PATH, _T("%S"), str.c_str());//%S宽字符
#else
		_stprintf_s(wc, MAX_PATH, _T("%s"), str.c_str());//%s单字符
#endif
		if (allowdir && (((wc[0] >= 'A' && wc[0] <= 'Z') || (wc[0] >= 'a' && wc[0] <= 'z')) && wc[1] == ':'))//支持软盘 AB
			return str;
		else
		{
			if (ifExistFile(wc, FALSE))
				break;
			else
				cout << endl << "\a错误：系统找不到指定文件或目录，请检测文件或目录是否存在，并确认程序拥有足够的权限。" << endl;
		}
	}
	rewind(stdin);
	return str;
}


/* 文件删除相关结构体 */
typedef enum _FILE_INFORMATION_CLASS//文件操作信息说明
{
	FileDirectoryInformation = 1,
	FileFullDirectoryInformation,
	FileBothDirectoryInformation,
	FileBasicInformation,
	FileStandardInformation,
	FileInternalInformation,
	FileEaInformation,
	FileAccessInformation,
	FileNameInformation,
	FileRenameInformation,
	FileLinkInformation,
	FileNamesInformation,
	FileDispositionInformation,
	FilePositionInformation,
	FileFullEaInformation,
	FileModeInformation,
	FileAlignmentInformation,
	FileAllInformation,
	FileAllocationInformation,
	FileEndOfFileInformation,
	FileAlternateNameInformation,
	FileStreamInformation,
	FilePipeInformation,
	FilePipeLocalInformation,
	FilePipeRemoteInformation,
	FileMailslotQueryInformation,
	FileMailslotSetInformation,
	FileCompressionInformation,
	FileObjectIdInformation,
	FileCompletionInformation,
	FileMoveClusterInformation,
	FileQuotaInformation,
	FileReparsePointInformation,
	FileNetworkOpenInformation,
	FileAttributeTagInformation,
	FileTrackingInformation,
	FileIdBothDirectoryInformation,
	FileIdFullDirectoryInformation,
	FileValidDataLengthInformation,
	FileShortNameInformation,
	FileIoCompletionNotificationInformation,
	FileIoStatusBlockRangeInformation,
	FileIoPriorityHintInformation,
	FileSfioReserveInformation,
	FileSfioVolumeInformation,
	FileHardLinkInformation,
	FileProcessIdsUsingFileInformation,
	FileNormalizedNameInformation,
	FileNetworkPhysicalNameInformation,
	FileIdGlobalTxDirectoryInformation,
	FileIsRemoteDeviceInformation,
	FileAttributeCacheInformation,
	FileNumaNodeInformation,
	FileStandardLinkInformation,
	FileRemoteProtocolInformation,
	FileMaximumInformation
} FILE_INFORMATION_CLASS, * PFILE_INFORMATION_CLASS;

typedef struct _IO_STATUS_BLOCK
{
	union
	{
		NTSTATUS Status;
		PVOID    Pointer;
	};
	ULONG_PTR Information;
} IO_STATUS_BLOCK, * PIO_STATUS_BLOCK;

typedef struct _FILE_DISPOSITION_INFORMATION
{
	BOOLEAN DeleteFile;
}FILE_DISPOSITION_INFORMATION, * PFILE_DISPOSITION_INFORMATION;

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES
{
	ULONG           Length;
	HANDLE          RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG           Attributes;
	PVOID           SecurityDescriptor;
	PVOID           SecurityQualityOfService;
}OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;

NTSTATUS(__stdcall* pf_NtSetInformationFile)(
	HANDLE FileHandle,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID FileInformation,
	ULONG Length,
	FILE_INFORMATION_CLASS FileInformationClass
	);
NTSTATUS(__stdcall* pf_NtDeleteFile)(
	POBJECT_ATTRIBUTES ObjectAttributes
	);
VOID(__stdcall* pf_RtlInitUnicodeString)(
	PUNICODE_STRING DestinationString,
	PCWSTR SourceString
	);
struct stat buf;

BOOL Remove(const char* FileName)
{
	string tempA = "\\??\\";
	tempA += FileName;
	FILE_DISPOSITION_INFORMATION fi = { 1 };
	IO_STATUS_BLOCK bs = { 0 };
	OBJECT_ATTRIBUTES ob;
	UNICODE_STRING str;
	HMODULE hmodule = LoadLibrary(L"ntdll.dll");
	if (hmodule == NULL)
		return FALSE;
	pf_NtSetInformationFile = (NTSTATUS(__stdcall*)(
		HANDLE,
		PIO_STATUS_BLOCK,
		PVOID,
		ULONG,
		FILE_INFORMATION_CLASS))GetProcAddress(hmodule, "NtSetInformationFile");
	pf_NtDeleteFile = (NTSTATUS(__stdcall*)(POBJECT_ATTRIBUTES)) GetProcAddress(hmodule, "NtDeleteFile");
	pf_RtlInitUnicodeString = (VOID(__stdcall*)(PUNICODE_STRING, PCWSTR)) GetProcAddress(hmodule, "RtlInitUnicodeString");
	_TCHAR wc[MAX_PATH] = { 0 }, _FileName[MAX_PATH + 5] = { 0 };
#ifdef UNICODE
	_stprintf_s(wc, MAX_PATH, _T("%S"), FileName);//%S宽字符
	_stprintf_s(_FileName, MAX_PATH + 5, _T("%S"), tempA.c_str());//%S宽字符
#else
	_stprintf_s(wc, MAX_PATH, _T("%s"), FileName);//%s单字符
	_stprintf_s(_FileName, MAX_PATH + 5, _T("%s"), tempA.c_str());//%S宽字符
#endif
	pf_RtlInitUnicodeString(&str, _FileName);
	InitializeObjectAttributes(&ob, &str, OBJ_CASE_INSENSITIVE, NULL, NULL);
	return ifExistFile(wc, FALSE);
}


/* 优化程序运行 */
BOOL IsWow64()//判断是否为 Wow64
{
	BOOL bIsWow64 = FALSE;
	LPFN_ISWOW64PROCESS fnIsWow64Process;
	HMODULE hmodule = ::GetModuleHandle(TEXT("kernel32"));
	if (!hmodule)
		return FALSE;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)::GetProcAddress(hmodule, "IsWow64Process");
	if (NULL != fnIsWow64Process)
		if (!fnIsWow64Process(::GetCurrentProcess(), &bIsWow64))
			return FALSE;
	return bIsWow64;
}

BOOL WOW64FsDir(BOOL bDisable)//关闭重定向
{
	static WOW64_DISABLE_FSDIR pfnDisable = NULL;
	static WOW64_REVERT_FSDIR pfnrevert = NULL;
	static PVOID OldValue = NULL;
	static BOOL bInit = FALSE;
	if (!bInit && IsWow64())
	{
		HMODULE hMod = LoadLibrary(TEXT("kernel32.dll"));
		if (hMod)
		{
			pfnDisable = (WOW64_DISABLE_FSDIR)GetProcAddress(hMod, "Wow64DisableWow64FsRedirection");
			pfnrevert = (WOW64_REVERT_FSDIR)GetProcAddress(hMod, "Wow64RevertWow64FsRedirection");
		}
		if (pfnDisable == NULL || pfnrevert == NULL)
			return FALSE;
		bInit = TRUE;
	}
	if (IsWow64())
	{
		if (bDisable)
			return pfnDisable(&OldValue);
		else
			return pfnrevert(OldValue);
	}
	return TRUE;
}

BOOL IsAboveVistaVersion(DWORD Win)//判断系统版本是否为 Win 或以上
{
	if (Win <= _WIN32_WINNT)//返回的是 bool
		return TRUE;
	else
		return FALSE;
}

BOOL IsAdmin()//是否具备管理员权限
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

BOOL GetAdminPermission(_TCHAR* tchar, _TCHAR* _option)//以管理员权限启动
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

Function AvoidBug()
{
	system("chcp 936&title 雨鹅简易反病毒工具&color f&cls");
	setlocale(LC_CTYPE, "");
	if (::IsWow64())//关闭重定向
		::WOW64FsDir(TRUE);
	/* 检查在 Win7 及以上是否以管理员权限启动 */
	if (IsAboveVistaVersion(_WIN32_WINNT_WIN7) && !IsAdmin())
	{
		_TCHAR _0[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, _0, MAX_PATH);
		echo("非管理员权限，尝试以管理员权限启动。", Orange);
		Sleep(pause);
		if (!GetAdminPermission(_0, NULL))
		{
			echo("提权失败，请重新发起扫描并授权，按任意键退出。", Red);
			Sleep(pause << 1);
			exit(EXIT_FAILURE);
		}
		exit(EXIT_SUCCESS);
	}
}


/* 标记病毒并处理 */
Function Function1()
{
	char objpath[MAX_PATH] = { 0 };
	strcpy_s(objpath, getPath("请输入文件路径，或在非管理员模式下将文件拖拽到本窗口：", false).c_str());
	if (strlen(objpath) >= MAX_PATH - 1 || strlen(objpath) <= 0)
		return;
	ifstream fin(objpath, ios::binary);
	vector<char> buf((unsigned int)fin.seekg(0, ios::end).tellg());
	fin.seekg(0, ios::beg).read(&buf[0], static_cast<streamsize>(buf.size()));
	fin.close();
	if (buf.empty())
	{
		cout << "\a错误：读入失败，读入为空，请按任意键返回。" << endl;
		system("pause>nul");
		return;
	}
	_buf = buf;//保存到全局变量中供功能 2 使用
	cout << endl << endl << "读入完成，请按任意键返回。" << endl;
	system("pause>nul");
	return;
}


/* 扫描相似病毒 */
void Timing()//并发编程计时
{
	for (Tk.second = 0; Scanning; ++Tk.second)
	{
		if (Scanning == 1)//暂停状态
		{
			Tk.second--;
			continue;
		}
		Sleep(1000);
		if (Tk.second >= 60)
			Translate();
		TimeUpdate();
	}
	return;
}

size_t ldistance(vector<char>source)// Levenshtein Distance 算法实现
{
	/* 步骤一：预处理 */
	size_t m = _buf.size() - 1;
	size_t n = source.size() - 1;
	if (m <= 0 || n <= 0)//空载返回
		return NULL;
	typedef vector< vector<size_t> > Tmatrix;
	Tmatrix matrix(n + 1);
	for (size_t i = 0; i <= n; ++i)
		matrix[i].resize(m + 1);

	/* 步骤二：初始化 */
	for (size_t i = 1; i <= n; ++i)
		matrix[i][0] = i;
	for (size_t i = 1; i <= m; ++i)
		matrix[0][i] = i;

	/* 步骤三：跑循环 */
	for (size_t i = 1; i <= n; i++)
	{
		if (Scanning == 0)
			return EOF;
		while (Scanning == 1);//暂停
		const char si = source[i - 1];

		/* 步骤四：子循环 */
		for (size_t j = 1; j <= m; j++)
		{

			const char dj = _buf[j - 1];
			/* 步骤五：作比较 */
			size_t cost;
			if (si == dj)
				cost = 0;
			else
				cost = 1;

			/* 步骤六：再取样 */
			const size_t above = matrix[i - 1][j] + 1;
			const size_t left = matrix[i][j - 1] + 1;
			const size_t diag = matrix[i - 1][j - 1] + cost;
			matrix[i][j] = min(above, min(left, diag));
		}
	}

	/* 步骤七：返回值 */
	return size_t(matrix[n][m] * 100 / max(m, n));
}

void Scanner(int Count, vector<char>source, const char* objpath)//对比播报
{
	if (source.empty() || source.size() <= 0)//排除空
		cout << setw(6) << Count << "\t\t   ——" << "\t读入失败！\t";
	/*
	else if ((source.size() >> 1) >= (1 << (sizeof(size_t) * 8)))//排除文件太大
		cout << setw(6) << Count << "\t\t   ——" << "\t文件过大！\t";
	*/
	else
	{
		size_t constract = ldistance(source);
		if (constract == EOF)
			cout << setw(6) << Count << "\t\t   ——" << "\t\t未完成扫描\t";
		else if (constract == NULL)
			cout << setw(6) << Count << "\t\t   ——" << "\t\t空载类错误\t";
		else if (constract >= isVirus[0])
		{
			cout << setw(6) << Count << "\t\t" << setw(6) << constract << "%\t\t";
			echo("低风险播报", Orange);
			cout << '\t';
		}
		else if (constract >= isVirus[1])
		{
			cout << setw(6) << Count << "\t\t" << setw(6) << constract << "%\t\t";
			echo("高风险播报", Yellow);
			cout << '\t';
		}
		else if (constract >= isVirus[2])
		{
			BOOL tmp = Remove(objpath);//优先执行清除
			cout << setw(6) << Count << "\t\t" << setw(6) << constract << "%\t\t";
			if (tmp)
			{
				echo("已自动清除", Green);
				cout << '\t';
			}
			else
			{
				echo("清除失败！", Red);
				cout << '\t';
			}
		}
		else
		{
			cout << setw(6) << Count << "\t\t" << setw(6) << constract << "%\t\t";
			echo("未发现风险", Blue);
			cout << '\t';
		}
	}
	if (strlen(objpath) > (MaxLen << 1))
		cout << setw(MaxLen - 3) << objpath << &objpath[strlen(objpath) - MaxLen + 3] << endl;
	else
		cout << objpath << endl;
	return;
}

LRESULT __stdcall CBTHookProc(long nCode, WPARAM wParam, LPARAM lParam)//并发编程
{
	if (nCode == HCBT_ACTIVATE)
	{
		SetDlgItemText((HWND)wParam, IDTRYAGAIN, TEXT("暂停"));
#if (IDCONTINUE == IDIGNORE)
		SetDlgItemText((HWND)wParam, IDTRYAGAIN, TEXT("继续"));
#endif
		UnhookWindowsHookEx(hHook);
	}
	return 0;
}

void RunTimeBroker()//并发编程控制
{
	for (;;)//无条件循环
	{
		hHook = SetWindowsHookEx(WH_CBT, (HOOKPROC)CBTHookProc, NULL, GetCurrentThreadId());
		int errorlevel = MessageBox(NULL, (Scanning == 2 ? TEXT("正在运行扫描，您可以通过本窗口控制扫描。") : TEXT("扫描已暂停，请点击“继续”以完成扫描。")),
			TEXT("扫描控制台"), MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_TOPMOST | MB_DEFBUTTON3);
		if (Scanning == 4)
			return;
		switch (errorlevel)
		{
		case IDCANCEL://中止
		case IDABORT://平台不友好时
			Scanning = 0;
			return;
		case IDTRYAGAIN://暂停
			Scanning = 1;
			break;
		case IDCONTINUE://继续
			Scanning = 2;
			break;
		default:
			break;
		}
	}
	return;
}

Function Function2()//扫描相似病毒主函数
{
	if (_buf.empty())
	{
		cout << "提示：请先使用功能 1 进行录入病毒样本特征。" << endl;
		cout << "要返回主界面，请按下任意键。" << endl;
		system("pause>nul");
		return;
	}
	MEMORYSTATUSEX memoryStatus;
	memset(&memoryStatus, 0, sizeof(MEMORYSTATUSEX));
	memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memoryStatus);
	if ((memoryStatus.ullAvailPhys >> 20) <= F2_MemReq)
	{
		if (MessageBox(NULL, TEXT("内存空间不足，继续运行可能会导致电脑卡顿，是否继续？"), TEXT("内存诊断"),
			MB_YESNO | MB_ICONWARNING | MB_TOPMOST | MB_DEFBUTTON2) == IDNO)
			return;
	}
	char objpath[LIMIT][MAX_PATH] = { { 0 } };
	int j = 0;
	for (j = 0; j < LIMIT; ++j)
	{
		strcpy_s(objpath[j], getPath("请输入第 " + to_string(j + 1) + " 个待扫描文件，或在非管理员模式下将文件拖拽至此：", false).c_str());
		if (strlen(objpath[j]) >= MAX_PATH - 1 || strlen(objpath[j]) <= 0)
		{
			if (j == 0)
				return;
			break;
		}
		cout << endl;
	}
	if (MessageBox(NULL, TEXT("该操作时间耗时较长，是否继续？\n操作过程中，您可以随时中止程序的运行。"), TEXT("扫描控制台"), MB_OKCANCEL | MB_ICONQUESTION | MB_TOPMOST) == IDNO)
		return;
	system("cls");
	cout << "   序号\t\t  相似度\t   状态\t\t                          文件路径" << endl;
	cout << "==========\t==========\t==========\t============================================================" << endl << endl;
	InitTimer();
	Scanning = 2;
	thread t(RunTimeBroker);
	for (int i = 0; i < j && Scanning; ++i)
	{
		ifstream fin(objpath[i], ios::binary);
		vector<char> buf((unsigned int)fin.seekg(0, ios::end).tellg());
		fin.seekg(0, ios::beg).read(&buf[0], static_cast<streamsize>(buf.size()));
		fin.close();
		Scanner(i + 1, buf, objpath[i]);
	}
	Scanning = 4;
	bool temp = t.joinable();
	cout << endl << endl << endl << endl;
	if (temp)
		cout << "扫描完毕，结果如上，请点击“取消”关闭对话框，然后按任意键返回。" << endl;
	else
		cout << "扫描被中止，部分结果如上，请按任意键返回。" << endl;
	t.join();//尝试结束线程
	Scanning = 0;//防止用户点击了其它按钮
	system("pause>nul");
	return;
}


/* 疑似病毒上报 */
Function Function3()
{
	ShellExecuteA(NULL, ("open"), ("https://sandbox.depthsec.com.cn/index.php/"), NULL, NULL, SW_SHOWNORMAL);
	Sleep(pause);
	return;
}


/* 扫描设置 */
void ScanfSet(short int i)//取值
{
	rewind(stdin);
	if (i < 0 || i > 2)//越界
		return;
	while (scanf_s(JL_SIZE_T_SPECIFIER, &isVirus[i]) != 1 || isVirus[i] < LOW || isVirus[i] > size_t(i == 2 ? 101 : 100))
	{
		rewind(stdin);
		cout << "\a输入有误，请重新输入：";
	}
	rewind(stdin);
	if (i == 1 || i == 2)
		if (isVirus[i - 1] > isVirus[i])//如果低级值高于高级值
		{
			isVirus[i - 1] = isVirus[i];//将低级值降为高级值
			cout << "提示：您的低级值高于当前值，已自动为您修改。" << endl;
		}
	return;
}

Function Function4()
{
	if (isVirus[0] >= isVirus[1] || isVirus[1] >= isVirus[2])//非法数据
	{
		isVirus[0] = DEFAULTLOW;
		isVirus[1] = DEFAULTMIDDLE;
		isVirus[2] = DEFAULTHIGH;
	}
	cout << "当前配置：" << endl;
	cout << "\t1 = 低风险相似度播报值：" << isVirus[0] << " %" << endl;
	cout << "\t2 = 高风险相似度播报值：" << isVirus[1] << " %" << endl;
	cout << "\t3 = 自动清除相似度值：" << isVirus[2] << " %" << endl << endl;
	short int choice = 1;
	while (choice != 0)
	{
		cout << "请选择一个配置以进行修改（输入“0”可返回）：";
		rewind(stdin);
		if (scanf_s("%hd", &choice) != 1 || choice < 0 || choice > 3)
		{
			choice = 1;
			continue;
		}
		switch (choice)
		{
		case 1:
			cout << "请输入低风险相似度播报值（" << LOW << "-100）：";
			break;
		case 2:
			cout << "请输入高风险相似度播报值（" << LOW << "-100）：";
			break;
		case 3:
			cout << "请输入自动清除相似度值（" << LOW << "-101）：";
			break;
		default:
			continue;
		}
		ScanfSet(choice - 1);
	}
	rewind(stdin);
	return;
}


/* 检测第三方反病毒引擎 */
Function Function5()
{
	cout << "正在获取进程信息，请稍候。" << endl;
	char Command[MAX_PATH + 30] = "wmic process get name > \"";
	_TCHAR StrLine[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, szPath, MAX_PATH);//程序所在目录
	for (size_t i = strlen(szPath) - 1; szPath[i] != '\\'; --i)
		szPath[i] = 0;
	strcat_s(szPath, "process.log");
	strcat_s(Command, szPath);
	strcat_s(Command, "\"");
	system(Command);
	FILE* fp;
	fopen_s(&fp, szPath, "rb");
	if (fp == NULL)
	{
		cout << "\a错误：无法获取进程信息，请按任意键返回。" << endl;
		system("pause>nul");
		return;
	}
	system("cls");
	cout << "在您的计算机中，我们探测到有如下反病毒软件：" << endl << endl;
	while (fgetws(StrLine, MAX_PATH, fp) != NULL)//读取一行
	{
		while (lstrlen(StrLine) > 0 && StrLine[lstrlen(StrLine) - 1] < 33)//排除 wmic 生成的空余字符干扰
			StrLine[lstrlen(StrLine) - 1] = 0;
		if (lstrcmp(StrLine, L"MPSVC.exe") == 0)
			cout << "\t微点主动防御软件" << endl;
		if (lstrcmp(StrLine, L"MPAVSvc.exe") == 0)//不能用 else if
			cout << "\t微点杀毒软件" << endl;
		if (lstrcmp(StrLine, L"PC Hunter Standard 32.exe") == 0)
			cout << "\tPC Hunter Standard" << endl;
		else if (lstrcmp(StrLine, L"PC Hunter Standard 64.exe") == 0)//必须用 else if
			cout << "\tPC Hunter Standard" << endl;
		if (IsWow64() && lstrcmp(StrLine, L"SuperKiller.exe") == 0)
			cout << "\t360 系统急救箱" << endl;//可能检测失效
		if (lstrcmp(StrLine, L"Scanner.exe") == 0)
			cout << "\t雨鹅简易反病毒工具" << endl;
		if (lstrcmp(StrLine, L"Defender.exe") == 0)
			cout << "\t影子系统" << endl;
		if (lstrcmp(StrLine, L"WinDefender.exe") == 0)
			cout << "\tWindows Defender" << endl;
		if (lstrcmp(StrLine, L"hipstray.exe") == 0)
			cout << "\t火绒" << endl;
		if (lstrcmp(StrLine, L"avp.exe") == 0)
			cout << "\t卡巴斯基" << endl;
		if (lstrcmp(StrLine, L"RavMonD.exe") == 0)
			cout << "\t瑞星杀毒软件" << endl;
		if (lstrcmp(StrLine, L"rfwmain.exe") == 0)
			cout << "\t瑞星防火墙" << endl;
		if (lstrcmp(StrLine, L"rtvscan.exe") == 0)
			cout << "\t诺顿杀毒软件" << endl;
		if (lstrcmp(StrLine, L"360sd.exe") == 0)
			cout << "\t360 杀毒" << endl;
		if (lstrcmp(StrLine, L"ZhuDongFangYu.exe") == 0)
			cout << "\t360 安全卫士" << endl;
		if (lstrcmp(StrLine, L"QQPCTray.exe") == 0)
			cout << "\t腾讯电脑管家" << endl;
		if (lstrcmp(StrLine, L"kxetray.exe") == 0)
			cout << "\t金山毒霸" << endl;
		if (lstrcmp(StrLine, L"ksafe.exe") == 0)
			cout << "\t金山卫士" << endl;
		if (lstrcmp(StrLine, L"BaiduSdSvc.exe") == 0)
			cout << "\t百度杀毒" << endl;
	}
	fclose(fp);
	cout << endl << endl << "输出完毕，请按任意键返回。" << endl;
	system("pause>nul");
	return;
}


/* 遍历文件以触发第三方反病毒软件静默扫描 */
void str_tok(char* str) //分离文件目录
{
	char* p = NULL, * buf = NULL, delims[] = "\\";
	p = strtok_s(str, delims, &buf);
	while (p != NULL)
	{
		strcpy_s(result, p);
		p = strtok_s(NULL, delims, &buf);
	}
}

void str_extension(char* str)//分离文件扩展名
{
	char* p = NULL, * buf = NULL, delims[] = ".";
	p = strtok_s(str, delims, &buf);
	while (p != NULL)
	{
		strcpy_s(result, p);
		p = strtok_s(NULL, delims, &buf);
	}
}

int SeekPath(const char* lp_path, const char* expname, bool isRoot)//遍历搜索目录
{
	static int cnt = 0;
	if (isRoot)
		cnt = 0;
	HANDLE hFile;
	WIN32_FIND_DATAA wfd;//数据结构
	char sz_path[MAX_PATH << 1] = { 0 };
	char buf[MAX_PATH << 1] = { 0 };
	char fileName[MAX_PATH] = { 0 };
	char temp[MAX_PATH] = { 0 };//临时数组
	char parseFileName[MAX_PATH] = { 0 };
	strcpy_s(sz_path, lp_path);
	strcat_s(sz_path, "\\*.*");//匹配任何文件包括文件夹目录
	hFile = FindFirstFileA(sz_path, &wfd);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (wfd.cFileName[0] == '.')//如果是.或..则过滤
				continue;
			else if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)//如果是目录则递归
			{
				strcpy_s(sz_path, lp_path);
				strcat_s(sz_path, "\\");
				strcat_s(sz_path, wfd.cFileName);//拼接目录全路径名
				SeekPath(sz_path, expname, false);//调用递归
			}
			else
			{
				strcpy_s(parseFileName, wfd.cFileName);
				str_extension(parseFileName);//分离文件的扩展名
				if (!(strcmp(result, expname)))//拓展名过滤规则
				{
					strcpy_s(fileName, lp_path);//合成文件名全路径
					strcat_s(fileName, "\\");
					strcat_s(fileName, wfd.cFileName);
					echo(fileName, White);
					strcpy_s(temp, lp_path);
					str_tok(temp);//获取目录
					//sprintf_s(buf, "%s;%s", fileName, result);//含有上级目录
					sprintf_s(buf, "%s", fileName);
					fprintf(pf_path_file, "%s\n", buf);
					cnt++;
				}
			}
		} while (FindNextFileA(hFile, &wfd));
	}
	return cnt;
}

int DumpFoundPaths(const char* lp_path, const char* out_file_name, const char* expname, bool quiet)//转存文件
{
	fopen_s(&pf_path_file, out_file_name, "a");
	if (!pf_path_file)
	{
		if (quiet)
			return EOF;
		cout << "\a错误：存储 log 文件出错，请按任意键返回。" << endl;
		system("pause>nul");
		return 0;
	}
	int cnt = SeekPath(lp_path, expname, true);
	fclose(pf_path_file);
	return cnt;
}

Function Function6(char* arv)
{
	MEMORYSTATUSEX memoryStatus;
	memset(&memoryStatus, 0, sizeof(MEMORYSTATUSEX));
	memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memoryStatus);
	if ((memoryStatus.ullAvailPhys >> 20) <= F6_MemReq)
	{
		if (arv)//取消定时扫描
			return;
		if (MessageBox(NULL, TEXT("内存空间不足，继续运行可能会导致电脑卡顿，是否继续？"), TEXT("内存诊断"),
			MB_YESNO | MB_ICONWARNING | MB_TOPMOST | MB_DEFBUTTON2) == IDNO)
			return;
	}
	GetModuleFileNameA(NULL, szPath, MAX_PATH);//程序所在目录
	for (size_t i = strlen(szPath) - 1; szPath[i] != '\\'; --i)
		szPath[i] = 0;
	if (strlen(szPath) == 0)
		strcpy_s(szPath, "D:\\Error.log");//出错了
	strcpy_s(save_path, szPath);//日志 Scanner.log 放在程序所在目录中
	strcat_s(save_path, "Scanner.log");
	string str;
	if (arv)
		str = string(arv);
	else
	{
		str = getPath("请输入目录路径，或在非管理员权限下将文件夹拖拽至此（“*”表示可读盘扫描）：", true);
		if (str == "")
			return;
	}
	strcpy_s(szPath, str.c_str());//一定要用线程安全函数
	if (szPath[strlen(szPath) - 1] == '\\')
		szPath[strlen(szPath) - 1] = 0;
	system("cls");
	cout << "开始对目录“" << szPath << "”运行遍历，请等待程序运行完毕。" << endl << endl;
	strcpy_s(Nowpath, szPath);
	InitTimer();
	Scanning = 2;
	thread t(Timing);
	if (strcmp(szPath, "*") == 0)
	{
		strcat_s(szPath, ":");
		for (char i = 'A'; i <= 'Z'; ++i)
		{
			szPath[0] = i;
			DumpFoundPaths(szPath, save_path, "exe", arv != NULL);
			DumpFoundPaths(szPath, save_path, "com", arv != NULL);
		}
	}
	else
	{
		DumpFoundPaths(szPath, save_path, "exe", arv != NULL);
		DumpFoundPaths(szPath, save_path, "com", arv != NULL);
	}
	Scanning = 0;
	while (!t.joinable());//等待线程中止
	t.join();
	cout << endl << endl << "遍历完成，耗时：" << (Tk.hour < 10 ? "0" : "") << Tk.hour << ":" << (Tk.minute < 10 ? "0" : "") << Tk.minute << ":" << (Tk.second < 10 ? "0" : "") << Tk.second << "，请按任意键返回。" << endl;
	if (arv == NULL)
		system("pause>nul");
	return;
}


/* 本地代码简略安全分析 */
typedef struct//顺序栈的数据类型
{
	ElemType* base;
	ElemType* top;
	int stacksize;
}Stack;

Status Init(Stack& S)//初始化顺序栈
{
	S.base = new ElemType[MAX_PATH];
	if (!S.base)
		return ERROR;
	*S.base = 0;
	S.top = S.base;
	S.stacksize = MAX_PATH;
	return OK;
}

Status Push(Stack& S, ElemType x)//入栈
{
	if (S.top - S.base == S.stacksize)
	{
		//cout << "\a>> 错误：栈空间已满，压入元素 " << x << " 失败！" << endl;
		return ERROR;
	}
	*S.top++ = x;
	//cout << ">> 压入元素 " << x << " 成功。" << endl;
	return OK;
}

Status Pop(Stack& S, ElemType& e)//出栈
{
	if (S.top == S.base)
	{
		//cout << "\a>> 错误：栈为空！" << endl;
		e = 0;
		return ERROR;
	}
	e = *--S.top;
	//cout << ">> 元素出栈成功，该元素为 " << e << "。" << endl;
	return OK;
}

Status Top(Stack S, ElemType& e)//获取栈顶元素
{
	if (S.top == S.base)
	{
		//cout << "\a>> 错误：栈为空！" << endl;
		e = 0;
		return ERROR;
	}
	e = *(S.top - 1);
	return OK;
}

/*
Status Show(Stack S)//打印栈
{
	cout << "栈：top—";
	while (S.base != S.top)
		cout << *--S.top << "—";
	cout << "bottom" << endl << endl;
	return OK;
}
*/

Status isEmpty(Stack S)//判断是否为空栈
{
	if (S.base == S.top)
		return OK;
	else
		return ERROR;
}

Status Destroy(Stack& S)//销毁栈
{
	if (!S.base)
		return ERROR;
	delete[] S.base;
	S.base = NULL;
	S.top = NULL;
	S.stacksize = 0;//归零
	if (S.base)
		return ERROR;
	return OK;
}


void strstr_cpp(_TCHAR* buffer, Stack& A, bool Judges[])// C/C++ 脚本
{
	ElemType e = 0;
	if (!isEmpty(A))// \n
	{
		Top(A, e);
		if (e == '/')
			Pop(A, e);
		Top(A, e);
	}
	for (int i = 0; i < lstrlen(buffer) - 1; ++i)//多出的 \n 可以用来占位
	{
		if (buffer[i] == '\\')//排除转义符号
		{
			i++;
			continue;
		}
		if (buffer[i] == '/' && buffer[i + 1] == '/' && isEmpty(A))// //
		{
			Push(A, '/');
			break;//当行注释
		}
		if (buffer[i] == '/' && buffer[i + 1] == '*' && isEmpty(A))// /*
		{
			Push(A, '*');
			i++;
		}
		if (buffer[i] == '\"' || buffer[i + 1] == '\"')// "
		{
			if (buffer[i] == buffer[i + 1])//两个引号直接循环
				continue;
			if (isEmpty(A))
				Push(A, '\"');
			else
			{
				Top(A, e);
				if (e == '\"')
					Pop(A, e);
			}
			if (buffer[i + 1] == '\"')//避免重复计入
				i++;
		}
		if (buffer[i] == '\'' || buffer[i + 1] == '\'' && isEmpty(A))// '
		{
			if (buffer[i] == buffer[i + 1])//两个引号直接循环
				continue;
			if (isEmpty(A))
				Push(A, '\'');
			else
			{
				Top(A, e);
				if (e == '\'')
					Pop(A, e);
			}
			if (buffer[i + 1] == '\'')//避免重复计入
				i++;
		}
		if (buffer[i] == '*' && buffer[i + 1] == '/' && !isEmpty(A))// */
		{
			Top(A, e);
			if (e == '*')
				Pop(A, e);
		}
		if (isEmpty(A))
		{
			int iLen = WideCharToMultiByte(CP_ACP, 0, buffer, -1, NULL, 0, NULL, NULL);
			char* chRtn = new char[iLen * sizeof(char)];
			WideCharToMultiByte(CP_ACP, 0, buffer, -1, chRtn, iLen, NULL, NULL);
			string str = &chRtn[i];//用 string 截取方便

			/* 存在非线程安全的函数  创建进程  跨进程写入数据  文件行为  注册表行为  网络行为 */
			if (str.substr(0, 8) == "stricmp(" || str.substr(0, 7) == "strcat(" || str.substr(0, 7) == "strcpy(" || str.substr(0, 7) == "strtok("
				|| str.substr(0, 6) == "scanf(" || str.substr(0, 5) == "gets(" || str.substr(0, 6) == "getch("
				|| str.substr(0, 7) == "sscanf(" || str.substr(0, 8) == "sprintf(" || str.substr(0, 6) == "fopen("
				|| str.substr(0, 5) == "itoa(" || str.substr(0, 6) == "_itoa(" || str.substr(0, 10) == "localtime("
				)
				Judges[0] = true;
			if (str.substr(0, 7) == "system(" || str.substr(0, 14) == "CreateProcess(" || str.substr(0, 15) == "CreateProcessA(")
				Judges[1] = true;
			if (str.substr(0, 13) == "OpenProcess(")
				Judges[2] = true;
			if (str.substr(0, 6) == "fopen(" || str.substr(0, 8) == "fopen_s(" || str.substr(0, 9) == "ifstream ")
				Judges[3] = true;
			if (str.substr(0, 5) == "HKEY ")
				Judges[4] = true;
			if (str.substr(0, 7) == "SOCKET " || str.substr(0, 7) == "socket ")
				Judges[5] = true;
		}
	}
	return;
}

void strstr_py(_TCHAR* buffer, Stack& A, bool Judges[])// python 脚本
{
	ElemType e = 0;
	if (!isEmpty(A))// \n
	{
		Top(A, e);
		if (e == '#')
			Pop(A, e);
		Top(A, e);
	}
	for (int i = 0; i < lstrlen(buffer) - 2; ++i)//多出的 \n 可以用来占位（或是 \r\n）
	{
		if (buffer[i] == '\\')//排除转义符号
		{
			i++;
			continue;
		}
		if (buffer[i] == '#' && isEmpty(A))// #
		{
			Push(A, '#');
			break;//单行注释
		}
		if (buffer[i] == '\"' || buffer[i + 1] == '\"' || buffer[i + 2] == '\"')// "
		{
			int tmp = 0;
			for (int j = i; j < i + 3; ++j)
				if (buffer[j] == '\"')
					tmp++;
			if (tmp == 3)//三个引号为字符串
			{
				if (isEmpty(A))
					Push(A, 'D');// Double Note
				else
				{
					Top(A, e);
					if (e == 'D')
						Pop(A, e);// """
				}
				i += 2;
			}
			else if (tmp == 2)
				i += 2;
			else if (tmp == 1)
			{
				if (isEmpty(A))
					Push(A, '\"');
				else
				{
					Top(A, e);
					if (e == '\"')
						Pop(A, e);
				}
				if (buffer[i + 1] == '\"')//避免重复计入
					i++;
				if (buffer[i + 2] == '\"')
					i += 2;
			}
		}
		if (buffer[i] == '\'' || buffer[i + 1] == '\'' || buffer[i + 2] == '\'')// '
		{
			int tmp = 0;
			for (int j = i; j < i + 3; ++j)
				if (buffer[j] == '\'')
					tmp++;
			if (tmp == 3)//三个引号为字符串
			{
				if (isEmpty(A))
					Push(A, 'S');// Single Note
				else
				{
					Top(A, e);
					if (e == 'S')
						Pop(A, e);// '''
				}
				i += 2;
			}
			else if (tmp == 2)
				i += 2;
			else if (tmp == 1)
			{
				if (isEmpty(A))
					Push(A, '\'');
				else
				{
					Top(A, e);
					if (e == '\'')
						Pop(A, e);
				}
				if (buffer[i + 1] == '\'')//避免重复计入
					i++;
				if (buffer[i + 2] == '\'')
					i += 2;
			}
		}
		if (isEmpty(A))
		{
			int iLen = WideCharToMultiByte(CP_ACP, 0, buffer, -1, NULL, 0, NULL, NULL);
			char* chRtn = new char[iLen * sizeof(char)];
			WideCharToMultiByte(CP_ACP, 0, buffer, -1, chRtn, iLen, NULL, NULL);
			string str = &chRtn[i];//用 string 截取方便

			/* （存在非线程安全的函数）  创建进程  （跨进程写入数据）  文件行为  注册表行为  网络行为 */
			if (str.substr(0, 14) == " import system" || str.substr(0, 10) == " os.system" || str.substr(0, 13) == " import popen" || str.substr(0, 9) == " os.popen")
				Judges[1] = true;
			if (str.substr(0, 5) == " open")
				Judges[3] = true;
			if (str.substr(0, 12) == "win32api.Reg" || str.substr(0, 8) == " _winreg")
				Judges[4] = true;
			if (str.substr(0, 13) == "import socket")
				Judges[5] = true;
		}
	}
	return;
}

void strstr_bat(_TCHAR* buffer, Stack& A, bool Judges[])// bat 脚本
{
	ElemType e = 0;
	for (int i = 0; i < lstrlen(buffer); ++i)
		if (lstrlen(buffer) > 0 && (buffer[0] == ' ' || buffer[0] == '\t'))//去掉行首的空格和缩进
		{
			for (int j = 0; j < lstrlen(buffer); ++j)
				buffer[j] = buffer[j + 1];
			if (lstrlen(buffer) > 0)
				buffer[lstrlen(buffer) - 1] = 0;
		}
	if (lstrlen(buffer) > 0 && buffer[0] == ':')//标签或注释
		return;
	if (lstrlen(buffer) > 3 && (buffer[0] == 'R' || buffer[0] == 'r') && (buffer[1] == 'E' || buffer[1] == 'e')
		&& (buffer[2] == 'M' || buffer[2] == 'm') && (buffer[3] == ' ')
		)// rem 注释
		return;
	if (lstrlen(buffer) > 0)//只要一行不为空就可能有创建进程的行为
		Judges[1] = true;
	Push(A, '(');//在命令最开始加括号
	for (int i = 0; i < lstrlen(buffer); ++i)
	{
		if (buffer[i] == '^')//跳过转义符
		{
			i++;
			continue;
		}
		else if (buffer[i] == '&')
			Push(A, '&');
		else if (buffer[i] == '(')
			Push(A, '(');
		else if (buffer[i] == ')')
			Pop(A, e);
		else if (buffer[i] == '%')//变量
		{
			Top(A, e);
			if (e == '%')
				Pop(A, e);
			else
				Push(A, '%');
		}
		else if (buffer[i] == '!')//延迟变量
		{
			Top(A, e);
			if (e == '!')
				Pop(A, e);
			else
				Push(A, '!');
		}
		if (!isEmpty(A))//不为空执行
		{
			Top(A, e);
			if (e == '%' || e == '|')//调用变量
				continue;
			int iLen = WideCharToMultiByte(CP_ACP, 0, buffer, -1, NULL, 0, NULL, NULL);
			char* chRtn = new char[iLen * sizeof(char)];
			WideCharToMultiByte(CP_ACP, 0, buffer, -1, chRtn, iLen, NULL, NULL);
			string str = &chRtn[i];//用 string 截取方便
			transform(str.begin(), str.end(), str.begin(), ::tolower);//批处理脚本不区分大小写

			/* （存在非线程安全的函数）  （创建进程）  （跨进程写入数据）  文件行为  注册表行为  网络行为 */
			if (str.substr(0, 14) == " import system" || str.substr(0, 10) == " os.system" || str.substr(0, 13) == " import popen" || str.substr(0, 9) == " os.popen")
				Judges[1] = true;
			if (str.substr(0, 5) == "move " || str.substr(0, 5) == "copy " || str.substr(0, 6) == "xcopy "
				|| str.substr(0, 4) == "del " || str.substr(0, 7) == "delete " || str.substr(0, 1) == "<" || str.substr(0, 1) == ">"
				|| str.substr(0, 3) == "md " || str.substr(0, 6) == "mkdir " || str.substr(0, 3) == "rd " || str.substr(0, 6) == "rmdir "
				|| str.substr(0, 5) == "type " || str.substr(0, 8) == "notepad " || str.substr(0, 6) == "write "
				)
				Judges[3] = true;
			if (str.substr(0, 4) == "reg " || str.substr(0, 8) == "regedit ")
				Judges[4] = true;
			if (str.substr(0, 5) == "ping " || str.substr(0, 8) == "tracert ")
				Judges[5] = true;
		}
		Top(A, e);
		if (e == '&')
			Pop(A, e);
	}
	Push(A, e);//命令结束加括号
	return;
}

void strstr_vbs(_TCHAR* buffer, Stack& A, bool Judges[])// vbs 脚本
{
	ElemType e = 0;
	for (int i = 0; i < lstrlen(buffer); ++i)
		if (lstrlen(buffer) > 0 && (buffer[0] == ' ' || buffer[0] == '\t'))//去掉行首的空格和缩进
		{
			for (int j = 0; j < lstrlen(buffer); ++j)
				buffer[j] = buffer[j + 1];
			if (lstrlen(buffer) > 0)
				buffer[lstrlen(buffer) - 1] = 0;
		}
	if (lstrlen(buffer) > 0 && buffer[0] == '\'')//注释
		return;
	int iLen = WideCharToMultiByte(CP_ACP, 0, buffer, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen * sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, buffer, -1, chRtn, iLen, NULL, NULL);
	string str = chRtn;//用 string 截取方便
	transform(str.begin(), str.end(), str.begin(), ::tolower);// vbs 脚本不区分大小写

	/* （存在非线程安全的函数）  创建进程  （跨进程写入数据）  文件行为  注册表行为  （网络行为） */
	if (str.find("createobject(\"wscript.shell\")") != str.npos)
		Judges[1] = true;
	if (str.find("createobject(\"scripting.filesystemobject\")") != str.npos)
		Judges[3] = true;
	if (str.find("regread(") != str.npos || str.find("regwrite(") != str.npos || str.find("regdelete(") != str.npos)
		Judges[4] = true;
	return;
}

#ifndef WIN32
#ifndef _WIN64
void strstr_sh(_TCHAR* buffer, Stack& A, bool Judges[])// sh 脚本
{
	cout << "请转到 linux 系统使用。" << endl;
	return;
}
#endif
#endif


Function Function7()
{
	string str = getPath("请输入文件路径，或在非管理员权限下将文件拖拽至此：", false);
	if (str == "")
		return;
	string temp_str = str;
#ifdef WIN32
	transform(str.begin(), str.end(), str.begin(), ::tolower);// Windows 统一换成小写
#endif
#ifdef _WIN64
	transform(str.begin(), str.end(), str.begin(), ::tolower);// Windows 统一换成小写
#endif
	cout << endl;
	short int type = NULL;//记录类型
	if (str.substr(str.length() - 4, str.length()) == ".cpp" || str.substr(str.length() - 2, str.length()) == ".c" || str.substr(str.length() - 2, str.length()) == ".h")
	{
		cout << "检测到 C/C++ 源代码文件，C 语言面向过程，C++ 面向对象。" << endl;
		type = 1;
	}
	else if (str.substr(str.length() - 3, str.length()) == ".py" || str.substr(str.length() - 4, str.length()) == ".pyw")//else if 运行较快
	{
		cout << "检测到 python 源代码文件。" << endl;
		type = 2;
	}
	else if (str.substr(str.length() - 4, str.length()) == ".bat" || str.substr(str.length() - 4, str.length()) == ".cmd")
	{
		cout << "检测到批处理脚本文件，请注意：命令行脚本检测的结果可能会出现偏差。" << endl;
		type = 3;
	}
	else if (str.substr(str.length() - 4, str.length()) == ".vbs")
	{
		cout << "检测到 vbs 脚本文件，此脚本文件面向对象。" << endl;
		type = 4;
	}
	else if (str.substr(str.length() - 3, str.length()) == ".sh")
	{
		cout << "检测到 linux shell 脚本文件，";
#if (defined WIN32 || defined _WIN64)
		cout << "此类脚本应在 linux 操作系统上使用。" << endl;
#else
		cout << "请注意：命令行脚本检测的结果可能会出现偏差。" << endl;
#endif`
		type = 5;
	}
	else if (str.substr(str.length() - 4, str.length()) == ".jar")
	{
		cout << "检测到 java 文件，暂不支持运行分析，请按任意键返回。" << endl;
		system("pause>nul");
		type = 6;
		return;
	}
	else if (str.substr(str.length() - 3, str.length()) == ".vb")
	{
		cout << "检测到 vb 文件，暂不支持运行分析，请按任意键返回。" << endl;
		system("pause>nul");
		type = 7;
		return;
	}
	else if (str.substr(str.length() - 4, str.length()) == ".exe")
	{
		cout << "检测到可执行文件，请使用功能 1 和功能 2，按任意键返回。" << endl;
		system("pause>nul");
		type = 8;
		return;
	}
	else if (str.substr(str.length() - 4, str.length()) == ".txt")
	{
		cout << "检测到文本文档，目前支持 4 种识别模式：" << endl;
		cout << ">> 1 = C/C++ 分析模式" << endl;
		cout << ">> 2 = python 分析模式" << endl;
		cout << ">> 3 = 批处理分析模式" << endl;
		cout << ">> 4 = vbs 分析模式" << endl;
		cout << ">> 0 = 取消" << endl;
		cout << endl << "请选择一项以继续：";
		rewind(stdin);
		while (scanf_s("%hd", &type) != 1 || type < 0 || type > 4)
		{
			rewind(stdin);
			cout << "\a错误：输入有误，请重新输入：";
		}
		if (type == 0)
			return;
	}
	else
	{
		cout << "\a错误：暂时无法识别此类文件，请检查后缀名是否正确，按任意键退出。" << endl;
		system("pause>nul");
		return;
	}
	str = temp_str;//归还 str
	ifstream fin(str.c_str(), ios::binary);//先以二进制方式获取编码模式
	unsigned char  s2;
	fin.read((char*)&s2, sizeof(s2));//读取第一个字节，然后左移 8 位
	int p = s2 << 8;
	fin.read((char*)&s2, sizeof(s2));//读取第二个字节
	p |= s2;
	string code;
	switch (p)//判断文本前两个字节
	{
	case 0xfffe://65534
		code = "Unicode";
		break;
	case 0xfeff://65279
		code = "Unicode big endian";
		break;
	case 0xefbb://61371
		code = "UTF-8";
		break;
	default:
		code = "ANSI";
		break;
	}
	fin.close();
	cout << "编码模式：" << code << "。" << endl;
	if (code == "ANSI")//切换文本读取模式（兼容 Windows）
		fopen_s(&pf_path_file, str.c_str(), "r");
	/*
	else if (code == "Unicode")
		fopen_s(&pf_path_file, str.c_str(), "rb");
	*/
	else
	{
		_TCHAR wc[MAX_PATH] = { 0 };
#ifdef UNICODE
		_stprintf_s(wc, MAX_PATH, _T("%S"), str.c_str());//%S宽字符
#else
		_stprintf_s(wc, MAX_PATH, _T("%s"), str.c_str());//%s单字符
#endif
		if (code == "UTF-8")
			_wfopen_s(&pf_path_file, wc, L"r");
		else if (code == "Unicode big endian")
			_wfopen_s(&pf_path_file, wc, L"rb");
		else
		{
			cout << "\a错误：不支持的编码模式，请尝试以 ANSI 编码另存为，按任意键返回。" << endl;
			system("pause>nul");
			return;
		}
	}
	if (pf_path_file == NULL)
	{
		cout << "\a错误：获取文件内容失败，请按任意键返回。" << endl;
		system("pause>nul");
		return;
	}
	fseek(pf_path_file, 0, SEEK_END);
	long ISize = ftell(pf_path_file);
	cout << "文件大小：" << ISize << "。" << endl;
	rewind(pf_path_file);
	if (code != "ANSI")
	{
		if (ISize > 4)//判断是否足够大
			fseek(pf_path_file, 3L, SEEK_CUR);//移动四个字节，避免读入文件头
		else
		{
			cout << "该源代码残缺，无法运行，请按任意键返回。" << endl;
			system("pause>nul");
			return;
		}
	}
	_TCHAR* buffer = (_TCHAR*)malloc(sizeof(_TCHAR) * ISize);
	if (buffer == NULL)
	{
		fclose(pf_path_file);
		pf_path_file = NULL;
		cout << "\a错误：内存资源不足，无法处理该请求，请按任意键返回。" << endl;
		system("pause>nul");
		return;
	}
	Stack A;
	if (Init(A) == ERROR)
	{
		fclose(pf_path_file);
		pf_path_file = NULL;
		cout << "\a错误：初始化栈失败，请按任意键返回。" << endl;
		system("pause>nul");
		return;
	}
	bool Judges[6] = { false };//存在非线程安全的函数  创建进程  跨进程写入数据  文件行为  注册表行为  网络行为
	system("cls");
	cout << "开始执行分析，过程可能需要一些时间，请耐心等待。" << endl;
	while (fgetws(buffer, ISize, pf_path_file) != NULL)//读取一行
	{
		//cout << buffer << endl;
		//system("pause");
		switch (type)
		{
		case 1:
			strstr_cpp(buffer, A, Judges);
			break;
		case 2:
			strstr_py(buffer, A, Judges);
			break;
		case 3:
			strstr_bat(buffer, A, Judges);
			break;
		case 4:
			strstr_vbs(buffer, A, Judges);
			break;
		case 5:
#if (defined WIN32 || defined _WIN64)
			system("cls");
			echo("请在 linux 系统上使用，并按任意键返回。", Orange);
			system("pause>nul");
			return;
#else
			system("cls");
			cout << "很抱歉，暂不支持 linux 脚本自动分析，请使用功能 2 上传分析，并按任意键返回。" << endl;
			system("pause>nul");
			return;
			//strstr_sh(buffer, A, Judges);
#endif
			break;
		default:
			system("cls");
			cout << "\a错误：很抱歉，程序暂不支持对此类文件自动分析，并按任意键退出。" << endl;
			system("pause>nul");
			return;
		}
	}
	fclose(pf_path_file);
	pf_path_file = NULL;//归还
	Destroy(A);//记得销毁栈
	system("cls");
	if (Judges[0] || Judges[1] || Judges[2] || Judges[3] || Judges[4] || Judges[5])//池化
	{
		cout << "分析完成，行为如下：" << endl << endl;
		if (Judges[0])
			cout << ">> 使用了非线程安全的函数" << endl;
		if (Judges[1])
			cout << ">> 存在进程行为" << endl;
		if (Judges[2])
			cout << ">> 存在跨进程写入数据行为" << endl;
		if (Judges[3])
			cout << ">> 存在文件行为" << endl;
		if (Judges[4])
			cout << ">> 存在注册表行为" << endl;
		if (Judges[5])
			cout << ">> 存在网络行为" << endl;
		cout << endl << endl << "分析完毕，请按任意键返回。" << endl;
	}
	else
		cout << "分析完成，未发现特殊行为，请按任意键返回。" << endl;
	system("pause>nul");
	return;
}

Function Function8()//颜色说明
{
	cout << "默认输出" << endl;
	echo("时间输出", Gray);
	echo("未发现风险", Blue);
	echo("低风险播报", Orange);
	echo("高风险播报", Yellow);
	echo("已自动清除", Green);
	echo("清除失败！", Red);
	cout << endl << endl << "说明完成，请按任意键返回。" << endl;
	system("pause>nul");
	return;
}

Function Function9(char* msg)//驱动删除文件
{
	HANDLE device = NULL;
	ULONG ret_len;
	int ret = EXIT_SUCCESS;

	/* 打开设备，每次要操作驱动对象时，先以此为例子打开设备 */
	device = CreateFileW(CWK_DEV_SYM, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
	if (device == INVALID_HANDLE_VALUE)
	{
		cout << "\a错误：与驱动通讯时发生异常！（" << msg << "）" << endl;
		return;
	}

	/* 发送驱删指令 */
	if (DeviceIoControl(device, CWK_DVC_SEND_STR, msg, (DWORD)(strlen(msg) + 1), NULL, 0, &ret_len, 0))
		cout << "通知驱动，删除文件——" << msg << "!" << endl;
	else
		cout << "\a错误：向驱动发送信息时发生异常！（" << msg << "）" << endl;

	CloseHandle(device);//记得关闭
	return;
}


/* main 函数 */
int main(int argc, char* argv[])
{
	char a[] = "C:\\1.txt";
	Function9(a);
	return 0;
	if (argc > 1)
	{
		if (IsAboveVistaVersion(_WIN32_WINNT_WIN7) && !IsAdmin())
			return EXIT_FAILURE;
		for (int i = 1; i < argc; ++i)
			Function6(argv[i]);
		return EXIT_SUCCESS;
	}
	AvoidBug();
	short int choice = 1;
	while (choice)
	{
		system("cls");
		cout << "欢迎使用雨鹅简易反病毒工具，本程序为您提供如下功能：" << endl;
		cout << "\t1 = 读取病毒样本" << endl;
		cout << "\t2 = 扫描相似病毒" << endl;
		cout << "\t3 = 疑似病毒上报" << endl;
		cout << "\t4 = 扫描设置" << endl;
		cout << "\t5 = 检测第三方反病毒引擎" << endl;
		cout << "\t6 = 遍历文件以触发第三方反病毒软件静默扫描" << endl;
		cout << "\t7 = 本地代码简略安全分析" << endl;
		cout << "\t8 = 颜色说明" << endl;
		cout << "\t0 = 退出程序" << endl;
		cout << endl << "请选择一项以继续：";
		rewind(stdin);
		while (scanf_s("%hd", &choice) != 1 || choice < 0 || choice > 8)
		{
			cout << "\a输入有误，请重新输入：";
			rewind(stdin);
		}
		rewind(stdin);
		system("cls");
		switch (choice)
		{
		case 1:
			Function1();
			break;
		case 2:
			Function2();
			break;
		case 3:
			Function3();
			break;
		case 4:
			Function4();
			break;
		case 5:
			Function5();
			break;
		case 6:
			Function6(NULL);
			break;
		case 7:
			Function7();
			break;
		case 8:
			Function8();
			break;
		default:
			break;
		}
	}
	return EXIT_SUCCESS;
}