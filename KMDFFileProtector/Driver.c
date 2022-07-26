#include <ntifs.h>
#include <ntddk.h>
#include "Listener.h"
#ifndef _KMDFFileProtector_H
#define _KMDFFileProtector_H "KMDFFileProtector"
#ifndef hN
#define hN 100
#endif
#endif//_KMDFFileProtector_H
HANDLE hFile[hN];
int hNum = 0;


/** 文件防删函数 **/
/* 直接占用文件防删 */
BOOLEAN NtOccurFile(UNICODE_STRING uPathName)
{
	if (hNum >= hN)
		return FALSE;
	OBJECT_ATTRIBUTES objAttri = { 0 };
	NTSTATUS ntStaus;
	IO_STATUS_BLOCK ioStatus = { 0 };
	InitializeObjectAttributes(&objAttri,//打开文件
		&uPathName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		0);
	ntStaus = ZwOpenFile(&hFile[hNum],
		FILE_READ_ACCESS,
		&objAttri,
		&ioStatus,
		FILE_ATTRIBUTE_NORMAL,
		0);
	DbgPrint(_KMDFFileProtector_H);
	DbgPrint("->NtOccurFile(%ws)->", (wchar_t*)uPathName.Buffer);
	if (NT_SUCCESS(ntStaus))
	{
		DbgPrint("Successful!\n");
		++hNum;
		return TRUE;
	}
	else
	{
		DbgPrint("Failed!\n");
		return FALSE;
	}
}


/** 驱动出入口函数 **/
/* 驱动停止 */
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	for (; hNum >= 0; --hNum)
		ZwClose(hFile[hNum]);
	listenerUnload();
	DbgPrint("\n");
	DbgPrint(_KMDFFileProtector_H);
	DbgPrint("->DriverUnload()\n");
	return;
}

/* 驱动启动 */
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgPrint("\n");
	DbgPrint(_KMDFFileProtector_H);
	DbgPrint("->DriverEntry()\n");
	NTSTATUS bRet = listenerEntry(pDriver);
	if (!NT_SUCCESS(bRet))
		return bRet;
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}