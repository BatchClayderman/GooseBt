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
#include <thread>
#include <mmdeviceapi.h> 
#include <endpointvolume.h>
#include <audioclient.h>
#include <tchar.h>
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EOF
#define EOF (-1)
#endif
#ifndef NULL
#define NULL 0
#endif
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#ifndef BUF_SIZE
#define BUF_SIZE 1024// 2 ** 10 == 1024
#endif
#ifndef RemoteTime
#define RemoteTime 300
#endif
#ifndef gapTime
#define gapTime 6000000
#endif
#ifdef PIPE_NAME
#undef PIPE_NAME
#endif//重要
#define PIPE_NAME L"\\\\.\\pipe\\GooseBtPipe"
#ifdef PIPE_IN
#undef PIPE_IN
#endif//重要
#define PIPE_IN L"\\\\.\\pipe\\GooseBtState"
#ifdef SYMBOL_LINK_NAME
#undef SYMBOL_LINK_NAME
#endif
#define SYMBOL_LINK_NAME L"\\\\.\\KMDFThread"
#ifndef MusicName
#define MusicName "Until the Last Moment.mp3"
#endif
#ifndef LogName
#define LogName "GooseBtMonitor.log"
#endif
#ifndef ScannerExeName
#define ScannerExeName "Scanner.exe"
#endif
#ifndef White
#define White 0xF
#endif//文字默认输出
#ifndef Yellow
#define Yellow 0xE
#endif//客户端提示信息输出
#ifndef Red
#define Red 0xC
#endif//服务端系统错误信息输出
#ifndef Green
#define Green 0xA
#endif//服务端系统正常信息输出
#pragma comment(lib,"Winmm.lib")
using namespace std;
typedef BOOL APIType;
int clientCount = NULL;
bool Outing = false;
short int RemoteState = 2;


/* string 类型子函数 */
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
	TimeStr += string(tmpStr) + ":";
	_itoa_s(sys.wMinute, tmpStr, 10);
	TimeStr += string(tmpStr) + ":";
	_itoa_s(sys.wSecond, tmpStr, 10);
	TimeStr += string(tmpStr) + ".";
	_itoa_s(sys.wMilliseconds, tmpStr, 10);
	TimeStr += string(tmpStr) + "\t";
	return TimeStr;
#else
	return "";
#endif
}


/* APIType 类型子函数 */
APIType LogDown(string str)//Log
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

APIType echo(string str, short int x)//输出流
{
	while (Outing);
	Outing = true;
	if (x >= 0x0 && x <= 0xF)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), x);
	cout << str << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), White);//归还默认色
	if (x == Red || x == Green)
		LogDown(str);
	Outing = false;
	return TRUE;
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

APIType IsAboveVistaVersion(DWORD Win)//判断系统版本是否为 Win 或以上
{
	if (Win <= _WIN32_WINNT)//返回的是 bool
		return TRUE;
	else
		return FALSE;
}

APIType GetAdminPermission(_TCHAR* Percent0, _TCHAR* Percent1, BOOL bShow)//以管理员权限启动
{
	SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
	sei.lpVerb = TEXT("runas");
	sei.lpFile = Percent0;
	sei.lpParameters = Percent1;
	if (bShow)
		sei.nShow = SW_SHOWNORMAL;
	else
		sei.nShow = SW_HIDE;
	return (ShellExecuteEx(&sei) ? TRUE : FALSE);
}

APIType OpenExeW(_TCHAR* exe, _TCHAR* param, _TCHAR* path, bool bShow, bool openfirst)
{
	if (IsAboveVistaVersion(_WIN32_WINNT_WIN7) && !IsAdmin())
		return GetAdminPermission(exe, param, bShow);
	HINSTANCE result = 0;
	int ishow = SW_SHOW;

	if (bShow)
		ishow = SW_SHOW;
	else
		ishow = SW_HIDE;

	if (openfirst)
		result = ::ShellExecuteW(NULL, TEXT("open"), param, NULL, NULL, ishow);
	else
		result = ::ShellExecuteW(NULL, NULL, exe, param, path, ishow);
	return (long long int)result <= HINSTANCE_ERROR ? FALSE : TRUE;
}


