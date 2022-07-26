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
#endif//Windows.h
#include <tchar.h>
#ifndef _AntiVirusMon_H
#define _AntiVirusMon_H
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
#ifndef VOID
#define VOID void
#endif
#ifndef BUF_SIZE
#define BUF_SIZE 1024// 2 ** 10 == 1024
#endif
#ifndef gapTime
#define gapTime 1000
#endif
#ifdef PIPE_NAME
#undef PIPE_NAME
#endif//重要
#define PIPE_NAME L"\\\\.\\pipe\\GooseBtPipe"
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
#endif//_AntiVirusMon_H
using namespace std;
typedef int Status;
typedef int(*funcptr)(int a, int b);
_TCHAR _0[MAX_PATH];


/* 目录处理 */
BOOL ifExistFile(LPCTSTR lpPath, BOOL dwFilter)//检查文件是否存在
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(lpPath, &fd);
	BOOL bFilter = (FALSE == dwFilter) ? TRUE : fd.dwFileAttributes & dwFilter;
	BOOL RetValue = ((hFind != INVALID_HANDLE_VALUE) && bFilter) ? TRUE : FALSE;
	FindClose(hFind);
	return RetValue;
}

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

Status CatChildDirectoryA(char* source1, char* source2, size_t _size_, char* buffer)//连接子目录
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


/* main 函数 */
int main()
{
	/* 程序初始化 */
	system("chcp 936&title DLL 加载与管道测试工具&color e&cls");
	setlocale(LC_CTYPE, "");
	GetModuleFileName(NULL, _0, MAX_PATH);
	_TCHAR buffer[MAX_PATH] = { 0 }, DllName[16] = L"GooseBtDll.dll";
	GetParentDirectory(_0, MAX_PATH, buffer);
	if (CatChildDirectory(buffer, DllName, MAX_PATH, buffer) == ERROR)
	{
		cout << "\a错误：获取 dll 绝对路径失败！" << endl;
		system("pause>nul&cls");
		return EXIT_FAILURE;
	}
	if (!ifExistFile(buffer, FALSE))
	{
		cout << "\a错误：系统找不到指定文件。" << endl;
		system("pause>nul&cls");
		return EXIT_FAILURE;
	}

	/* DLL 加载代码 */
	HMODULE hDLL = LoadLibrary(buffer);
	if (hDLL)
	{
		funcptr Add = (funcptr)GetProcAddress(hDLL, "Add");
		if (Add)
			cout << "1 + 1 = " << Add(1, 1) << endl;
		else
		{
			cout << "\a错误：动态链接库丢失条目 Add！" << endl;
			system("pause>nul&cls");
			return EXIT_FAILURE;
		}
	}
	else
	{
		cout << "\a错误：加载 dll 失败！" << endl;
		system("pause>nul&cls");
		return EXIT_FAILURE;
	}
	FreeLibrary(hDLL);
	system("pause>nul&cls");
	
	/* 命名管道 */
	for (;;)
	{
		HANDLE h_pipe;
		char buf_msg[BUF_SIZE] = "尝试传入测试信息，该消息为测试信息，请勿回复。";
		cout << "输入些消息以发送：" << endl;
		rewind(stdin);
		fflush(stdin);
		fgets(buf_msg, sizeof(buf_msg), stdin);
		rewind(stdin);
		fflush(stdin);
		if (strcmp(buf_msg,"\n") == 0)
		{
			cout << "消息为空，无需发送。" << endl << endl;
			continue;
		}
		if (buf_msg[strlen(buf_msg) - 1] == '\n')
			buf_msg[strlen(buf_msg) - 1] = 0;
		DWORD num_rcv;//实际接收到的字节数
		cout << "尝试连接命名管道，请稍候。" << endl;
		if (::WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER))//连接命名管道
		{
			h_pipe = ::CreateFile(PIPE_NAME, GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);//打开指定命名管道
			if (h_pipe == INVALID_HANDLE_VALUE)
			{
				cout << "打开命名管道失败，其错误代码为：" << ::GetLastError() << "，请按任意键退出。" << endl;
				::system("pause>nul");
				return EXIT_FAILURE;
			}
			else
			{
				if (::WriteFile(h_pipe, buf_msg, BUF_SIZE, &num_rcv, nullptr))
					cout << "信息发送成功！" << endl << endl;
				else
				{
					cout << "信息发送失败，其错误代码为：" << ::GetLastError() << "，请按任意键退出。" << endl;
					::CloseHandle(h_pipe);
					::system("pause>nul");
					return EXIT_FAILURE;
				}
			}
			::CloseHandle(h_pipe);
		}
		else
			cout << "连接管道失败，请检查服务端是否正常运行。" << endl << endl;
	}
	::system("pause>nul");
	return EXIT_SUCCESS;
}