#include <iostream>
#ifdef WIN32
#include <Windows.h>
#include <WinInet.h>
#else
#ifdef _WIN64
#include <Windows.h>
#include <WinInet.h>
#ifndef WIN32
#define WIN32 1
#endif
#endif
#endif//_Windows_H
#include <fstream>
#include <tchar.h>
#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")//不显示窗口
//#pragma comment(linker,"/MERGE:.rdata=.text /MERGE:.data=.text /SECTION:.text,EWR")//减小编译体积（会有警告）
#ifndef _GooseBtUpdate_H
#define _GooseBtUpdate_H
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#ifndef VersionFile
#define VersionFile "Version.dat"
#endif
#ifndef NewVersionFile
#define NewVersionFile "NewVersion.dat"
#endif
#ifndef x86File
#define x86File "GooseBt_4G_Anti-Virus_Software_x86.exe"
#endif
#ifndef x64File
#define x64File "GooseBt_4G_Anti-Virus_Software_x64.exe"
#endif
#ifndef BatchFile
#define BatchFile "FileInforSupport.bat"
#endif
#ifndef UpdateExeName
#define UpdateExeName "GooseBtUpdate.exe"
#endif
#ifndef UpdateLogName
#define UpdateLogName "UpdateLog.dat"
#endif
#ifndef SvcLauncher
#define SvcLauncher "SVCLauncher.exe"
#endif
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "urlmon.lib")//编译时请务必使用静态编译
#endif//_GooseBtUpdate_H
using namespace std;
HHOOK hHook;

unsigned long long FileData[0x2000000];//载入数据
unsigned long long T1, T2, W[80];//本轮使用的消息，和分组中保存的消息
unsigned long long HashI_1[8];//中间结果
unsigned long long HashI[8];//最终结果
const unsigned long long Kt[80] = {// 80 个常数
	0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL,
	0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
	0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL,
	0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
	0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
	0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
	0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL, 0x2de92c6f592b0275ULL,
	0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
	0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL,
	0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
	0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL,
	0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
	0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL,
	0x92722c851482353bULL, 0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
	0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
	0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
	0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL,
	0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
	0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL,
	0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
	0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL,
	0xc67178f2e372532bULL, 0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
	0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL,
	0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
	0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
	0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
	0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL,
};


/* 获取父目录路径 */
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


/* 管理员权限相关问题 */
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

wstring getExeFullFilename()
{
	_TCHAR buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	int i = lstrlen(buffer);
	for (; buffer[i] != '\\' && i > 0; --i)
		buffer[i] = 0;
	lstrcat(buffer, _T(UpdateExeName));
	return buffer;
}

BOOL getStart(const char* ImagePath, const char* param)
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


/* 网络连接相关问题 */
BOOL isConnected()//检查网络连接
{
	DWORD flag;
	return InternetGetConnectedState(&flag, 0) ? TRUE : FALSE;
}

BOOL FileDownload(const char* Url, const char* SavePath)//将 Url 指向的地址的文件下载到 SavePath 指向的本地文件
{
	if (!isConnected())
	{
		MessageBox(NULL, TEXT("很抱歉，网络连接失败，请检查您的网络，并确保本程序未被拒绝联网。"), TEXT("GooseBt 升级程序"), MB_OK | MB_ICONERROR | MB_TOPMOST);
		return FALSE;
	}
	DeleteUrlCacheEntryA(Url);
	BOOL bRet;
	if (URLDownloadToFileA(NULL, Url, SavePath, 0, NULL) == S_OK)
		bRet = TRUE;
	else
		bRet = FALSE;
	DeleteUrlCacheEntryA(Url);
	if (!bRet)
		MessageBox(NULL, TEXT("很抱歉，更新文件下载失败，请稍后再试。"), TEXT("GooseBt 升级程序"), MB_OK | MB_ICONERROR | MB_TOPMOST);
	return bRet;
}