/* 并发编程函数 */
BOOL SendMsg(short int msg)
{
	HANDLE h_pipe;
	DWORD num_rcv;//实际接收到的字节数

	if (::WaitNamedPipe(PIPE_IN, NMPWAIT_WAIT_FOREVER))//连接命名管道
	{
		h_pipe = ::CreateFile(PIPE_IN, GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);//打开指定命名管道
		if (h_pipe == INVALID_HANDLE_VALUE)
			return FALSE;
		else
		{
			char Mesg[MAX_PATH] = { 0 };
			Mesg[0] = (char)(msg + 48);
			if (::WriteFile(h_pipe, Mesg, MAX_PATH, &num_rcv, nullptr))
			{
				::CloseHandle(h_pipe);
				return TRUE;
			}
			else
			{
				::CloseHandle(h_pipe);
				return FALSE;
			}
		}
		::CloseHandle(h_pipe);
	}
	return FALSE;
}

int pipe()//命名管道
{
	/* 初始化服务端代码 */
	HANDLE h_pipe;
	char buf_msg[BUF_SIZE] = { 0 }, Cchar[MAX_PATH] = { 0 };
	DWORD num_rcv;//实际接收到的字节数
	string EString = "";

	for (;;)
	{
		/* 检测命名管道是否已存在 */
		if (::WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER))
		{
			echo("命名管道已存在，无需重复创建，请按任意键退出本程序。", Yellow);
			::system("pause>nul");
			exit(EXIT_SUCCESS);//必须使用 exit
		}

		/* 创建命名管道 GooseBtPipe，消息只能从客户端流向服务器，读写数据采用阻塞模式、字节流形式，超时值置为 0 表示采用默认的 50 毫秒。 */
		h_pipe = ::CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_INBOUND, PIPE_READMODE_BYTE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, BUF_SIZE, BUF_SIZE, 0, nullptr);
		if (h_pipe == INVALID_HANDLE_VALUE)
		{
			_itoa_s(::GetLastError(), Cchar, 10);//AvoidBug
			EString = "错误：命名管道创建失败，请按任意键退出。（" + string(Cchar);
			EString += "）";
			echo(EString, Red);
			::system("pause>nul");
			return EXIT_FAILURE;
		}
		else
			echo("信息：命名管道创建成功，正在等待客户端连接。", Green);

		/* 等待命名管道客户端连接 */
		if (::ConnectNamedPipe(h_pipe, nullptr))
		{
			clientCount++;
			_itoa_s(clientCount, Cchar, 10);
			EString = "信息：检测到客户端连入，已有 " + string(Cchar);
			EString += " 个客户端成功连入。";
			echo(EString, Green);
			memset(buf_msg, 0, BUF_SIZE);
			if (::ReadFile(h_pipe, buf_msg, BUF_SIZE, &num_rcv, nullptr))//读取数据
			{
				echo("接收到客户端信息：", Yellow);
				echo(buf_msg, White);
				if (strcmp(buf_msg, "Set RemoteState 0") == 0)
				{
					RemoteState = 0;
					string Resp = "模式更改：";
					Resp += (char)(RemoteState + 48);
					echo(Resp.c_str(), Green);
				}
				else if (strcmp(buf_msg, "Set RemoteState 1") == 0)
				{
					RemoteState = 1;
					string Resp = "模式更改：";
					Resp += (char)(RemoteState + 48);
					echo(Resp.c_str(), Green);
				}
				else if (strcmp(buf_msg, "Set RemoteState 2") == 0)
				{
					RemoteState = 2;
					string Resp = "模式更改：";
					Resp += (char)(RemoteState + 48);
					echo(Resp.c_str(), Green);
				}
				else if (strcmp(buf_msg, "Set RemoteState 3") == 0)
				{
					RemoteState = 3;
					string Resp = "模式更改：";
					Resp += (char)(RemoteState + 48);
					echo(Resp.c_str(), Green);
				}
				else if (strcmp(buf_msg, "Set RemoteState 4") == 0)
				{
					RemoteState = 4;
					string Resp = "模式更改：";
					Resp += (char)(RemoteState + 48);
					echo(Resp.c_str(), Green);
				}
				else if (strcmp(buf_msg, "Set RemoteState 5") == 0)
				{
					RemoteState = 5;
					string Resp = "模式更改：";
					Resp += (char)(RemoteState + 48);
					echo(Resp.c_str(), Green);
				}
				else if (strcmp(buf_msg, "Request RemoteState") == 0)
				{
					Sleep(RemoteTime);
					if (SendMsg(RemoteState))
					{
						string Resp = "回送信息：";
						Resp += (char)(RemoteState + 48);
						echo(Resp.c_str(), Green);
					}
					else
					{
						Sleep(RemoteTime);
						if (SendMsg(RemoteState))
						{
							string Resp = "回送信息：";
							Resp += (char)(RemoteState + 48);
							echo(Resp.c_str(), Green);
						}
						else
						{
							Sleep(RemoteTime);
							if (SendMsg(RemoteState))
							{
								string Resp = "回送信息：";
								Resp += (char)(RemoteState + 48);
								echo(Resp.c_str(), Green);
							}
							else
							{
								string Resp = "回送信息失败，请检查远程管道。";
								echo(Resp.c_str(), Red);
							}
						}
					}
				}
			}
			else
			{
				_itoa_s(::GetLastError(), Cchar, 10);//AvoidBug
				EString = "错误：收到客户端消息映射，但读取客户端信息失败！（" + string(Cchar);
				EString += "）";
				echo(EString, Red);
				return EXIT_FAILURE;
			}
			::CloseHandle(h_pipe);//关闭管道
			echo("信息：当前管道已关闭，尝试重新建立管道。\n", Green);
		}
		else
			break;
	}
	return EXIT_SUCCESS;
}

