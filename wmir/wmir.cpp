#include <iostream>
#ifdef WIN32
#include <Windows.h>
#elif (defined _WIN64 || defined WIN64)
#include <Windows.h>
#ifndef _WIN32
#define _WIN32 1
#endif
#ifndef WIN32
#define WIN32 1
#endif
#endif//_Windows_H
#include <tchar.h>
#ifndef _wmir_H
#define _wmir_H
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EOF
#define EOF (-1)
#endif
#endif//_whir_H
using namespace std;


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


class WmirType
{
private:
	_TCHAR uOpType = '0';// flag = 0
	_TCHAR uPath[MAX_PATH] = { 0 };// flag = 1
	_TCHAR uSubKeyType = '0';// flag = 2 (fveh)
	_TCHAR uSubKey[MAX_PATH] = { 0 };// flag = 3
	_TCHAR uValueType = '0';// flag = 4
	_TCHAR uValue[MAX_PATH] = { 0 };// flag = 5

public:
	_TCHAR allMsg[MAX_PATH << 2] = { 0 };
	void setOpType(_TCHAR tch)
	{
		this->uOpType = tch;
		return;
	}
	void setPath(_TCHAR path[])
	{
		lstrcpy(this->uPath, path);
		return;
	}
	bool fromCommandline(int argc, _TCHAR* argv[])
	{
		lstrcpy(uPath, argv[2]);
		short int flag = 0;
		for (int i = 3; i < argc; ++i)
		{
			if (0 == flag
				&& (
					(0 == _wcsicmp(argv[i], L"/v") || 0 == _wcsicmp(argv[i], L"-v"))
					|| (0 == _wcsicmp(argv[i], L"/vk") || 0 == _wcsicmp(argv[i], L"-vk"))
				)
			)
			{
				if ('0' != this->uSubKeyType)
					wcout << L"警告：设定为项或键类型的参数重复！" << endl;
				this->uSubKeyType = 'k';
				flag = 3;//下一个命令行参数应当为键名称
			}
			else if (0 == flag && (0 == _wcsicmp(argv[i], L"/ve") || 0 == _wcsicmp(argv[i], L"-ve")))
			{
				if ('0' != this->uSubKeyType)
					wcout << L"警告：设定为项或键类型的参数重复！" << endl;
				this->uSubKeyType = 'e';
				lstrcpy(this->uSubKey, L"");//注意没有下一个参数
			}
			else if (0 == flag && (0 == _wcsicmp(argv[i], L"/vf") || 0 == _wcsicmp(argv[i], L"-vf")))
			{
				if ('0' != this->uSubKeyType)
					wcout << L"警告：设定为项或键类型的参数重复！" << endl;
				this->uSubKeyType = 'f';
				lstrcpy(this->uSubKey, L"");//注意没有下一个参数
			}
			else if (0 == flag && (0 == _wcsicmp(argv[i], L"/vi") || 0 == _wcsicmp(argv[i], L"-vi")))
			{
				if ('0' != this->uSubKeyType)
					wcout << L"警告：设定为项或键类型的参数重复！" << endl;
				this->uSubKeyType = 'i';
				flag = 3;//下一个命令行参数应当为键名称
			}
			else if (0 == flag && (0 == _wcsicmp(argv[i], L"/t") || 0 == _wcsicmp(argv[i], L"-t")))
			{
				if ('0' != this->uValueType)
					wcout << L"警告：指定值类型的参数重复！" << endl;
				flag = 4;//下一个命令行参数应当为值类型
			}
			else if (0 == flag && (0 == _wcsicmp(argv[i], L"/d") || 0 == _wcsicmp(argv[i], L"-d")))
			{
				if (0 != this->uValue[0])
					wcout << L"警告：重复设定值！" << endl;
				flag = 5;//下一个命令行参数应当为值内容
			}
			else
				switch (flag)
				{
				case 3://键名称
					lstrcpy(this->uSubKey, argv[i]);
					flag = 0;//重置参数
					break;
				case 4://值类型
					if (0 == _wcsicmp(argv[i], L"REG_SZ") || 0 == _wcsicmp(argv[i], L"S"))
						this->uValueType = 'S';
					else if (0 == _wcsicmp(argv[i], L"REG_BINARY") || 0 == _wcsicmp(argv[i], L"B"))
						this->uValueType = 'B';
					else if (0 == _wcsicmp(argv[i], L"REG_DWORD") || 0 == _wcsicmp(argv[i], L"D"))
						this->uValueType = 'D';
					else if (0 == _wcsicmp(argv[i], L"REG_QWORD") || 0 == _wcsicmp(argv[i], L"Q"))
						this->uValueType = 'Q';
					else if (0 == _wcsicmp(argv[i], L"REG_MULTI_SZ") || 0 == _wcsicmp(argv[i], L"M"))
						this->uValueType = 'M';
					else if (0 == _wcsicmp(argv[i], L"REG_EXPAND_SZ") || 0 == _wcsicmp(argv[i], L"E"))
						this->uValueType = 'E';
					else if (0 == _wcsicmp(argv[i], L"REG_NONE") || 0 == _wcsicmp(argv[i], L"N"))
						this->uValueType = 'N';
					else
						return false;
					flag = 0;//重置参数
					break;
				case 5://值内容
					lstrcpy(this->uValue, argv[i]);
					flag = 0;//重置参数
					break;
				default://非法参数
					return false;
				}
		}
		if (0 != flag)//参数没处理完
			return false;
		if ('0' == this->uSubKeyType)
			this->uSubKeyType = 'f';//默认值
		if ('0' == this->uValueType)
			this->uValueType = 'S';//默认值
		return true;
	}
	void generateDriverMsg()
	{
		for (int i = 0; i < MAX_PATH << 2; ++i)//清空
			this->allMsg[i] = 0;
		wsprintf(allMsg, L"%c\n%s\n%c\n%s\n%c\n%s", this->uOpType, this->uPath, this->uSubKeyType, this->uSubKey, this->uValueType, this->uValue);
#ifdef _DEBUG
		wcout << L"uOpType = \'" << this->uOpType << "\'" << endl;
		wcout << L"uPath = \"" << this->uPath << "\"" << endl;
		wcout << L"uSubKeyType = \'" << this->uSubKeyType << "\'" << endl;
		wcout << L"uSubKey = \"" << this->uSubKey << "\"" << endl;
		wcout << L"uValueType = \'" << this->uValueType << "\'" << endl;
		wcout << L"uValue = \"" << this->uValue << "\"" << endl << endl;
#endif
		return;
	}
};

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
		wcout << L"\a错误：与驱动通讯时发生异常！" << endl;
		return EXIT_FAILURE;
	}

	/* 发送指令 */
	char tmp[MAX_PATH << 2] = { 0 };
	TcharToChar(msg, tmp);
	if (DeviceIoControl(device, CWK_DVC_SEND_STR, tmp, (DWORD)(strlen(tmp)), NULL, 0, &ret_len, 0))
		wcout << L"向驱动发送操作注册表请求成功！" << endl;
	else
	{
		wcout << L"\a错误：向驱动发送信息时发生异常！" << endl;
		ret = EXIT_FAILURE;
	}
	
	CloseHandle(device);//记得关闭
	return ret;
}