/* 正文部分 */
void InitializeHash()//初始化 HashI_1 HashI
{
	HashI[0] = 0x6a09e667f3bcc908;
	HashI[1] = 0xbb67ae8584caa73b;
	HashI[2] = 0x3c6ef372fe94f82b;
	HashI[3] = 0xa54ff53a5f1d36f1;
	HashI[4] = 0x510e527fade682d1;
	HashI[5] = 0x9b05688c2b3e6c1f;
	HashI[6] = 0x1f83d9abfb41bd6b;
	HashI[7] = 0x5be0cd19137e2179;
	return;
}

unsigned long long ROTR(unsigned long long x, int n)//循环右移 n 位
{
	return ((x >> n) | (x << (64 - n)));
}

void SHA_512(unsigned __int64 N)
{
	int i, t, j;
	for (i = 0; i < N; ++i)
	{
		for (j = 0; j < 16; ++j)
			W[j] = FileData[i * 16 + j];//从全部数据中载入本次所需的消息
		for (j = 16; j < 80; ++j)//将 16-79 轮的信息计算出
			W[j] = (ROTR(W[j - 2], 19) ^ ROTR(W[j - 2], 61) ^ (W[j - 2] >> 6)) + W[j - 7] + (ROTR(W[j - 15], 1) ^ ROTR(W[j - 15], 8) ^ (W[j - 15] >> 7)) + W[j - 16];
		for (j = 0; j < 8; ++j)
			HashI_1[j] = HashI[j];//每次开始输入之前，将之前得到的输出读入，然后对中间的 hashI_1 值进行操作，输出给 HashI
		for (t = 0; t < 80; ++t)// 80 轮操作
		{
			T1 = HashI_1[7] + ((HashI_1[4] & HashI_1[5]) ^ ((~HashI_1[4]) & HashI_1[6]))
				+ (ROTR(HashI_1[4], 14) ^ ROTR(HashI_1[4], 18) ^ ROTR(HashI_1[4], 41)) + W[t] + Kt[t];
			T2 = (ROTR(HashI_1[0], 28) ^ ROTR(HashI_1[0], 34) ^ ROTR(HashI_1[0], 39))
				+ ((HashI_1[0] & HashI_1[1]) ^ (HashI_1[0] & HashI_1[2]) ^ (HashI_1[1] & HashI_1[2]));
			HashI_1[7] = HashI_1[6];
			HashI_1[6] = HashI_1[5];
			HashI_1[5] = HashI_1[4];
			HashI_1[4] = HashI_1[3] + T1;
			HashI_1[3] = HashI_1[2];
			HashI_1[2] = HashI_1[1];
			HashI_1[1] = HashI_1[0];
			HashI_1[0] = T1 + T2;
		}
		for (j = 0; j < 8; j++)
			HashI[j] += HashI_1[j];//得到输出
	}
	return;
}