void Monitor()//过滤驱动实时监控
{
	HANDLE hDevice = CreateFile(SYMBOL_LINK_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	for (;;)
	{
		if (hDevice == INVALID_HANDLE_VALUE)
		{
			echo("错误：连接驱动失败，稍后将发起重连！\n", Red);
			Sleep(gapTime);
			hDevice = CreateFile(SYMBOL_LINK_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		}
		else
			break;
	}

	for (;;)
	{
		char buffer[MAX_PATH] = { 0 };
		ULONG size;
		BOOL result = ReadFile(hDevice, buffer, MAX_PATH, &size, NULL);
		if (result)
		{
			if (strcmp(buffer, "STOP") == 0)
			{
				echo("警告：驱动已停止！\n", Yellow);
				break;
			}
			else if (strcmp(buffer, "NULL") == 0)
				continue;
			else
				printf("%s", buffer);
		}
		else
		{
			echo("错误：驱动连接中断！\n", Red);
			break;
		}
	}
	CloseHandle(hDevice);
	return;
}

void Scanner()//定时查杀
{
	for (;;)
	{
		Sleep(gapTime);
		_TCHAR Percent0[MAX_PATH] = { 0 }, Percent1[] = L"*";
		string str = GF_GetEXEPath();
		str += ScannerExeName;
#ifdef UNICODE
		_stprintf_s(Percent0, MAX_PATH, _T("%S"), str.c_str());//%S宽字符
#else
		_stprintf_s(Percent0, MAX_PATH, _T("%s"), str.c_str());//%s单字符
#endif
		if (OpenExeW(Percent0, Percent1, NULL, false, false))
			echo("信息：定时查杀已启动。", Green);
		else
		{
			echo("错误：定时查杀运行失败！", Red);
			MessageBox(NULL, TEXT("定时查杀运行失败，请检查您的计算机是否存在异常！"), TEXT("GooseBt 第四代反病毒软件"), MB_OK | MB_ICONERROR | MB_TOPMOST);
		}
	}
	return;
}

bool SetVolum(short int volume)
{
	if (volume > 100 || volume < 0)
		return false;
	HRESULT hr;
	IMMDeviceEnumerator* pDeviceEnumerator = NULL;
	IMMDevice* pDevice = NULL;
	IAudioEndpointVolume* pAudioEndpointVolume = NULL;
	IAudioClient* pAudioClient = NULL;

	try
	{
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pDeviceEnumerator);
		if (FAILED(hr))
			throw "CoCreateInstance";
		hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
		if (FAILED(hr))
			throw "GetDefaultAudioEndpoint";
		hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pAudioEndpointVolume);
		if (FAILED(hr))
			throw "pDevice->Active";
		hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
		if (FAILED(hr))
			throw "pDevice->Active";

		float fVolume = volume / 100.0f;
		hr = pAudioEndpointVolume->SetMasterVolumeLevelScalar(fVolume, &GUID_NULL);
		if (FAILED(hr))
			throw "SetMasterVolumeLevelScalar";

		pAudioClient->Release();
		pAudioEndpointVolume->Release();
		pDevice->Release();
		pDeviceEnumerator->Release();
		return true;
	}
	catch (...)
	{
		if (pAudioClient)
			pAudioClient->Release();
		if (pAudioEndpointVolume)
			pAudioEndpointVolume->Release();
		if (pDevice)
			pDevice->Release();
		if (pDeviceEnumerator)
			pDeviceEnumerator->Release();
		throw;
	}
	return false;
}

