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
#ifdef LoopOptions
#undef LoopOptions
#endif//用于遍历目标参数
#define LoopOptions for (int i = 2; i < argc; ++i)
#ifdef LoopHandles
#undef LoopHandles
#endif//用于遍历句柄
#define LoopHandles for (size_t j = 0; j < vecHandles.size(); ++j)
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
#ifdef CWK_DEV_SYM_P//重要
#undef CWK_DEV_SYM_P
#endif
#define CWK_DEV_SYM_P L"\\\\.\\GooseBtKMDFFileProtector"
#ifdef CWK_DEV_SYM_F//重要
#undef CWK_DEV_SYM_F
#endif
#define CWK_DEV_SYM_F L"\\\\.\\GooseBtZwDeleteFile"

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


/* NTQUERYOBJECT */
typedef struct _OBJECT_NAME_INFORMATION {
	UNICODE_STRING Name;
	WCHAR NameBuffer[1];
} OBJECT_NAME_INFORMATION, * POBJECT_NAME_INFORMATION;

/* winternl.h 中以定义，微软未放出 134 的定义，需自己手动定义 */
typedef enum _OBJECT_INFORMATION_CLASS_SELFDEFINE {
	//ObjectBasicInformation,
	ObjectNameInformation = 1,
	//ObjectTypeInformation,
	ObjectAllInformation = 3,
	ObjectDataInformation = 4
} OBJECT_INFORMATION_CLASS_SELFDEFINE, * POBJECT_INFORMATION_CLASS_SELFDEFINE;

typedef NTSTATUS(WINAPI* NTQUERYOBJECT)(
	_In_opt_ HANDLE Handle,
	_In_ OBJECT_INFORMATION_CLASS_SELFDEFINE ObjectInformationClass,
	_Out_opt_ PVOID ObjectInformation,
	_In_ ULONG ObjectInformationLength,
	_Out_opt_ PULONG ReturnLength);

/* NTQUERYSYSTEMINFORMATION */
typedef struct _SYSTEM_HANDLE {
	DWORD dwProcessId;
	BYTE bObjectType;
	BYTE bFlags;
	WORD wValue;
	PVOID pAddress;
	DWORD GrantedAccess;
} SYSTEM_HANDLE, * PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION {
	DWORD dwCount;
	SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION;

typedef NTSTATUS(WINAPI* NTQUERYSYSTEMINFORMATION)(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL);

//typedef struct _IO_STATUS_BLOCK {
//    LONG Status;
//    LONG Information;
//} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef struct _FILE_NAME_INFORMATION {
	ULONG FileNameLength;
	WCHAR FileName[MAX_PATH];
} FILE_NAME_INFORMATION;

__declspec(dllimport) LONG __stdcall ZwQueryInformationFile(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN ULONG FileInformationLength,
	IN ULONG FileInformationClass
);

typedef LONG(__stdcall* PFN_ZwQueryInformationFile) (
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN ULONG FileInformationLength,
	IN ULONG FileInformationClass
	);

// typedef struct _FILE_NAME_INFORMATION {
//     ULONG FileNameLength;
//     WCHAR FileName[1];
// } FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

typedef NTSTATUS(WINAPI* NTQUERYINFORMATIONFILE)(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass);

// typedef struct _CLIENT_ID {
//     HANDLE UniqueProcess;
//     HANDLE UniqueThread;
// } CLIENT_ID, *PCLIENT_ID;


/* ncScopedHandle */
class ncScopedHandle
{
	ncScopedHandle(const ncScopedHandle&);
	ncScopedHandle& operator=(const ncScopedHandle&);
public:
	ncScopedHandle(HANDLE handle)
		: _handle(handle)
	{
	}

	~ncScopedHandle()
	{
		if (_handle != NULL) {
			CloseHandle(_handle);
		}
	}

	operator HANDLE() const
	{
		return _handle;
	}

	PHANDLE  operator& ()
	{
		return &_handle;
	}

	void operator=(HANDLE handle)
	{
		if (_handle != NULL)
			CloseHandle(_handle);
		_handle = handle;
	}

private:
	HANDLE _handle;
};


/* ncFileHandle */
struct ncFileHandle
{
	SYSTEM_HANDLE    _handle;       //占用文件的进程句柄详细信息
	tstring          _filePath;     //文件的完整路径（tstring 等同于 wstring）
	tstring          _path;         //占用进程的程序磁盘位置

