#include <ntifs.h>
#include <ntddk.h>
#ifndef _NTNoFileCreate_H
#define _NTNoFileCreate_H "NTNoFileCreate"
#define SYMBOLIC_LINK_NAME L"\\DosDevices\\NTNoFileCreate"
#define NT_DEVICE_NAME L"\\FileSystem\\NTNoFileCreate"
#define NT_FILTER_DEVICE_NAME L"\\FileSystem\\Filters\\NTNoFileCreate"
#define IS_MY_DEVICE_OBJECT(_devObj)								\
	(((_devObj) != NULL)											\
	&& ((_devObj)->DriverObject == gSFilterControlDeviceObject)		\
	&& ((_devObj)->DeviceExtension != NULL)							\
	&& ((*(ULONG *)(_devObj)->DeviceExtension) == SFLT_POOL_TAG))
#define IS_MY_CONTROL_DEVICE_OBJECT(_devObj)						\
	(((_devObj) == gSFilterControlDeviceObject) ?					\
		(ASSERT(((_devObj)->DriverObject == gSFilterDriverObject)	\
			&& ((_devObj)->DeviceExtension == NULL)), TRUE) :		\
		FALSE														\
	)
#endif
PDEVICE_OBJECT gSFilterControlDeviceObject;


NTSTATUS DispatchCommon(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	DbgPrint("%s->%lu\n", _NTNoFileCreate_H, pIrp->Flags);
	pIrp->IoStatus.Status = STATUS_SUCCESS;			// IRP ��¼��β�������.
	pIrp->IoStatus.Information = 0;					// Information ������¼ʵ�ʴ�����ֽ�����
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);		//�ύ����
	return STATUS_SUCCESS;							//����� STATUS_SUCCESS �Ǹ� R3 ���ģ����ڵķ����Ǹ� IO ������ϵͳ��
}

NTSTATUS DispatchCreate(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
	if (IS_MY_CONTROL_DEVICE_OBJECT(pDeviceObject))
	{
		Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
		Irp->IoStatus.Information = 0;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return STATUS_INVALID_DEVICE_REQUEST;
	}

	PFILE_OBJECT FileObject = IoGetCurrentIrpStackLocation(Irp)->FileObject;
	PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);

	ULONG Option = Stack->Parameters.Create.Options;
	if ((Option >> 24) == FILE_CREATE)
	{
		DbgPrint("%s->%wZ->%x\n", _NTNoFileCreate_H, &FileObject->FileName, Option);
		Irp->IoStatus.Status = STATUS_ACCESS_DENIED;
		IofCompleteRequest(Irp, IO_NO_INCREMENT);
		return STATUS_ACCESS_DENIED;
	}
	
	return STATUS_SUCCESS;
}

VOID DriverNotificationRoutine(PDEVICE_OBJECT pDevice, BOOLEAN FsActive)
{
	DbgPrint("%s->%wZ->%s", _NTNoFileCreate_H, &pDevice->DriverObject->DriverName, FsActive ? "TRUE" : "FALSE");
	return;
}



/** ��������ں��� **/
/* ����ֹͣ */
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	DbgPrint("\n%s->DriverUnload()\n", _NTNoFileCreate_H);
	UNICODE_STRING strLink;
	RtlInitUnicodeString(&strLink, SYMBOLIC_LINK_NAME);
	IoDeleteSymbolicLink(&strLink);
	IoUnregisterFsRegistrationChange(pDriver, DriverNotificationRoutine); 
	IoDeleteDevice(pDriver->DeviceObject);
	return;
}

/* �������� */
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgPrint("\n%s->DriverEntry()\n", _NTNoFileCreate_H);
	
	UNICODE_STRING nameString, syblnkString;
	RtlInitUnicodeString(&nameString, NT_FILTER_DEVICE_NAME);

	/* ���ɿ����豸 */
	NTSTATUS status = IoCreateDevice(
		pDriver, 
		0, 
		&nameString, 
		FILE_DEVICE_DISK_FILE_SYSTEM, 
		FILE_DEVICE_SECURE_OPEN, 
		FALSE, 
		&gSFilterControlDeviceObject
	);

	if (STATUS_OBJECT_PATH_NOT_FOUND == status)//δ�ҵ�ע���·��
	{
		RtlInitUnicodeString(&nameString, NT_DEVICE_NAME);
		status = IoCreateDevice(
			pDriver,
			0,
			&nameString,
			FILE_DEVICE_DISK_FILE_SYSTEM,
			FILE_DEVICE_SECURE_OPEN,
			FALSE,
			&gSFilterControlDeviceObject
		);
	}
	
	if (!NT_SUCCESS(status))//����ʧ��
	{
		DbgPrint("%s->IoCreateDevice->Failed(%d)! \n", _NTNoFileCreate_H, status);
		return status;
	}
	
	/* ������������ */
	RtlInitUnicodeString(&syblnkString, SYMBOLIC_LINK_NAME);
	status = IoCreateSymbolicLink(&syblnkString, &nameString);

	if (!NT_SUCCESS(status))//����ʧ����ɾ������һ��
	{
		IoDeleteSymbolicLink(&syblnkString);
		status = IoCreateSymbolicLink(&syblnkString, &nameString);

		if (!NT_SUCCESS(status))
		{
			DbgPrint("%s->IoCreateSymbolicLink->Failed(%d)! \n", _NTNoFileCreate_H, status);
			IoDeleteDevice(gSFilterControlDeviceObject);
			return status;
		}
	}

	/* �ɷ����� */
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
		pDriver->MajorFunction[i] = DispatchCommon;
	pDriver->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
	IoRegisterFsRegistrationChange(pDriver, DriverNotificationRoutine);
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}