/* 帮助 */
void addHelp()
{
	wcout << endl << L"描述：向注册表中增加项或键。" << endl;
	wcout << endl << L"wmir add [注册表路径] [项或键类型] ..." << endl << endl;
	wcout << L"可用参数列表：" << endl;
	wcout << L"\t/v [键名称]|/ve|/vf|/vi [项名称]\t项或键类型" << endl;
	wcout << L"\t/t [值类型]\t\t\t\t值类型" << endl;
	wcout << L"\t/d [值内容]\t\t\t\t值内容" << endl << endl;
	wcout << L"可用的键类型：" << endl;
	wcout << L"\t/v [键名称]\t指定注册表路径中的键" << endl;
	wcout << L"\t/ve\t\t指定注册表路径中的默认键" << endl;
	wcout << L"\t/vf\t\t指定当前注册表路径（默认）" << endl;
	wcout << L"\t/vi [项名称]\t指定注册表路径中的子项" << endl << endl;
	wcout << L"可用的值类型：" << endl;
	wcout << L"\tS = REG_SZ\t\t字符串值（默认）" << endl;
	wcout << L"\tB = REG_BINARY\t\t二进制值" << endl;
	wcout << L"\tD = REG_DWORD\t\t32 位 DWORD 值" << endl;
	wcout << L"\tQ = REG_QWORD\t\t64 位 QWORD 值" << endl;
	wcout << L"\tM = REG_MULTI_SZ\t多字符串值" << endl;
	wcout << L"\tE = REG_EXPAND_SZ\t可扩充字符串值" << endl;
	wcout << L"\tN = REG_NONE\t\t无类型值" << endl << endl;
	wcout << L"示例：" << endl;
	wcout << L"\twmir add HKLM\\SoftWare\\GooseBt /vf" << endl;
	wcout << L"\twmir add HKLM\\SoftWare\\GooseBt /ve /d \"GooseBt\"" << endl;
	wcout << L"\twmir add HKLM\\SoftWare\\GooseBt /vi GooseBt" << endl;
	wcout << L"\twmir add HKLM\\SoftWare\\GooseBt /v wmir /t D /d 3" << endl << endl;
	return;
}