string CheckUpdate(string FilePath)
{
	int i, t, k, l, j = 0;
	unsigned __int64 N, M;// n 个 1024，M 个 256m
	unsigned char lastChar[128];
	unsigned long long TxtLength;
	InitializeHash();
	fstream FileDataf(FilePath, ios::in | ios::binary);
	FileDataf.seekp(0, ios::end);
	TxtLength = FileDataf.tellp();//获得文件的大小
	FileDataf.seekp(0, ios::beg);
	N = 1 << 21;// 256m 中含有 1^21 个 1024
	M = (TxtLength >> 28) + 1;//获得数据有多少个256m的块
	for (t = 0; t < M; ++t)
	{
		if (t == M - 1)
		{
			N = (TxtLength - (1 << 28) * (M - 1)) >> 7;//当只剩下最后一组256m时，计算剩下的1024组数-1
			for (i = 0; i < N; i++) {//将剩下的满1024的组先读入
				FileDataf.read((char*)lastChar, 128);//一次读取128个char
				for (k = 0; k < 16; k++)
				{
					FileData[j] = 0;
					for (l = 0; l < 8; l++)
						FileData[j] = (FileData[j] << 8) | lastChar[k * 8 + l];
					j++;
				}
			}
			N = TxtLength - (1 << 28) * (M - 1) - (N << 7);//计算最后剩下的字节数
			for (i = 0; i < N; ++i)
				FileDataf.read((char*)(&lastChar[i]), 1);
			if (i >= 112)//补余时，若最后一段大于 896 则必须再加一层 1024
			{
				lastChar[i++] = 128;//最高位填充1之后填充0 
				for (; i < 128; ++i)
					lastChar[i] = 0;
				for (i = 0; i < 16; ++i)
				{
					FileData[j] = 0;
					for (k = 0; k < 8; k++)
						FileData[j] = (FileData[j] << 8) | lastChar[i * 8 + k];
					j++;
				}
				for (i = 0; i < 112; i++)//新的1024行要再次填充到896位
					lastChar[i] = 0;
			}
			else
			{
				lastChar[i++] = 128;//最高位填充1之后填充0 
				for (; i < 112; ++i)
					lastChar[i] = 0;
			}
			//最后128位为消息长度，第一个数固定为0，第二个数直接为TextLength * 8
			//将数据从lastChar数组中读入到FileData数组中
			for (i = 0; i < 14; ++i)
			{
				FileData[j] = 0;
				for (k = 0; k < 8; ++k)
					FileData[j] = (FileData[j] << 8) | lastChar[i * 8 + k];
				j++;
			}
			FileData[j++] = 0;
			FileData[j++] = TxtLength << 3;
			N = j >> 4;// j 的数量肯定是合理的，则可以由此时j的数量得到最后1024的组数
			SHA_512(N);//进行 hash
		}
		else
		{
			for (i = 0; i < N; ++i)
			{
				FileDataf.read((char*)lastChar, 128);
				for (k = 0; k < 16; ++k)
				{
					FileData[j] = 0;
					for (l = 0; l < 8; ++l)
						FileData[j] = (FileData[j] << 8) | lastChar[k * 8 + l];
					j++;
				}
			}
			SHA_512(N);
		}
	}
	FileDataf.close();
	char HashStrings[129] = { 0 }, tmpStrings[17] = { 0 };
	for (j = 0; j < 8; ++j)
	{
		sprintf_s(tmpStrings, "%016I64x", HashI[j]);
		strcat_s(HashStrings, tmpStrings);
	}
	return string(HashStrings);
}

void SilentRunBatch(string FolderPath)//批处理更新 Version.dat
{
	string BatchPath = FolderPath + BatchFile;
	SHELLEXECUTEINFO exeInfo;
	exeInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	exeInfo.fMask = (SEE_MASK_DOENVSUBST | SEE_MASK_FLAG_DDEWAIT | 0x04000000 | SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE);
	exeInfo.hwnd = NULL;
	exeInfo.lpVerb = L"open";
	_TCHAR wc[MAX_PATH] = { 0 };
#ifdef UNICODE
	_stprintf_s(wc, MAX_PATH, _T("%S"), BatchPath.c_str());//%S宽字符
#else
	_stprintf_s(wc, MAX_PATH, _T("%s"), BatchPath.c_str());//%s单字符
#endif
	exeInfo.lpFile = wc;
	exeInfo.lpParameters = L"/QYES";
	exeInfo.lpDirectory = NULL;
	exeInfo.nShow = SW_HIDE;
	exeInfo.hInstApp = NULL;
	exeInfo.lpIDList = NULL;
	exeInfo.lpClass = NULL;
	exeInfo.hkeyClass = NULL;
	exeInfo.dwHotKey = NULL;
	exeInfo.hMonitor = NULL;
	exeInfo.hIcon = NULL;
	exeInfo.hProcess = NULL;
	::ShellExecuteEx(&exeInfo);
	if (exeInfo.hProcess)
	{
		::WaitForSingleObject(exeInfo.hProcess, INFINITE);
		::CloseHandle(exeInfo.hProcess);
	}
	return;
}

