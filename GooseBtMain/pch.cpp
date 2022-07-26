// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"
#include <iostream>

#ifdef PIPE_NAME
#undef PIPE_NAME
#endif//重要
#define PIPE_NAME L"\\\\.\\pipe\\GooseBtPipe"
#ifdef PIPE_IN
#undef PIPE_IN
#endif//重要
#define PIPE_IN L"\\\\.\\pipe\\GooseBtState"
#ifndef BUF_SIZE
#define BUF_SIZE 1024// 2 ** 10 == 1024
#endif
#ifndef RemoteTime
#define RemoteTime 200
#endif

short int RtTime = NULL;
BOOL SendMsg(char* msg)
{
	HANDLE h_pipe;
	DWORD num_rcv;//实际接收到的字节数
	
	if (::WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER))//连接命名管道
	{
		h_pipe = ::CreateFile(PIPE_NAME, GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);//打开指定命名管道
		if (h_pipe == INVALID_HANDLE_VALUE)
			return FALSE;
		else
		{
			if (::WriteFile(h_pipe, msg, BUF_SIZE, &num_rcv, nullptr))
			{
				Sleep(RemoteTime);
				::CloseHandle(h_pipe);
				return TRUE;
			}
			else
			{
				::CloseHandle(h_pipe);
				return FALSE;
			}
		}
	}
	return FALSE;
}

BOOL ApplySettings(short int MonLevel)
{
	char buf_msg[BUF_SIZE] = "Set RemoteState ", tmp[10] = { 0 };
	_itoa_s(MonLevel, tmp, 10);
	strcat_s(buf_msg, tmp);
	return SendMsg(buf_msg);
}
// 当使用预编译的头时，需要使用此源文件，编译才能成功。

short int pipe()//命名管道
{
	/* 初始化服务端代码 */
	HANDLE h_pipe;
	char buf_msg[BUF_SIZE] = { 0 }, Cchar[MAX_PATH] = { 0 };
	DWORD num_rcv;//实际接收到的字节数
	///* 检测命名管道是否已存在 */
	//if (::WaitNamedPipe(PIPE_IN, NMPWAIT_WAIT_FOREVER))
	//	return EOF;

	/* 创建命名管道 GooseBtPipe，消息只能从客户端流向服务器，读写数据采用阻塞模式、字节流形式，超时值置为 0 表示采用默认的 50 毫秒。 */
	h_pipe = ::CreateNamedPipe(PIPE_IN, PIPE_ACCESS_INBOUND, PIPE_READMODE_BYTE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, BUF_SIZE, BUF_SIZE, 0, nullptr);
	if (h_pipe == INVALID_HANDLE_VALUE)
		return EOF;

	/* 等待命名管道客户端连接 */
	if (::ConnectNamedPipe(h_pipe, nullptr))
	{
		memset(buf_msg, 0, BUF_SIZE);
		if (::ReadFile(h_pipe, buf_msg, BUF_SIZE, &num_rcv, nullptr))//读取数据
		{
			::CloseHandle(h_pipe);//关闭管道
			Sleep(RemoteTime);
			return (short int)(buf_msg[0] - 48);
		}
		else
		{
			::CloseHandle(h_pipe);//关闭管道
			if (RtTime > NULL)
			{
				Sleep(RemoteTime);
				return pipe();
			}
			else
				return EOF;
		}
	}
	else
		return EOF;
}

short int GetRemoteState()//获取远程状态
{
	char buf_msg[BUF_SIZE] = "Request RemoteState";
	RtTime = 3;
	if (SendMsg(buf_msg))
		return pipe();
	else
		return EOF;
}