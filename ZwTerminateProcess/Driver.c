#include <ntifs.h>
#include <ntstrsafe.h>
#include "Listener.h"
#ifndef _ZwTerminateProcess_H
#define _ZwTerminateProcess_H "ZwTerminateProcess"
#ifndef NULL
#define NULL ((void *)0)
#endif
#ifndef VOID
#define VOID void
#endif
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#ifndef ZeroMax
#define ZeroMax 0x1000000//足够清零的值
#endif
#endif//_ZwTerminateProcess_H
#pragma warning(disable:6387)//屏蔽内存清零的 C6387 警告
char* PsGetProcessImageFileName(PEPROCESS Process);
const GUID  CWK_GUID_CLASS_MYCDO = {
	0x17a0d1e0L,
	0x3249,
	0x12e1,
	{ 0x92, 0x16, 0x45, 0x1a, 0x21, 0x30, 0x29, 0x06 }
};//默认的 GUID，复制粘贴即可
int b_dp[MAX_PATH][MAX_PATH] = { {0} };


/** 字符数组转 ULONG **/
DWORD powerful(DWORD a, DWORD x)
{
	DWORD tmp = 1;
	for (DWORD i = 0; i < x; ++i)
		tmp *= a;
	return tmp;
}

BOOLEAN StringToULONG(char* targetPID, DWORD* pid)// targetPID -> pid
{
	size_t stringLength = strlen(targetPID);//计算一次以减少时间复杂度
#if (defined _WIN64 || defined WIN64)
	if (stringLength >= 10)//最多 10 个字符长（取决于 DWORD）
		return FALSE;
#elif (defined _WIN32 || defined WIN32)// _WIN32 为 32 位内核驱动
	if (stringLength >= 10)//最多 10 个字符长（取决于 DWORD）
		return FALSE;
#elif (defined _WIN16 || defined WIN16)
	if (stringLength >= 6)//最多 6 个字符长（取决于 DWORD）
		return FALSE;
#else
	return FALSE;
#endif
	*pid = 0;//置零
	for (size_t i = 0; i < stringLength; ++i)
		if (targetPID[i] >= '0' && targetPID[i] <= '9')//确保是数字字符
			*pid += (targetPID[i] - '0') * powerful(10, (DWORD)(stringLength - i - 1));//记得要乘对应的数而且 10 的指数还要 -1
		else
		{
			*pid = 0;//置零
			return FALSE;
		}
	return TRUE;
}


/** 进程信息转换 **/
/* PID -> 映像名称 */
BOOLEAN GetProcessName(ULONG dwPid, char* dwImageName, BOOLEAN reduceDbgPrint)
{
	HANDLE ProcessHandle;
	NTSTATUS status;
	OBJECT_ATTRIBUTES ObjectAttributes;
	CLIENT_ID myCid;
	PEPROCESS EProcess;
	InitializeObjectAttributes(&ObjectAttributes, 0, 0, 0, 0);
	myCid.UniqueProcess = (HANDLE)dwPid;
	myCid.UniqueThread = 0;

	/* 从驱动打开进程获取句柄 */
	status = ZwOpenProcess(&ProcessHandle, PROCESS_ALL_ACCESS, &ObjectAttributes, &myCid);
	if (!NT_SUCCESS(status))
	{
		if (!reduceDbgPrint)
			DbgPrint("%s->GetProcessName(%u)->Error opening process! \n", _ZwTerminateProcess_H, dwPid);
		return FALSE;
	}

	/* 得到 EPROCESS，结构中取进程名 */
	status = ObReferenceObjectByHandle(ProcessHandle, FILE_READ_DATA, 0, KernelMode, &EProcess, 0);
	if (status == STATUS_SUCCESS)
	{
		char ProcessName[MAX_PATH] = { 0 };
		strcpy_s(ProcessName, MAX_PATH, (char*)PsGetProcessImageFileName(EProcess));
		if (!reduceDbgPrint)
			DbgPrint("%s->GetProcessName(%u)->PID = %u\t\tImageName = %s\n", _ZwTerminateProcess_H, dwPid, dwPid, PsGetProcessImageFileName(EProcess));
		if (dwImageName)
			strcpy_s(dwImageName, MAX_PATH, ProcessName);
		ZwClose(ProcessHandle);//关闭驱动句柄
		return TRUE;
	}
	else
	{
		if (!reduceDbgPrint)
			DbgPrint("%s->GetProcessName(%u)->Get ProcessName error! \n", _ZwTerminateProcess_H, dwPid);//获取失败
		return FALSE;
	}
}