LRESULT __stdcall CBTHookProc(long nCode, WPARAM wParam, LPARAM lParam)//并发编程 Funtion1
{
	if (nCode == HCBT_ACTIVATE)
	{
		SetDlgItemText((HWND)wParam, IDYES, TEXT("完整更新"));
		SetDlgItemText((HWND)wParam, IDNO, TEXT("增量更新"));
		SetDlgItemText((HWND)wParam, IDCANCEL, TEXT("取消更新"));
		UnhookWindowsHookEx(hHook);
	}
	return 0;
}

int main()//本程序无窗口
{
	/* 程序初始化 */
	setlocale(LC_CTYPE, "");
	SetConsoleTitle(L"GooseBt 升级程序");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
	if (!IsAdmin())
	{
		_TCHAR _0[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, _0, MAX_PATH);
		if (!GetAdminPermission(_0, NULL))
			MessageBox(NULL, TEXT("提示：用户取消授权。"), TEXT("GooseBt 升级程序"), MB_OK | MB_ICONWARNING | MB_TOPMOST);
		return EXIT_FAILURE;
	}

	/* 文件路径 */
	string FolderPath = GF_GetEXEPath();
	string VersionPath = FolderPath + VersionFile;
	string NewVersion = FolderPath + NewVersionFile;
	string x86Path = FolderPath + x86File;
	string x64Path = FolderPath + x64File;

	/* 本地信息 */
	SetFileAttributesA(VersionPath.c_str(), FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_SYSTEM);
	FILE* fp;
	fopen_s(&fp, VersionPath.c_str(), "r");
	if (NULL == fp)
	{
		SilentRunBatch(FolderPath);
		SetFileAttributesA(VersionPath.c_str(), FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_SYSTEM);
		fopen_s(&fp, VersionPath.c_str(), "r");
		if (NULL == fp)
		{
			MessageBox(NULL, TEXT("读取本地软件信息失败！"), TEXT("GooseBt 升级程序"), MB_OK | MB_ICONERROR | MB_TOPMOST);
			return EXIT_FAILURE;
		}
	}
	char LocalVer[MAX_PATH] = { 0 };
	fgets(LocalVer, MAX_PATH, fp);
	rewind(fp);//归还文件指针
	fclose(fp);//记得关闭文件

	/* 远程服务器信息 */
	SetFileAttributesA(NewVersion.c_str(), FILE_ATTRIBUTE_NORMAL);
#ifdef _WIN64
	if (!FileDownload("http://goosebt.com:8080/externalLinksController/chain/Version.dat?ckey=o2%2F12f7odvlCmRo%2FAkFsmpBxFLue7WxbkqpLTz672PladiOZYrKqnp6AFCw6kV0n", NewVersion.c_str()))
		return EXIT_FAILURE;
#else
	if (!FileDownload("http://goosebt.com:8080/externalLinksController/chain/Version.dat?ckey=U7vhFxlQGsiOEIWMV4ndGDQf2TFLx6hV4fR%2F7W5jGeuy8gy66SXou0kSij1ruNFK", NewVersion.c_str()))
		return EXIT_FAILURE;
#endif
	SetFileAttributesA(NewVersion.c_str(), FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_SYSTEM);
	char NewVer[MAX_PATH] = { 0 }, buffer[MAX_PATH << 2] = { 0 };
	fopen_s(&fp, NewVersion.c_str(), "r");
	if (NULL == fp)
	{
		MessageBox(NULL, TEXT("读取远程更新信息失败，点击“确定”，执行回滚。"), TEXT("GooseBt 升级程序"), MB_OK | MB_ICONERROR | MB_TOPMOST);
		SilentRunBatch(FolderPath);
		return EXIT_FAILURE;
	}
	fgets(NewVer, MAX_PATH, fp);
	string Tmptext;
	if (strcmp(LocalVer, NewVer) == 0)
	{
		rewind(fp);
		fclose(fp);
		string cmdL = "del /a /f /q \"" + NewVersion + "\"";
		system(cmdL.c_str());//删除远程文件信息
		Tmptext = "当前程序已是最新（";
		Tmptext += LocalVer;
		Tmptext = Tmptext.substr(0, Tmptext.length() - 1);
		Tmptext += "）";
		MessageBoxA(NULL, Tmptext.c_str(), "GooseBt 升级程序", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
		return EXIT_SUCCESS;
	}
	Tmptext = "当前主程序版本：";
	Tmptext += LocalVer;
	Tmptext += "远程服务器版本：";
	Tmptext += NewVer;
	hHook = SetWindowsHookEx(WH_CBT, (HOOKPROC)CBTHookProc, NULL, GetCurrentThreadId());
	if (hHook)
	{
		Tmptext += "请选择一种更新方式以继续。";
		switch (MessageBoxA(NULL, Tmptext.c_str(), "GooseBt 升级程序", MB_YESNOCANCEL | MB_ICONINFORMATION | MB_TOPMOST))
		{
		case IDYES:
#ifdef _WIN64
			return FileDownload("http://goosebt.com:8080/externalLinksController/chain/GooseBt_4G_Anti-Virus_Software_x64.exe?ckey=VvkSdk8UPqRfOVZ7SI5u6wN8PuiyOkOfuaxVsI7ZdyOSGsS7m7TK%2BV25B5cCQ%2FUh", x64Path.c_str()) ? EXIT_SUCCESS : EXIT_FAILURE;
#else
			return FileDownload("http://goosebt.com:8080/externalLinksController/chain/GooseBt_4G_Anti-Virus_Software_x86.exe?ckey=RmutXeRsi6hzEpp3m1TnXU5ac9E5O2sS7gPjwknKrN0Wq5X9fU5FuW1V%2Bm2Us79w", x86Path.c_str()) ? EXIT_SUCCESS : EXIT_FAILURE;
#endif
		case IDCANCEL:
			if (fp)//避开跳过实例初始化错误
			{
				rewind(fp);
				fclose(fp);
				string cmdL = "del /a /f /q \"" + NewVersion + "\"";
				system(cmdL.c_str());//删除远程文件信息
			}
			return EXIT_SUCCESS;
		default:
			break;
		}
	}
	else
	{
		Tmptext += "是否完整更新？";
		switch (MessageBoxA(NULL, Tmptext.c_str(), "GooseBt 升级程序", MB_YESNOCANCEL | MB_ICONINFORMATION | MB_TOPMOST))
		{
		case IDYES:
#ifdef _WIN64
			return FileDownload("http://goosebt.com:8080/externalLinksController/chain/GooseBt_4G_Anti-Virus_Software_x64.exe?ckey=VvkSdk8UPqRfOVZ7SI5u6wN8PuiyOkOfuaxVsI7ZdyOSGsS7m7TK%2BV25B5cCQ%2FUh", x64Path.c_str()) ? EXIT_SUCCESS : EXIT_FAILURE;
#else
			return FileDownload("http://goosebt.com:8080/externalLinksController/chain/GooseBt_4G_Anti-Virus_Software_x86.exe?ckey=RmutXeRsi6hzEpp3m1TnXU5ac9E5O2sS7gPjwknKrN0Wq5X9fU5FuW1V%2Bm2Us79w", x86Path.c_str()) ? EXIT_SUCCESS : EXIT_FAILURE;
#endif
		case IDCANCEL:
			if (fp)//避开跳过实例初始化错误
			{
				rewind(fp);
				fclose(fp);
				string cmdL = "del /a /f /q \"" + NewVersion + "\"";
				system(cmdL.c_str());//删除远程文件信息
			}
			return EXIT_SUCCESS;
		default:
			break;
		}
	}

	/* 通知服务停止运行 *//* 通知驱动停止守护 */
	string DriEvent = "\"";
	DriEvent += GF_GetEXEPath() + SvcLauncher + "\"";
	if (!getStart(DriEvent.c_str(), "/Stop"))
	{
		MessageBox(NULL, TEXT("错误：与驱动通讯过程发生异常，或驱动停止失败！"), TEXT("占用性错误"), MB_OK | MB_ICONERROR | MB_TOPMOST);
		return EXIT_FAILURE;
	}

	/* 增量升级 */
	while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
		if (buffer[strlen(buffer) - 1] == '\n' || buffer[strlen(buffer) - 1] == '\r')
			buffer[strlen(buffer) - 1] = 0;
		if (buffer[strlen(buffer) - 1] == '\r')//排除某些情况下行末为\r\n
			buffer[strlen(buffer) - 1] = 0;
		string splited[3];
		char* Nothing = NULL;
		splited[0] = strtok_s(buffer, "\t", &Nothing);
		splited[1] = strtok_s(NULL, "\t", &Nothing);
		splited[2] = strtok_s(NULL, "\t", &Nothing);
		string NewSHA = CheckUpdate(GF_GetEXEPath() + splited[0]);
		if (NewSHA == splited[1])
			continue;
		if (!FileDownload(splited[2].c_str(), (GF_GetEXEPath() + splited[0] + (splited[0] == UpdateExeName ? ".tmp" : "")).c_str()))
		{
			Tmptext = "文件 " + splited[0] + " 下载失败，继续升级吗？";
			if (MessageBoxA(NULL, Tmptext.c_str(), "GooseBt 升级程序", MB_YESNO | MB_ICONQUESTION | MB_TOPMOST) == IDYES)
				continue;
			else
			{
				rewind(fp);
				fclose(fp);
				return EXIT_FAILURE;
			}
		}
		NewSHA = CheckUpdate(GF_GetEXEPath() + splited[0] + (splited[0] == UpdateExeName ? ".tmp" : ""));
		if (NewSHA != splited[1])
		{
			Tmptext = "文件 " + splited[0] + " 检验失败，继续升级吗？";
			if (MessageBoxA(NULL, Tmptext.c_str(), "GooseBt 升级程序", MB_YESNO | MB_ICONQUESTION | MB_TOPMOST) == IDYES)
				continue;
			else
			{
				rewind(fp);
				fclose(fp);
				return EXIT_FAILURE;
			}
		}
	}
	rewind(fp);
	fclose(fp);

	/* 通知服务启动程式 *//* 通知驱动启动守护 */
	if (!getStart(DriEvent.c_str(), "/Start"))
	{
		MessageBox(NULL, TEXT("错误：与驱动通讯过程发生异常，或驱动停止失败！"), TEXT("占用性错误"), MB_OK | MB_ICONERROR | MB_TOPMOST);
		return EXIT_FAILURE;
	}

	/* 重命名文件 */
	Tmptext = "升级成功结束（";
	Tmptext += NewVer;
	Tmptext = Tmptext.substr(0, Tmptext.length() - 1);
	Tmptext += "）\n";
	Tmptext += "是否查看更新日志？";
	if (MessageBoxA(NULL, Tmptext.c_str(), "GooseBt 升级程序", MB_YESNO | MB_ICONINFORMATION | MB_TOPMOST) == IDYES)
	{
		SetFileAttributesA((GF_GetEXEPath() + UpdateLogName).c_str(), FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_SYSTEM);
		ShellExecuteA(NULL, "open", "notepad.exe", (GF_GetEXEPath() + UpdateLogName).c_str(), "", SW_SHOWNORMAL);
	}
	Tmptext = "/c call cd /d \"%~dp0\"&ping localhost>nul&del /a /f /q \"";
	Tmptext += UpdateExeName;
	Tmptext += "\"&ren \"";
	Tmptext += UpdateExeName;
	Tmptext += ".tmp\" \"";
	Tmptext += UpdateExeName;
	Tmptext += "\"";
	ShellExecuteA(NULL, "open", "cmd.exe", Tmptext.c_str(), GF_GetEXEPath().c_str(), SW_HIDE);
	return EXIT_SUCCESS;
}