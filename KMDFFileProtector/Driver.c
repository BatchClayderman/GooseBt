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
	InitializeObjectAttributes(&objAttri, //打开文件
		&uPathName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		0
	);
	ntStaus = ZwOpenFile(&hFile[hNum],
		FILE_READ_ACCESS,
		&objAttri,
		&ioStatus,
		FILE_ATTRIBUTE_NORMAL,
		0
	);
	
	if (NT_SUCCESS(ntStaus))
	{
		DbgPrint("%s->NtOccurFile(\"%ws\")->Successful! \n", _KMDFFileProtector_H, (wchar_t*)uPathName.Buffer);
		++hNum;
		return TRUE;
	}
	else
	{
		DbgPrint("%s->NtOccurFile(\"%ws\")->Failed! \n", _KMDFFileProtector_H, (wchar_t*)uPathName.Buffer);
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
	DbgPrint("\n%s->DriverUnload()\n", _KMDFFileProtector_H);
	return;
}

/* 驱动启动 */
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgPrint("\n%s->DriverEntry()\n", _KMDFFileProtector_H);
	NTSTATUS bRet = listenerEntry(pDriver);
	if (!NT_SUCCESS(bRet))
		return bRet;
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}