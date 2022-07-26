#include <ntifs.h>
#include <ntddk.h>
#ifndef _KMDFFile_H
#define _KMDFFile_H "KMDFFile"
#endif


/** ��������ں��� **/
/* ����ֹͣ */
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	DbgPrint("\n");
	DbgPrint(_KMDFFile_H);
	DbgPrint("->DriverUnload()\n");
	return;
}

/* �������� */
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgPrint("\n");
	DbgPrint(_KMDFFile_H);
	DbgPrint("->DriverEntry()\n");
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}