/* 匹配通配符 */
NTSTATUS match_string(char* mstr, char* wstr)//处理通配符
{
	if (mstr == NULL || wstr == NULL)
		return STATUS_MEMORY_NOT_ALLOCATED;
	size_t m_len = strlen(mstr), w_len = strlen(wstr);//[w_len + 1][m_len + 1]
	if (m_len >= MAX_PATH || w_len >= MAX_PATH)
		return STATUS_MEMORY_NOT_ALLOCATED;
	CHAR m_str[MAX_PATH] = { 0 }, w_str[MAX_PATH] = { 0 };
	for (size_t i = 0; i < strlen(mstr); ++i)//变成小写字母
		if (mstr[i] >= 'A' && mstr[i] <= 'Z')
			m_str[i] = mstr[i] + 32;
		else
			m_str[i] = mstr[i];
	for (size_t i = 0; i < strlen(wstr); ++i)//变成小写字母
		if (wstr[i] >= 'A' && wstr[i] <= 'Z')
			w_str[i] = wstr[i] + 32;
		else
			w_str[i] = wstr[i];
	for (int i = 0; i < MAX_PATH; ++i)
		for (int j = 0; j < MAX_PATH; ++j)
			b_dp[i][j] = 0;
	b_dp[0][0] = 1;
	for (size_t i = 1; i <= w_len; ++i)
	{
		CHAR ch = w_str[i - 1];
		b_dp[i][0] = (int)(b_dp[i - 1][0] && (ch == '*'));//设置每次循环的初值，即当星号不出现在首位时，匹配字符串的初值都为 false
		for (size_t j = 1; j <= m_len; ++j)
		{
			CHAR ch2 = m_str[j - 1];
			if (ch == '*')
				b_dp[i][j] = (int)(b_dp[i - 1][j] || b_dp[i][j - 1]);//当匹配字符为 * 时，状态取决于上面状态和左边状态的值
			else
				b_dp[i][j] = (int)(b_dp[i - 1][j - 1] && (ch == '?' || ch2 == ch));//决定于上一次和本次
		}
	}
	return b_dp[w_len][m_len] == 1 ? TRUE : FALSE;//匹配成功的返回值是 1
}

/* 映像名称 -> PID */
VOID GetProcessID(char* targetPsName, ULONG* dwPid)
{
	char dwProcessName[MAX_PATH] = { 0 };
	unsigned int j = 0;
	for (ULONG i = 0; i <= 240000 && j < MAX_PATH; i += 4)
		if (GetProcessName(i, dwProcessName, TRUE) && match_string(dwProcessName, targetPsName) == TRUE)
			dwPid[j++] = i;
	return;
}


/** 驱动级结束进程函数 **/
/* TerminateProcess->NtTerminateProcess->ZwTerminateProcess */
BOOLEAN ZwKillProcess(DWORD PID)
{
	HANDLE hProcess = NULL;
	CLIENT_ID ClientId = {
		.UniqueProcess = NULL,
		.UniqueThread = NULL,
	};
	OBJECT_ATTRIBUTES oa;

	/* 填充 CID */
	ClientId.UniqueProcess = (HANDLE)PID;
	ClientId.UniqueThread = 0;

	/* 填充 OA */
	oa.Length = sizeof(oa);
	oa.RootDirectory = 0;
	oa.ObjectName = 0;
	oa.Attributes = 0;
	oa.SecurityDescriptor = 0;
	oa.SecurityQualityOfService = 0;

	/* 打开进程，如果句柄有效，则结束进程 */
	ZwOpenProcess(&hProcess, 1, &oa, &ClientId);
	if (hProcess)
	{
		ZwTerminateProcess(hProcess, 0);
		ZwClose(hProcess);
		return TRUE;
	}
	else
		return FALSE;
}

