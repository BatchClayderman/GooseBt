#include <ntifs.h>
#include <ntddk.h>
#ifndef _NTNoFileCreate_H
#define _NTNoFileCreate_H "NTNoFileCreate"
#endif


NTSTATUS DispatchCommon(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	DbgPrint("%s->%lu\n", _NTNoFileCreate_H, pIrp->Flags);
	pIrp->IoStatus.Status = STATUS_SUCCESS;			// IRP ��¼��β�������.
	pIrp->IoStatus.Information = 0;					// Information ������¼ʵ�ʴ�����ֽ�����
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);		//�ύ����
	return STATUS_SUCCESS;							//����� STATUS_SUCCESS �Ǹ� R3 ���ģ����ڵķ����Ǹ� IO ������ϵͳ��
}

NTSTATUS DispatchCreate(__in PDEVICE_OBJECT DeviceObject, __in PIRP Irp)
{
	PFILE_OBJECT FileObject = IoGetCurrentIrpStackLocation(Irp)->FileObject;
	PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);

	ULONG Option = Stack->Parameters.Create.Options;
	DbgPrint("%s->%u\n", _NTNoFileCreate_H, Option);
	if ((Option >> 24) == FILE_CREATE)
	{
		DbgPrint("%s->%wZ->%x\n", _NTNoFileCreate_H, &FileObject->FileName, Option);
		Irp->IoStatus.Status = STATUS_ACCESS_DENIED;
		IofCompleteRequest(Irp, IO_NO_INCREMENT);
		return STATUS_ACCESS_DENIED;
	}
	
	return STATUS_SUCCESS;
}



/** ��������ں��� **/
/* ����ֹͣ */
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	DbgPrint("\n%s->DriverUnload()\n", _NTNoFileCreate_H);
	return;
}

/* �������� */
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgPrint("\n%s->DriverEntry()\n", _NTNoFileCreate_H);
	pDriver->Flags |= 0x20;
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
		pDriver->MajorFunction[i] = DispatchCommon;
	pDriver->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}