void delHelp()
{
	wcout << endl << L"描述：向注册表中删除项或键。" << endl;
	wcout << endl << L"wmir del [注册表路径] [项或键类型] ..." << endl << endl;
	wcout << L"可用参数列表：" << endl;
	wcout << L"\t/v [键名称]|/ve|/vf|/vi [项名称]\t项或键类型" << endl << endl;
	wcout << L"可用的键类型：" << endl;
	wcout << L"\t/v [键名称]\t指定注册表路径中的键" << endl;
	wcout << L"\t/ve\t\t指定注册表路径中的默认键" << endl;
	wcout << L"\t/vf\t\t指定当前注册表" << endl;
	wcout << L"\t/vi [项名称]\t指定注册表路径中的子项" << endl << endl;
	wcout << L"示例：" << endl;
	wcout << L"\twmir del HKLM\\SoftWare\\GooseBt /v wmir" << endl;
	wcout << L"\twmir del HKLM\\SoftWare\\GooseBt /vi GooseBt" << endl;
	wcout << L"\twmir del HKLM\\SoftWare\\GooseBt /ve" << endl;
	wcout << L"\twmir del HKLM\\SoftWare\\GooseBt /vf" << endl << endl;
	wcout << L"请注意，删除默认键仅作值类型还原和值清空处理。" << endl << endl;
	return;
}

void setHelp()
{
	wcout << endl << L"描述：向注册表中设置项或键。" << endl;
	wcout << endl << L"wmir set [注册表路径] [键类型] ..." << endl << endl;
	wcout << L"可用参数列表：" << endl;
	wcout << L"\t/v [键名称]|/ve\t\t键类型" << endl;
	wcout << L"\t/t [值类型]\t\t值类型" << endl;
	wcout << L"\t/d [值内容]\t\t值内容" << endl << endl;
	wcout << L"可用的键类型（必需参数）：" << endl;
	wcout << L"\t/v [键名称]\t指定注册表路径中的键" << endl;
	wcout << L"\t/ve\t\t指定注册表路径中的默认键" << endl << endl;
	wcout << L"可用的值类型：" << endl;
	wcout << L"\tS = REG_SZ\t\t字符串值（默认）" << endl;
	wcout << L"\tB = REG_BINARY\t\t二进制值" << endl;
	wcout << L"\tD = REG_DWORD\t\t32 位 DWORD 值" << endl;
	wcout << L"\tQ = REG_QWORD\t\t64 位 QWORD 值" << endl;
	wcout << L"\tM = REG_MULTI_SZ\t多字符串值" << endl;
	wcout << L"\tE = REG_EXPAND_SZ\t可扩充字符串值" << endl;
	wcout << L"\tN = REG_NONE\t\t无类型值" << endl << endl;
	wcout << L"示例：" << endl;
	wcout << L"\twmir set HKLM\\SoftWare\\GooseBt /ve /d \"GooseBt\"" << endl;
	wcout << L"\twmir set HKLM\\SoftWare\\GooseBt /v wmir /t D /d 3" << endl << endl;
	return;
}