	ncFileHandle(SYSTEM_HANDLE handle, const tstring& filePath, const tstring& path)
		: _handle(handle)
		, _filePath(filePath)
		, _path(path)
	{
	}
};


/* 文件解锁子函数 */
EXTERN_C BOOL GetVolumeNameByHandle(HANDLE hFile, LPWSTR szVolumeName, UINT cchMax)//根据文件句柄获取文件所在磁盘名
{
	BOOL bResult = FALSE;
	WCHAR szBuf[MAX_PATH << 1] = { 0 };
	WCHAR* pIter = szBuf;
	BY_HANDLE_FILE_INFORMATION stFileInfo = { 0 };

	do
	{
		if (FALSE == GetFileInformationByHandle(hFile, &stFileInfo))
			break;
		if (0 == GetLogicalDriveStringsW(_countof(szBuf), szBuf))
			break;
		for (; pIter; pIter += 4)
		{
			DWORD dwVolumeSerialNumber = 0;
			if (GetVolumeInformationW(pIter, NULL, 0, &dwVolumeSerialNumber, NULL, NULL, NULL, 0))
			{
				if (dwVolumeSerialNumber == stFileInfo.dwVolumeSerialNumber)
				{
					lstrcpynW(szVolumeName, pIter, cchMax);
					bResult = TRUE;
					break;
				}
			}
		}
	} while (FALSE);
	return bResult;
}

EXTERN_C BOOL GetFilePathFromHandleW(HANDLE hFile, LPWSTR lpszPath, UINT cchMax)
{
	BOOL bResult = FALSE;
	WCHAR szValue[MAX_PATH] = { 0 };
	IO_STATUS_BLOCK isb = { 0 };
	FILE_NAME_INFORMATION fni = { 0 };
	HMODULE hNtDLL = NULL;
	PFN_ZwQueryInformationFile pfn_ZwQueryInformationFile = NULL;

	do
	{
		/* 加载 dll 模块 */
		if (INVALID_HANDLE_VALUE == hFile || NULL == lpszPath || 0 == cchMax)
			break;
		hNtDLL = LoadLibraryW(L"ntdll.dll");//加载 ntdll.dll
		if (hNtDLL == NULL)
			break;

		/* 加载模块条目 */
		pfn_ZwQueryInformationFile = (PFN_ZwQueryInformationFile)GetProcAddress(hNtDLL, "ZwQueryInformationFile");
		if (NULL == pfn_ZwQueryInformationFile)
			break;
		if (0 != pfn_ZwQueryInformationFile(hFile, &isb, &fni, sizeof(fni), 9))// 9 == FileNameInformation
			break;
		if (FALSE == GetVolumeNameByHandle(hFile, szValue, _countof(szValue)))
			break;
		PathAppendW(szValue, fni.FileName);
		lstrcpynW(lpszPath, szValue, cchMax);
		bResult = TRUE;
	} while (FALSE);
	return bResult;
}

EXTERN_C BOOL GetFilePathFromHandleA(HANDLE hFile, LPSTR  lpszPath, UINT cchMax)
{
	BOOL bResult = FALSE;
	WCHAR szTmep[MAX_PATH] = { 0 };

	do
	{
		if (INVALID_HANDLE_VALUE == hFile || NULL == lpszPath || 0 == cchMax)
			break;
		if (FALSE == GetFilePathFromHandleW(hFile, szTmep, _countof(szTmep)))
			break;
		if (0 == WideCharToMultiByte(CP_ACP, 0,
			szTmep, lstrlenW(szTmep),
			lpszPath, cchMax, NULL, NULL)
			)
			break;
		bResult = TRUE;
	} while (FALSE);
	return bResult;
}


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


/* 各大子函数 */
VOID GetDeviceDriveMap(std::map<tstring, tstring>& mapDeviceDrive)// GetDeviceDriveMap
{
	TCHAR szDrives[512];
	if (!GetLogicalDriveStrings(_countof(szDrives) - 1, szDrives))
		return;
	TCHAR* lpDrives = szDrives;
	TCHAR szDevice[MAX_PATH];
	TCHAR szDrive[3] = _T(" :");

	do
	{
		*szDrive = *lpDrives;
		if (QueryDosDevice(szDrive, szDevice, MAX_PATH))
			mapDeviceDrive[szDevice] = szDrive;
		while (*lpDrives++);
	} while (*lpDrives);
}