/* 内存清零法 */
BOOLEAN ZeroKill(ULONG PID)
{
	NTSTATUS ntStatus = STATUS_SUCCESS;
	PVOID handle;
	PEPROCESS Eprocess;
	ntStatus = PsLookupProcessByProcessId((HANDLE)PID, &Eprocess);
	if (NT_SUCCESS(ntStatus))
	{
		KeAttachProcess(Eprocess);// Attach 进程虚拟空间
		for (int i = 0; i <= ZeroMax && i <= 0x7fffffff; i += 0x1000)
			if (MmIsAddressValid((PVOID)i))
			{
				_try
				{
					ProbeForWrite((PVOID)i, 0x1000, sizeof(ULONG));
					memset((PVOID)i, 0xcc, 0x1000);
				}
					_except(1)
				{
					continue;
				}
			}
		KeDetachProcess();
		if (ObOpenObjectByPointer((PVOID)Eprocess, 0, NULL, 0, NULL, KernelMode, &handle) != STATUS_SUCCESS)
			return FALSE;
		ZwTerminateProcess((HANDLE)handle, STATUS_SUCCESS);
		ZwClose((HANDLE)handle);
		return TRUE;
	}
	return FALSE;
}

/* 通过 PID 结束 */
BOOLEAN PIDCallTerminate(DWORD targetPID)
{
	DbgPrint("%s->ZwTerminate(%d)\n", _ZwTerminateProcess_H, targetPID);
	if (ZwKillProcess(targetPID))
	{
		DbgPrint("%s->ZwTerminate(%d)->ZwTerminteProcess->TRUE\n", _ZwTerminateProcess_H, targetPID);
		return TRUE;
	}
	else
		DbgPrint("%s->ZwTerminate(%d)->ZwTerminteProcess->FALSE\n", _ZwTerminateProcess_H, targetPID);
	if (ZeroKill(targetPID))
	{
		DbgPrint("%s->ZwTerminate(%d)->ZeroKill->TRUE\n", _ZwTerminateProcess_H, targetPID);
		return TRUE;
	}
	else
		DbgPrint("%s->ZwTerminate(%d)->ZeroKill->FALSE\n", _ZwTerminateProcess_H, targetPID);
	return FALSE;
}

/* 通过映像名称结束 */
VOID ZwKillImage(char* dwProcessName)
{
	ULONG dwPid[MAX_PATH] = { 1 };//第一个为 1 则说明未找到进程
	GetProcessID(dwProcessName, dwPid);
	if (dwPid[0] != 1)
	{
		unsigned int i = 0;
		do
		{
			PIDCallTerminate(dwPid[i++]);
		} while (dwPid[i] != 0);
	}
	return;
}


/** 驱动重启与关机 **/
/* 驱动重启 */
VOID DrivenReboot()
{
	DbgPrint("%s->DrivenReboot()\n", _ZwTerminateProcess_H);
	KeBugCheck(POWER_FAILURE_SIMULATE);
	/*
	typedef void(__fastcall* FCRB)(void);
	__asm
	{
		mov al, 0FEh
		out 64h, al
		ret
	}
	typedef void(__fastcall* FCRB)(void);
	__asm
	{
		mov al, 01
		out 92h, al
		ret
	} // 另一种方案
	*/
	return;
}

#if (!(defined _WIN64 || defined WIN64))
/* 驱动关机 */
VOID DrivenShutdown()
{
	DbgPrint("%s->DrivenShutdown()\n", _ZwTerminateProcess_H);
	typedef void(__fastcall* FCRB)(void);
	__asm
	{
		mov ax, 2001h
		mov dx, 1004h
		out dx, ax
		ret
	}
	return;
}
#endif

/* 驱动蓝屏 */
VOID DrivenError()
{
	RemoveEntryList(NULL);
	return;
}



/** 驱动出入口函数 **/
/* 驱动停止 */
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	listenerUnload();
	DbgPrint("\n%s->DriverUnload()\n", _ZwTerminateProcess_H);
	return;
}

/* 驱动启动 */
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgPrint("\n%s->DriverEntry()\n", _ZwTerminateProcess_H);
	NTSTATUS bRet = listenerEntry(pDriver);
	if (!NT_SUCCESS(bRet))
		return bRet;
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}