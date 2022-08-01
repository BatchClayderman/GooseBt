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
	DbgPrint("\n%s->DriverUnload()\n", _KMDFFile_H);
	return;
}

/* �������� */
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgPrint("\n%s->DriverEntry()\n", _KMDFFile_H);
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}