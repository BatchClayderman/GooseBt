#include <ntifs.h>
#include <ntstrsafe.h>
#include "Listener.h"
#ifndef _ZwOpReg_H
#define _ZwOpReg_H "ZwOpReg"
#ifndef NULL
#define NULL ((void *)0)
#endif
#ifndef VOID
#defnie VOID void
#endif
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#ifndef INFO_SEP
#define INFO_SEP '\n'
#endif
#ifndef INFO_MEMBER_CNT
#define INFO_MEMBER_CNT 6
#endif
#ifndef INFO_SEP_CNT
#define INFO_SEP_CNT 5
#endif
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifdef DeviceName
#undef DeviceName
#endif
#define DeviceName L"\\Device\\GooseBtZwOpReg"
#ifdef CWK_CDO_SYB_NAME
#undef CWK_CDO_SYB_NAME
#endif
#define CWK_CDO_SYB_NAME  L"\\??\\GooseBtZwOpReg"//在 R0 层的链接符号

/* 从应用层给驱动发送一个字符串 */
#ifndef CWK_DVC_SEND_STR
#define CWK_DVC_SEND_STR       \
	(ULONG)CTL_CODE(           \
	FILE_DEVICE_UNKNOWN,       \
	0x911,METHOD_BUFFERED,     \
	FILE_WRITE_DATA            \
)
#endif

/* 从驱动读取一个字符串 */
#ifndef CWK_DVC_RECV_STR
#define CWK_DVC_RECV_STR       \
	(ULONG)CTL_CODE(           \
	FILE_DEVICE_UNKNOWN,       \
	0x912,METHOD_BUFFERED,     \
	FILE_READ_DATA             \
)
#endif
#endif//_ZwOpReg_H
PDEVICE_OBJECT g_cdo = NULL;//控制设备


/** 结构体 **/
typedef struct
{
	char uOpType;// flag = 0
	char uPath[MAX_PATH];// flag = 1
	char uSubKeyType;// flag = 2 (fveh)
	char uSubKey[MAX_PATH];// flag = 3
	char uValueType;// flag = 4
	char uValue[MAX_PATH];// flag = 5
} ZwOpRegStruct;


/** 字符串处理子函数 **/
/* 是否合法信息 */
BOOLEAN isValidStructer(char buffer[], int newline_index[INFO_MEMBER_CNT], ULONG inlen)
{
	if (inlen >= MAX_PATH << 2)
		return FALSE;
	newline_index[INFO_MEMBER_CNT - 1] = inlen;
	ULONG cnt = 0;
	for (ULONG i = 0; i < inlen; ++i)
		if (INFO_SEP == buffer[i])
		{
			if (cnt >= INFO_SEP_CNT)//分隔符数量太多
				return FALSE;
			newline_index[cnt++] = i;
		}
	if (INFO_SEP_CNT != cnt)//分隔符数量太少
		return FALSE;
	if (newline_index[0] != 1 || newline_index[2] - newline_index[1] != 2 || newline_index[4] - newline_index[3] != 2)//单独的一个字符
		return FALSE;
	for (int i = 1; i < INFO_MEMBER_CNT - 1; ++i)//各个数据长度（从 1 开始检查即可）
		if (newline_index[i + 1] - newline_index[i] > MAX_PATH)
			return FALSE;
	return TRUE;
}