void Alert()
{
	//SetVolum(10);
	//return;
	FILE* pf = nullptr;
	string MusicPath = GF_GetEXEPath();
	MusicPath += MusicName;
	fopen_s(&pf, MusicPath.c_str(), "rb");
	if (pf == NULL)
	{
		echo("错误：文件不存在！", Red);
		return;
	}

	echo("信息：音乐服务已启动。", Green);
#ifdef UNICODE
	_TCHAR mciCmd[MAX_PATH << 1] = L"open \"", wc[MAX_PATH] = { 0 };
#ifdef UNICODE
	_stprintf_s(wc, MAX_PATH, _T("%S"), MusicPath.c_str());//%S宽字符
#else
	_stprintf_s(wc, MAX_PATH, _T("%s"), MusicPath.c_str());//%s单字符
#endif
	lstrcat(mciCmd, wc);
	lstrcat(mciCmd, L"\" alias MySong");
#else
	char mciCmd[MAX_PATH << 1] = "open \"";
	strcat_s(mciCmd, GF_GetEXEPath().c_str());
	strcat_s(mciCmd, MusicName);
	strcat_s(mciCmd, "\" alias MySong");
#endif
	//"open \"D:\\星空\\云启发音乐\\演奏版\\心兰相随.mp3\" alias MySong"
	mciSendString(mciCmd, NULL, 0, NULL);
	mciSendString(TEXT("play MySong repeat wait"), NULL, 0, NULL);

	/*
	控制命令：
	open	play	pause
	resume	close	loop
	*/
	/*
	for (;;)
	{
		rewind(stdin);
		fflush(stdin);
		char input = getchar();
		rewind(stdin);
		fflush(stdin);
		switch (input)
		{
		case 'p'://暂停
			mciSendString(TEXT("pause MySong"), NULL, 0, NULL);
			break;
		case 'c'://继续
			mciSendString(TEXT("resume MySong"), NULL, 0, NULL);
			break;
		case 'w'://关闭
			mciSendString(TEXT("close MySong"), NULL, 0, NULL);
			break;
		case 'l'://循环
			mciSendString(TEXT("loop MySong"), NULL, 0, NULL);
			break;
		default:
			break;
		}
	}
	*/
	echo("信息：音乐服务已停止。", Green);
	return;
}


/* main 函数 */
int main()
{
	SetConsoleTitleW(L"GooseBt 反病毒软件实时监控模块");
	setlocale(LC_CTYPE, "");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), White);//设置默认色
	thread t1(pipe);//启动命名管道
	thread t2(Monitor);//启动文件过滤驱动实时监控
	thread t3(Scanner);//启动定时扫描
	thread t4(Alert);//播报病毒
	while (!(t1.joinable() && t2.joinable() && t3.joinable() && t4.joinable()));//等待线程退出
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	return EXIT_SUCCESS;
}