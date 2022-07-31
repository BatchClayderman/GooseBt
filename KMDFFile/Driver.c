#include <ntifs.h>
#include <ntddk.h>
#ifndef _KMDFFile_H
#define _KMDFFile_H "KMDFFile"
#endif



/** 驱动出入口函数 **/
/* 驱动停止 */
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	DbgPrint("\n%s->DriverUnload()\n", _KMDFFile_H);
	return;
}

/* 驱动启动 */
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgPrint("\n%s->DriverEntry()\n", _KMDFFile_H);
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}