/* R3 转 R0 注册表路径 */
BOOLEAN transReg(char uPath[MAX_PATH])
{
	while ('\\' == uPath[strlen(uPath) - 1])//如果最后一个字符是'\\'则直接去掉
		uPath[strlen(uPath) - 1] = 0;
	while ('\\' == uPath[0])//如果第一个字符是'\\'则直接去掉
	{
		char uTmp[MAX_PATH] = { 0 };
		strcpy_s(uTmp, MAX_PATH, uPath);
		strcpy_s(uPath, MAX_PATH, &uTmp[1]);
	}
	if (strlen(uPath) >= 4)
	{
		if (('H' == uPath[0] || 'h' == uPath[0])
			&& ('K' == uPath[1] || 'k' == uPath[1])
			&& ('L' == uPath[2] || 'l' == uPath[2])
			&& ('M' == uPath[3] || 'm' == uPath[3])
			)
		{
			char uTmp[MAX_PATH] = { 0 };
			strcpy(uTmp, "\\registry\\machine");
			if (strlen(uPath) - 4 + strlen(uTmp) >= MAX_PATH)//不够长
				return FALSE;
			strcat_s(uTmp, MAX_PATH, &uPath[4]);
			strcpy(uPath, uTmp);
			return TRUE;
		}
		else if (('H' == uPath[0] || 'h' == uPath[0])
			&& ('K' == uPath[1] || 'k' == uPath[1])
			&& ('C' == uPath[2] || 'c' == uPath[2])
			&& ('U' == uPath[3] || 'u' == uPath[3])
			)
		{
			char uTmp[MAX_PATH] = { 0 };
			strcpy(uTmp, "\\registry\\user");
			if (strlen(uPath) - 4 + strlen(uTmp) >= MAX_PATH)//不够长
				return FALSE;
			strcat_s(uTmp, MAX_PATH, &uPath[4]);
			strcpy(uPath, uTmp);
			return TRUE;
		}
		else if (('H' == uPath[0] || 'h' == uPath[0])
			&& ('K' == uPath[1] || 'k' == uPath[1])
			&& ('C' == uPath[2] || 'c' == uPath[2])
			&& ('R' == uPath[3] || 'r' == uPath[3])
			)
		{
			char uTmp[MAX_PATH] = { 0 };
			strcpy(uTmp, "\\registry\\root");
			if (strlen(uPath) - 4 + strlen(uTmp) >= MAX_PATH)//不够长
				return FALSE;
			strcat_s(uTmp, MAX_PATH, &uPath[4]);
			strcpy(uPath, uTmp);
			return TRUE;
		}
		else if (('H' == uPath[0] || 'h' == uPath[0])
			&& ('K' == uPath[1] || 'k' == uPath[1])
			&& ('C' == uPath[2] || 'c' == uPath[2])
			&& ('C' == uPath[3] || 'c' == uPath[3])
			)
		{
			char uTmp[MAX_PATH] = { 0 };
			strcpy(uTmp, "\\registry\\config");
			if (strlen(uPath) - 4 + strlen(uTmp) >= MAX_PATH)//不够长
				return FALSE;
			strcat_s(uTmp, MAX_PATH, &uPath[4]);
			strcpy(uPath, uTmp);
			return TRUE;
		}
	}
	else if (strlen(uPath) >= 3)
	{
		if (('H' == uPath[0] || 'h' == uPath[0])
			&& ('K' == uPath[1] || 'k' == uPath[1])
			&& ('U' == uPath[2] || 'u' == uPath[2])
			)
		{
			char uTmp[MAX_PATH] = { 0 };
			strcpy(uTmp, "\\registry\\users");
			if (strlen(uPath) - 3 + strlen(uTmp) >= MAX_PATH)//不够长
				return FALSE;
			strcat_s(uTmp, MAX_PATH, &uPath[3]);
			strcpy(uPath, uTmp);
			return TRUE;
		}
	}
	return FALSE;
}

/* 分割路径 */
BOOLEAN separatePath(char uPath[MAX_PATH], char uSubKey[MAX_PATH])
{
	int index = (int)strlen(uPath) - 1, cnt = 0;// cnt 记录反斜杠数量
	for (; index > -1 && uPath[index] != '\\'; --index);//定位最后一个字符'\\'的位置
	for (int i = index - 1; i > -1; --i)//计算剩余反斜杠数量（至少为 2）
		if ('\\' == uPath[i] && ++cnt >= 2)//已超过 2
			break;
	if (cnt < 2)
		return FALSE;
	strcpy_s(uSubKey, MAX_PATH, &uPath[index + 1]);//复制出子项
	for (int i = (int)strlen(uPath) - 1; i >= index; --i)//从后往前清除（清除反斜杠）
		uPath[i] = 0;
	return TRUE;
}


