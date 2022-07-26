#include <ntifs.h>
#include <ntddk.h>
#ifndef _KMDFTDI_H
#define _KMDFTDI_H "KMDFTDI"
#endif


/** ��������ں��� **/
/* ����ֹͣ */
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	DbgPrint("\n");
	DbgPrint(_KMDFTDI_H);
	DbgPrint("->DriverUnload()\n");
	return;
}

/* �������� */
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgPrint("\n");
	DbgPrint(_KMDFTDI_H);
	DbgPrint("->DriverEntry()\n");
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}