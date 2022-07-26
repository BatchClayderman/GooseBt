#include <ntifs.h>
#include <ntddk.h>
#ifndef _NTNoFileCreate_H
#define _NTNoFileCreate_H "NTNoFileCreate"
#endif


NTSTATUS DispatchCommon(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	DbgPrint("%s->%lu\n", _NTNoFileCreate_H, pIrp->Flags);
	pIrp->IoStatus.Status = STATUS_SUCCESS;			// IRP 记录这次操作与否的.
	pIrp->IoStatus.Information = 0;					// Information 用来记录实际传输的字节数的
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);		//提交请求
	return STATUS_SUCCESS;							//上面的 STATUS_SUCCESS 是给 R3 看的，现在的返回是给 IO 管理器系统的
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



/** 驱动出入口函数 **/
/* 驱动停止 */
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	DbgPrint("\n%s->DriverUnload()\n", _NTNoFileCreate_H);
	return;
}

/* 驱动启动 */
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