/** 通信函数 **/
NTSTATUS cwkDispatch(IN PDEVICE_OBJECT dev, IN PIRP irp)//分发函数
{
	/* 获取当前 irp 栈空间 */
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(irp);
	NTSTATUS status = STATUS_SUCCESS;
	ULONG ret_len = 0;//请求长度

	/* 循环处理发送给 g_cdo 的请求，如果不是发送给 g_cdo，则明显错误 */
	while (dev == g_cdo)
	{
		if (irpsp->MajorFunction == IRP_MJ_CREATE || irpsp->MajorFunction == IRP_MJ_CLOSE)// 生成和关闭请求，一律跳过即可
			break;
		if (irpsp->MajorFunction == IRP_MJ_DEVICE_CONTROL)
		{
			/* 处理 DeviceIoControl */
			PVOID buffer = irp->AssociatedIrp.SystemBuffer;//缓冲区地址
			ULONG inlen = irpsp->Parameters.DeviceIoControl.InputBufferLength;//输入缓冲区长度
			ULONG outlen = irpsp->Parameters.DeviceIoControl.OutputBufferLength;//输出缓冲区长度
			ANSI_STRING NTPath, NTSubKey, NTValue;
			UNICODE_STRING uPath, uSubKey, uValue;
			ULONG ul = 0;
			LONG64 ull = 0;
			PWSTR endptr;
			RtlInitUnicodeString(&uPath, L"");
			RtlInitUnicodeString(&uSubKey, L"");
			RtlInitUnicodeString(&uValue, L"");

			/* 根据前面定义好的设备请求功能号，详细判断输入还是输出，我们在这里只判断输入 */
			switch (irpsp->Parameters.DeviceIoControl.IoControlCode)
			{
			case CWK_DVC_SEND_STR:
				ASSERT(buffer != NULL && inlen < MAX_PATH << 2 && inlen > 0 && 0 == outlen);
				if (NULL == buffer || inlen >= MAX_PATH << 2 || inlen <= 0 || 0 != outlen)
				{
					DbgPrint("%s->ASSERT(buffer != NULL && inlen < MAX_PATH << 2 && inlen > 0 && 0 == outlen)", _ZwOpReg_H);
					break;
				}
				char bufIN[MAX_PATH << 2] = { 0 };
				int cur_index = 0, newline_index[INFO_MEMBER_CNT] = { 0 };
				strcpy_s(bufIN, inlen, (char*)buffer);
				bufIN[0] = ((char*)buffer)[0];
				if (!isValidStructer(bufIN, newline_index, inlen))//检查是否合法并获得分割位置
				{
					DbgPrint("%s->UnknownMsg->\"%s\"\n", _ZwOpReg_H, bufIN);//无效指令充当接收信息
					break;
				}

				/* 开始切割收到的数据 */
				ZwOpRegStruct regInfo = { '0', { 0 }, '0', { 0 }, '0', { 0 } };
				regInfo.uOpType = bufIN[0];
				cur_index = 0;
				for (int i = newline_index[0] + 1; i < newline_index[1]; ++i)
					regInfo.uPath[cur_index++] = bufIN[i];
				regInfo.uSubKeyType = bufIN[newline_index[1] + 1];
				cur_index = 0;
				for (int i = newline_index[2] + 1; i < newline_index[3]; ++i)
					regInfo.uSubKey[cur_index++] = bufIN[i];
				regInfo.uValueType = bufIN[newline_index[3] + 1];
				cur_index = 0;
				for (int i = newline_index[4] + 1; i < newline_index[5]; ++i)
					regInfo.uValue[cur_index++] = bufIN[i];

				/* R3 转 R0 的字符串处理 */
				if (!transReg(regInfo.uPath))
				{
					DbgPrint("%s->InvalidPath->\"%s\"\n", _ZwOpReg_H, regInfo.uPath);
					break;
				}

				/* 打印相关字段信息 */
				DbgPrint("%s->GetWmirMsg()->uOpType = \'%c\'\n", _ZwOpReg_H, regInfo.uOpType);
				DbgPrint("%s->GetWmirMsg()->uPath = \"%s\"\n", _ZwOpReg_H, regInfo.uPath);
				DbgPrint("%s->GetWmirMsg()->uSubKeyType = \'%c\'\n", _ZwOpReg_H, regInfo.uSubKeyType);
				DbgPrint("%s->GetWmirMsg()->uSubKey = \"%s\"\n", _ZwOpReg_H, regInfo.uSubKey);
				DbgPrint("%s->GetWmirMsg()->uValueType = \'%c\'\n", _ZwOpReg_H, regInfo.uValueType);
				DbgPrint("%s->GetWmirMsg()->uValue = \"%s\"\n", _ZwOpReg_H, regInfo.uValue);

				/* 执行相关操作 */
				RtlInitAnsiString(&NTPath, regInfo.uPath);
				RtlInitAnsiString(&NTSubKey, regInfo.uSubKey);
				RtlInitAnsiString(&NTValue, regInfo.uValue);
				RtlAnsiStringToUnicodeString(&uPath, &NTPath, TRUE);
				RtlAnsiStringToUnicodeString(&uSubKey, &NTSubKey, TRUE);
				RtlAnsiStringToUnicodeString(&uValue, &NTValue, TRUE);

				/* 进行相应地分发 */
				switch (bufIN[0])
				{
				case 'A':
				case 'a'://执行 add
					switch (regInfo.uSubKeyType)
					{
					case 'F':
					case 'f':
						if (!separatePath(regInfo.uPath, regInfo.uSubKey))
						{
							DbgPrint("%s->Invalid RegistryPath->\"%s\"\n", _ZwOpReg_H, regInfo.uPath);//异常分割路径
							break;
						}
						RtlInitAnsiString(&NTPath, regInfo.uPath);
						RtlInitAnsiString(&NTSubKey, regInfo.uSubKey);
						RtlAnsiStringToUnicodeString(&uPath, &NTPath, TRUE);
						RtlAnsiStringToUnicodeString(&uSubKey, &NTSubKey, TRUE);
					case 'I':
					case 'i':
						ntIBinaryCreateKey(uPath, uSubKey);
						break;
					case 'E':
					case 'e':
						RtlInitUnicodeString(&uSubKey, L"");//默认键处理流程与常规键一致
					case 'K':
					case 'k':
						switch (regInfo.uValueType)
						{
						case 'S':
						case 's':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_SZ, uValue.Buffer, uValue.Length);//一定要用 wchar_t
							break;
						case 'B':
						case 'b':
							if (!NT_SUCCESS(RtlUnicodeStringToInteger(&uValue, 0, &ul)))
							{
								DbgPrint("%s->Invalid Binary->\"%s\"", _ZwOpReg_H, regInfo.uValue);
								break;
							}
							ntIBinarySetKeyValue(uPath, uSubKey, REG_BINARY, regInfo.uValue, sizeof(ULONG));
							break;
						case 'D':
						case 'd':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_DWORD, regInfo.uValue, (ULONG)strlen(regInfo.uValue));
							break;
						case 'Q':
						case 'q':
							if (!NT_SUCCESS(RtlUnicodeStringToInt64(&uValue, 0, &ull, &endptr)))
							{
								DbgPrint("%s->Invalid UnsignedLongLong->\"%s\"", _ZwOpReg_H, regInfo.uValue);
								break;
							}
							ntIBinarySetKeyValue(uPath, uSubKey, REG_QWORD,&ull, sizeof(ULONG64));
							break;
						case 'M':
						case 'm':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_MULTI_SZ, uValue.Buffer, uValue.Length);//一定要用 wchar_t
							break;
						case 'E':
						case 'e':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_EXPAND_SZ, uValue.Buffer, uValue.Length);//一定要用 wchar_t
							break;
						case 'N':
						case 'n':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_NONE, regInfo.uValue, (ULONG)strlen(regInfo.uValue));//一定要用 char
							break;
						default:
							DbgPrint("%s->Invalid ValueType->\'%c\'\n", _ZwOpReg_H, regInfo.uValueType);//无效值类型
							break;
						}
						break;
					default:
						DbgPrint("%s->Invalid SubKeyType->\'%c\'\n", _ZwOpReg_H, regInfo.uSubKeyType);//无效键类型
						break;
					}
					break;
				case 'D':
				case 'd'://执行 del
					switch (regInfo.uSubKeyType)
					{
					case 'E':
					case 'e':
						RtlInitUnicodeString(&uSubKey, L"");
					case 'K':
					case 'k':
						ntIBinaryDeleteKey(uPath, uSubKey, FALSE);
						break;
					case 'F':
					case 'f':
						if (!separatePath(regInfo.uPath, regInfo.uSubKey))
						{
							DbgPrint("%s->Invalid RegistryPath->\"%s\"\n", _ZwOpReg_H, regInfo.uPath);//异常分割路径
							break;
						}
						RtlInitAnsiString(&NTPath, regInfo.uPath);
						RtlInitAnsiString(&NTSubKey, regInfo.uSubKey);
						RtlAnsiStringToUnicodeString(&uPath, &NTPath, TRUE);
						RtlAnsiStringToUnicodeString(&uSubKey, &NTSubKey, TRUE);
					case 'I':
					case 'i':
						ntIBinaryDeleteKey(uPath, uSubKey, TRUE);
						break;
					default:
						DbgPrint("%s->Invalid SubKeyType->\'%c\'\n", _ZwOpReg_H, regInfo.uSubKeyType);//无效键类型
						break;
					}
					break;
				case 'S':
				case 's'://执行 set
					switch (regInfo.uSubKeyType)
					{
					case 'E':
					case 'e':
						RtlInitUnicodeString(&uSubKey, L"");//默认键处理流程与常规键一致
					case 'K':
					case 'k':
						switch (regInfo.uValueType)
						{
						case 'S':
						case 's':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_SZ, uValue.Buffer, uValue.Length);//一定要用 wchar_t
							break;
						case 'B':
						case 'b':
							if (!NT_SUCCESS(RtlUnicodeStringToInteger(&uValue, 0, &ul)))
							{
								DbgPrint("%s->Invalid Binary->\"%s\"", _ZwOpReg_H, regInfo.uValue);
								break;
							}
							ntIBinarySetKeyValue(uPath, uSubKey, REG_BINARY, regInfo.uValue, sizeof(ULONG));
							break;
						case 'D':
						case 'd':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_DWORD, regInfo.uValue, (ULONG)strlen(regInfo.uValue));
							break;
						case 'Q':
						case 'q':
							if (!NT_SUCCESS(RtlUnicodeStringToInt64(&uValue, 0, &ull, &endptr)))
							{
								DbgPrint("%s->Invalid UnsignedLongLong->\"%s\"", _ZwOpReg_H, regInfo.uValue);
								break;
							}
							ntIBinarySetKeyValue(uPath, uSubKey, REG_QWORD, &ull, sizeof(ULONG64));
							break;
						case 'M':
						case 'm':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_MULTI_SZ, uValue.Buffer, uValue.Length);//一定要用 wchar_t
							break;
						case 'E':
						case 'e':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_EXPAND_SZ, uValue.Buffer, uValue.Length);//一定要用 wchar_t
							break;
						case 'N':
						case 'n':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_NONE, regInfo.uValue, (ULONG)strlen(regInfo.uValue));//一定要用 char
							break;
						default:
							DbgPrint("%s->Invalid ValueType->\'%c\'\n", _ZwOpReg_H, regInfo.uValueType);//无效值类型
							break;
						}
						break;
					default:
						DbgPrint("%s->Invalid SubKeyType->\'%c\'\n", _ZwOpReg_H, regInfo.uSubKeyType);//无效键类型
						break;
					}
					break;
				case 'Q':
				case 'q'://执行 query
					switch (regInfo.uSubKeyType)
					{
					case 'I':
					case 'i':
						while ('\\' == regInfo.uSubKey[strlen(regInfo.uSubKey) - 1])//清除结尾的反斜杠
							regInfo.uSubKey[strlen(regInfo.uSubKey) - 1] = 0;
						while ('\\' == regInfo.uSubKey[0])//清除开头的反斜杠
						{
							char uTmp[MAX_PATH] = { 0 };
							strcpy_s(uTmp, MAX_PATH, regInfo.uSubKey);
							strcpy_s(regInfo.uSubKey, MAX_PATH, &uTmp[1]);
						}
						strcat_s(regInfo.uPath, MAX_PATH, "\\");
						strcat_s(regInfo.uPath, MAX_PATH, regInfo.uSubKey);
						RtlInitAnsiString(&NTPath, regInfo.uPath);
						RtlAnsiStringToUnicodeString(&uPath, &NTPath, TRUE);
					case 'F':
					case 'f':
						ntIBinaryQueryKey(uPath);
						break;
					default:
						DbgPrint("%s->Invalid SubKeyType->\'%c\'\n", _ZwOpReg_H, regInfo.uSubKeyType);//无效键类型
						break;
					}
					break;
				default:
					DbgPrint("%s->Invalid Operation->\"%c\"\n", _ZwOpReg_H, regInfo.uOpType);//无效操作指令
					break;
				}
				break;
			case CWK_DVC_RECV_STR:
			default://到这里的请求都是不接收的（我们没设置接收）
				status = STATUS_INVALID_PARAMETER;//未知的请求一律返回非法参数错误
				break;
			}
		}
		break;
	}

	/* 不是发送给 g_cdo，明显错误 */
	irp->IoStatus.Information = ret_len;//填写返回信息
	irp->IoStatus.Status = status;//填写返回结果
	IoCompleteRequest(irp, IO_NO_INCREMENT);//完成请求
	return status;
}