BOOL DevicePathToDrivePath(tstring& path)// DevicePathToDrivePath
{
	static std::map<tstring, tstring> mapDeviceDrive;
	if (mapDeviceDrive.empty())
		GetDeviceDriveMap(mapDeviceDrive);

	for (std::map<tstring, tstring>::const_iterator it = mapDeviceDrive.begin(); it != mapDeviceDrive.end(); ++it)
	{
		size_t nLength = it->first.length();
		if (_tcsnicmp(it->first.c_str(), path.c_str(), nLength) == 0)
		{
			path.replace(0, nLength, it->second);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL GetHandlePath(HANDLE handle, tstring& path)// GetHandlePath
{
	static NTQUERYOBJECT fpNtQueryObject =
		(NTQUERYOBJECT)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtQueryObject");
	if (fpNtQueryObject == NULL)
		return FALSE;

	DWORD dwLength = 0;
	OBJECT_NAME_INFORMATION info;
	NTSTATUS status = fpNtQueryObject(handle, ObjectNameInformation, &info, sizeof(info), &dwLength);
	if (status != STATUS_BUFFER_OVERFLOW)
		return FALSE;

	POBJECT_NAME_INFORMATION pInfo = (POBJECT_NAME_INFORMATION)malloc(dwLength);
	for (;;)
	{
		status = fpNtQueryObject(handle, ObjectNameInformation, pInfo, dwLength, &dwLength);
		if (status != STATUS_BUFFER_OVERFLOW)
			break;
		POBJECT_NAME_INFORMATION tmpInfo = (POBJECT_NAME_INFORMATION)realloc(pInfo, dwLength);
		pInfo = (POBJECT_NAME_INFORMATION)realloc(tmpInfo, dwLength);
	}

	BOOL bRes = FALSE;
	if (NT_SUCCESS(status) && pInfo)
	{
		path = pInfo->Name.Buffer;
		bRes = DevicePathToDrivePath(path);
	}
	free(pInfo);
	return bRes;
}

PSYSTEM_HANDLE_INFORMATION GetSystemHandleInfo()// GetSystemHandleInfo
{
	static NTQUERYSYSTEMINFORMATION fpNtQuerySystemInformation =
		(NTQUERYSYSTEMINFORMATION)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtQuerySystemInformation");

	if (fpNtQuerySystemInformation == NULL)
		return NULL;

	DWORD dwLength = 0;
	SYSTEM_HANDLE_INFORMATION shi;
	NTSTATUS status = fpNtQuerySystemInformation(SystemHandleInformation, &shi, sizeof(shi), &dwLength);
	if (status != STATUS_INFO_LENGTH_MISMATCH)
		return NULL;

	PSYSTEM_HANDLE_INFORMATION pshi = (PSYSTEM_HANDLE_INFORMATION)malloc(dwLength);
	for (;;)
	{
		status = fpNtQuerySystemInformation(SystemHandleInformation, pshi, dwLength, &dwLength);
		if (status != STATUS_INFO_LENGTH_MISMATCH)
			break;
		PSYSTEM_HANDLE_INFORMATION tmpshi = (PSYSTEM_HANDLE_INFORMATION)realloc(pshi, dwLength);//防止 realloc 泄露内存
		pshi = (PSYSTEM_HANDLE_INFORMATION)realloc(tmpshi, dwLength);
	}

	if (!NT_SUCCESS(status))
	{
		free(pshi);
		pshi = NULL;
	}
	return pshi;
}

/** 检测指定句柄是否可能导致 NtQueryObject 卡死：
 *	注意必须使用 NtQueryInformationFile 而不是使用 NtQueryObject 进行检测，
 *	否则可能导致 WinXP 系统下进程死锁而无法结束。
 */
void CheckBlockThreadFunc(void* param)
{
	static NTQUERYINFORMATIONFILE fpNtQueryInformationFile =
		(NTQUERYINFORMATIONFILE)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtQueryInformationFile");

	if (fpNtQueryInformationFile != NULL)
	{
		BYTE buf[1024];
		IO_STATUS_BLOCK ioStatus;
		fpNtQueryInformationFile((HANDLE)param, &ioStatus, buf, 1024, FileNameInformation);
	}
}

BOOL IsBlockingHandle(HANDLE handle)// IsBlockingHandle
{
	HANDLE hThread = (HANDLE)_beginthread(CheckBlockThreadFunc, 0, (void*)handle);

	if (WaitForSingleObject(hThread, 100) != WAIT_TIMEOUT)
		return FALSE;

	TerminateThread(hThread, 0);
	return TRUE;
}

BOOL FindFileHandle(LPCTSTR lpName, vector<ncFileHandle>& handles)// FindFileHandle
{
	handles.clear();

	if (NULL == lpName)
		return FALSE;

	/* 打开“NUL”文件以便后续获取文件句柄类型值 */
	ncScopedHandle hTempFile = CreateFile(_T("NUL"), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
	if (hTempFile == NULL)
		return FALSE;

	/* 获取当前系统中所有的句柄信息 */
	PSYSTEM_HANDLE_INFORMATION pshi = GetSystemHandleInfo();
	if (pshi == NULL)
		return FALSE;

	/* 查询当前系统的文件句柄类型值 */
	BYTE nFileType = 0;
	DWORD dwCrtPid = GetCurrentProcessId();
	for (DWORD i = 0; i < pshi->dwCount; ++i)
		if (pshi->Handles[i].dwProcessId == dwCrtPid && hTempFile == (HANDLE)pshi->Handles[i].wValue)
		{
			nFileType = pshi->Handles[i].bObjectType;
			break;
		}

	HANDLE hCrtProc = GetCurrentProcess();
	for (DWORD i = 0; i < pshi->dwCount; ++i)
	{
		/* 过滤掉非文件类型的句柄 */
		if (pshi->Handles[i].bObjectType != nFileType)
			continue;

		/* 将上述句柄复制到当前进程中 */
		ncScopedHandle handle = NULL;
		ncScopedHandle hProc = OpenProcess(PROCESS_DUP_HANDLE, FALSE, pshi->Handles[i].dwProcessId);
		if (NULL == hProc
			|| !DuplicateHandle(
			hProc, (HANDLE)pshi->Handles[i].wValue,
			hCrtProc, &handle, 0,
			FALSE, DUPLICATE_SAME_ACCESS
			)
		)
			continue;

		/* 过滤掉会导致 NtQueryObject 卡死的句柄（如管道等） */
		if (IsBlockingHandle(handle))
			continue;

		/* 获取句柄对应的文件路径并进行匹配检查 */
		tstring filePath;
		if (GetHandlePath(handle, filePath) && filePath.find(lpName) != tstring::npos)
		{
			ncScopedHandle hProcess = OpenProcess(MAXIMUM_ALLOWED, FALSE, pshi->Handles[i].dwProcessId);
			TCHAR szProcName[MAX_PATH];
			GetProcessImageFileName(hProcess, szProcName, MAX_PATH);
			tstring path(szProcName);
			DevicePathToDrivePath(path);
			ncFileHandle fh(pshi->Handles[i], filePath, path);
			handles.push_back(fh);
		}
	}

	free(pshi);
	return TRUE;
}

BOOL CloseHandleEx(HANDLE handle, DWORD dwPid)
{
	if (GetCurrentProcessId() == dwPid)//是本程序在占用文件
		return CloseHandle(handle);

	ncScopedHandle hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, dwPid);
	if (hProcess == NULL)
		return FALSE;

	return DuplicateHandle(hProcess, handle, GetCurrentProcess(), NULL, 0, FALSE, DUPLICATE_CLOSE_SOURCE);
}


/* 关闭重定向 */
APIType IsWow64()//判断是否为 Wow64
{
	APIType bIsWow64 = FALSE;
	LPFN_ISWOW64PROCESS fnIsWow64Process;
	HMODULE hmodule = ::GetModuleHandle(TEXT("kernel32"));
	if (!hmodule)
		return FALSE;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)::GetProcAddress(hmodule, "IsWow64Process");
	if (NULL != fnIsWow64Process)
		fnIsWow64Process(::GetCurrentProcess(), &bIsWow64);
	return bIsWow64;
}

APIType WOW64FsDir(BOOL bDisable)//关闭重定向
{
	static WOW64_DISABLE_FSDIR pfnDisable = NULL;
	static WOW64_REVERT_FSDIR pfnrevert = NULL;
	static PVOID OldValue = NULL;
	static BOOL bInit = FALSE;
	if (!bInit && IsWow64())//if (!bInit && OVI_IS64(g_OsVer))
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
	if (IsWow64())//if (OVI_IS64(g_OsVer))
	{
		if (bDisable)
			return pfnDisable(&OldValue);
		else
			return pfnrevert(OldValue);
	}
	return TRUE;
}

APIType FindFirstFileExists(LPCTSTR lpPath, BOOL dwFilter)//检查文件是否存在
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
			delete[]_str;
			FindClose(_hFind);
		}
	}
	FindClose(hFind);
	return RetValue;
}