void queryHelp()
{
	wcout << endl << L"描述：向注册表中查询一级子项（请使用 DebugView 接收）。" << endl;
	wcout << endl << L"wmir query [注册表路径] [项类型] ..." << endl << endl;
	wcout << L"可用参数列表：" << endl;
	wcout << L"\t/vf|/vi [项名称]\t项类型" << endl << endl;
	wcout << L"可用的项类型：" << endl;
	wcout << L"\t/vf\t\t指定当前注册表" << endl;
	wcout << L"\t/vi [项名称]\t指定注册表路径中的子项" << endl << endl;
	wcout << L"示例：" << endl;
	wcout << L"\twmir query HKLM\\SoftWare\\GooseBt /vf" << endl;
	wcout << L"\twmir query HKLM\\SoftWare\\GooseBt /vi GooseBt" << endl << endl;
	return;
}

void showHelp()
{
	wcout << endl << L"描述：综合注册表管理器。" << endl;
	wcout << endl << L"wmir [首参数] [注册表路径] [项或键类型] ..." << endl << endl;
	wcout << L"可用首参数列表：" << endl;
	wcout << L"\tadd\t\t增" << endl;
	wcout << L"\tdel\t\t删" << endl;
	wcout << L"\tset\t\t改" << endl;
	wcout << L"\tquery\t\t查" << endl << endl;
	wcout << L"请注意，本工具仅支持在内核模式下操作注册表。" << endl;
	wcout << L"要在非内核下操作注册表，请使用系统自带的 reg 命令。" << endl;
	wcout << L"重复参数以最后一个为准，冗余参数会被自动忽略。" << endl;
	wcout << L"有关 wmir 的更多信息，请键入“wmir [首参数] /?”并回车。" << endl << endl;
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
		WmirType wrt;
		if (0 == _wcsicmp(argv[1], L"add"))
			wrt.setOpType(L'a');
		else if (0 == _wcsicmp(argv[1], L"del") || 0 == _wcsicmp(argv[1], L"delete"))
			wrt.setOpType(L'd');
		else if (0 == _wcsicmp(argv[1], L"set"))
			wrt.setOpType(L's');
		else if (0 == _wcsicmp(argv[1], L"query"))
			wrt.setOpType(L'q');
		else
		{
			wcout << L"\a错误：命令行首参数不正确！" << endl;
			return EOF;
		}
		if (!wrt.fromCommandline(argc, argv))
		{
			wcout << L"\a错误：命令行参数不正确！" << endl;
			return EOF;
		}
		wrt.setPath(argv[2]);
		wrt.generateDriverMsg();
		DriverConnector(wrt.allMsg, CWK_DEV_SYM_R);
		return EXIT_SUCCESS;
	}
	else if (3 == argc
		&& (
			0 == _wcsicmp(argv[2], L"/?") || 0 == _wcsicmp(argv[2], L"-?")
			|| 0 == _wcsicmp(argv[2], L"/help") || 0 == _wcsicmp(argv[2], L"-help")
		)
	)//子帮助类
	{
		if (0 == _wcsicmp(argv[1], L"add"))
			addHelp();
		else if (0 == _wcsicmp(argv[1], L"del") || 0 == _wcsicmp(argv[1], L"delete"))
			delHelp();
		else if (0 == _wcsicmp(argv[1], L"set"))
			setHelp();
		else if (0 == _wcsicmp(argv[1], L"query"))
			queryHelp();
		else
		{
			wcout << L"\a错误：命令行首参数不正确！" << endl;
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