VOID listenerUnload()//清除通讯符号
{
	UNICODE_STRING cdo_syb = RTL_CONSTANT_STRING(CWK_CDO_SYB_NAME);//符号链接字符串
	ASSERT(g_cdo != NULL);
	IoDeleteSymbolicLink(&cdo_syb);//删除符号链接
	IoDeleteDevice(g_cdo);//删除控制设备
	return;
}

NTSTATUS listenerEntry(PDRIVER_OBJECT pDriver)
{
	/* 生成一个控制设备然后生成符号链接 */
	//UNICODE_STRING sddl = RTL_CONSTANT_STRING(L"D:P(A;;GA;;;WD)");//通用（直接复制即可）
	UNICODE_STRING cdo_name = RTL_CONSTANT_STRING(DeviceName);//设备名
	UNICODE_STRING cdo_syb = RTL_CONSTANT_STRING(CWK_CDO_SYB_NAME);//符号链接

	/* 生成一个设备对象 */
	NTSTATUS status = IoCreateDevice(
		pDriver,
		0, &cdo_name,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&g_cdo
	);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("%s->IoCreateDevice()->Failed(%d)! \n", _ZwOpReg_H, status);
		return status;
	}

	/* 生成链接符号并与设备绑定 */
	IoDeleteSymbolicLink(&cdo_syb);//为防止冲突先预先删除
	status = IoCreateSymbolicLink(&cdo_syb, &cdo_name);//将链接符号与设备绑定
	if (!NT_SUCCESS(status))
	{
		DbgPrint("%s->IoCreateSymbolicLink()->Failed(%d)! \n", _ZwOpReg_H, status);
		IoDeleteDevice(g_cdo);//删除设备
		return status;
	}

	/* 将所有的分发函数设置为自定义的 */
	for (ULONG i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
		pDriver->MajorFunction[i] = cwkDispatch;
	DbgPrint("%s->listenerEntry()->Successful! \n", _ZwOpReg_H);

	/* 清除控制设备的初始化标记 */
	g_cdo->Flags &= ~DO_DEVICE_INITIALIZING;
	return STATUS_SUCCESS;
}