/* 与驱动通信函数 */
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
void showHelp()
{
	wcout << endl << L"描述：综合文件管理器。" << endl;
	wcout << endl << L"wmif [首参数] [文件路径]" << endl << endl;
	wcout << L"可用首参数列表：" << endl;
	wcout << L"\tshowlock\t查询文件占用情况" << endl;
	wcout << L"\tunlock\t\t解锁文件" << endl;
	wcout << L"\tsmash\t\t粉碎文件" << endl;
	wcout << L"\tKMDFProtect\t驱动保护（占用文件法）" << endl << endl;
	return;
}


/* 遵循 Microsoft Windows 命令行规则的 main 函数 */
int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_CTYPE, "");
	wcout.imbue(locale("chs", LC_CTYPE));
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);//更改优先级
	SetProcessPriorityBoost(GetCurrentProcess(), FALSE);//锁定优先级
	if (::IsWow64())//关闭重定向
		::WOW64FsDir(TRUE);

	if (argc >= 3)//参数数量正确
	{
		if (0 == _wcsicmp(argv[1], L"showlock")
			|| 0 == _wcsicmp(argv[1], L"unlock")
			|| 0 == _wcsicmp(argv[1], L"smash")
		)//攻击类
		{
			LoopOptions
			{
				tstring path(argv[i]);
				vector<ncFileHandle> vecHandles;
				if (!FindFirstFileExists(argv[i], FALSE))
				{
					wcout << L"文件“" << argv[i] << L"”不存在，无需执行进一步操作！" << endl;
					continue;
				}
				if (0 == _wcsicmp(argv[1], L"smash"))//驱删
					DriverConnector(argv[i], CWK_DEV_SYM_F);
				else//不涉及驱动的其它操作
				{
					if (!FindFileHandle(path.c_str(), vecHandles))
						wcout << L"查询文件“" << argv[i] << L"”锁定信息失败，无法执行进一步操作！" << endl;
					else
					{
						if (0 == _wcsicmp(argv[1], L"unlock"))
						{
							if (vecHandles.size() > 0)
							{
								wcout << L"文件“" << argv[i] << L"”共有 " << vecHandles.size() << L" 个文件句柄。" << endl;
								LoopHandles
								{
									wcout << L"[" << j << L"]\t0x" << vecHandles[j]._handle.pAddress << L"\t" << vecHandles[j]._handle.dwProcessId << L"\t\"" << vecHandles[j]._path.c_str() << L"\"" << endl;
									CloseHandleEx((HANDLE)vecHandles[j]._handle.wValue, vecHandles[j]._handle.dwProcessId);
								}
							}
							else
								wcout << L"文件“" << argv[i] << L"”未被锁定，无需解除占用。" << endl;
						}
						else if (0 == _wcsicmp(argv[1], L"showlock"))
						{
							if (vecHandles.size() > 0)
							{
								wcout << L"文件“" << argv[i] << L"”共有 " << vecHandles.size() << L" 个文件句柄。" << endl;
								LoopHandles
									wcout << L"[" << j << L"]\t0x" << vecHandles[j]._handle.pAddress << L"\t" << vecHandles[j]._handle.dwProcessId << L"\t\"" << vecHandles[j]._path.c_str() << L"\"" << endl;
							}
							else
								wcout << L"文件“" << argv[i] << L"”未被锁定。" << endl;
						}
					}
				}
				wcout << endl;
			}
			return EXIT_SUCCESS;
		}
		else if (0 == _wcsicmp(argv[1], L"KMDFProtect"))//防护类
		{
			LoopOptions
				DriverConnector(argv[i], CWK_DEV_SYM_P);
			return EXIT_SUCCESS;
		}
		else
		{
			wcout << L"\a错误：命令行参数不正确！" << endl;
			return EOF;
		}
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
	else
	{
		wcout << L"\a错误：命令行参数不正确！" << endl;
		return EOF;
	}
}