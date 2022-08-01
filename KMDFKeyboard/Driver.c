#include <ntddk.h>
#include <ntddkbd.h>
#ifndef _KMDFKeyboard_H
#define _KMDFKeyboard_H "KMDFKeyboard"
#define KEY_UP        1
#define KEY_DOWN      0  
#define LCONTROL      ((USHORT)0x1D)
#define CAPS_LOCK      ((USHORT)0x3A)  
#endif//_KMDFKeyboard_H
PDEVICE_OBJECT HookDeviceObject;
PDEVICE_OBJECT kbdDevice;


typedef struct
{
	ULONG NodeSize;
	PDEVICE_OBJECT filterdevice;
	PDEVICE_OBJECT LowerDeviceObject;
	PDEVICE_OBJECT targetdevice;
	KSPIN_LOCK IoRequestsSpinLock;
	KEVENT IoInProcessEvent;
} C2P_DEV_EXT, * PC2P_DEV_EXT;


/** �������� **/
/* ������� */
NTSTATUS KSnifferReadComplete(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context)
{
	UNREFERENCED_PARAMETER(DeviceObject);//�ֶ���Ŀ
	UNREFERENCED_PARAMETER(Context);//�ֶ���Ŀ

	if (NT_SUCCESS(Irp->IoStatus.Status))
	{
		PKEYBOARD_INPUT_DATA KeyData = Irp->AssociatedIrp.SystemBuffer;
		DbgPrint("%s->ScanCode: %x->%s", _KMDFKeyboard_H, KeyData->MakeCode, KeyData->Flags ? "Up" : "Down");//���������ɨ����

		if (KeyData->MakeCode == CAPS_LOCK)//�޸İ���
			KeyData->MakeCode = LCONTROL;
	}
	if (Irp->PendingReturned)
		IoMarkIrpPending(Irp);

	return Irp->IoStatus.Status;
}

/* ���̰��»��ɿ� */
NTSTATUS KSnifferDispatchRead(IN PDEVICE_OBJECT pDeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);//��ȡ��ǰ��IRP��
	PIO_STACK_LOCATION nextIrpStack = IoGetNextIrpStackLocation(Irp);
	*nextIrpStack = *currentIrpStack;
	IoSetCompletionRoutine(Irp, KSnifferReadComplete, pDeviceObject, TRUE, TRUE, TRUE);//�����������
	return IoCallDriver(kbdDevice, Irp);
}

/* �����Դ���� */
NTSTATUS KSnifferDispatchPower(IN PDEVICE_OBJECT pDeviceObject, IN PIRP Irp)
{
	PC2P_DEV_EXT devExt = (PC2P_DEV_EXT)pDeviceObject->DeviceExtension;
	PoStartNextPowerIrp(Irp);
	IoSkipCurrentIrpStackLocation(Irp);
	return PoCallDriver(devExt->LowerDeviceObject, Irp);
}

/* �����弴�� */
NTSTATUS KSnifferDispatchPnP(IN PDEVICE_OBJECT pDeviceObject, IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PC2P_DEV_EXT devExt = (PC2P_DEV_EXT)pDeviceObject->DeviceExtension;
	PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);

	switch (irpStack->MinorFunction)
	{
	case IRP_MN_REMOVE_DEVICE:
		DbgPrint("%s->IRP_MN_REMOVE_DEVICE\n", _KMDFKeyboard_H);
		IoSkipCurrentIrpStackLocation(Irp);
		IoCallDriver(devExt->LowerDeviceObject, Irp);
		IoDetachDevice(devExt->LowerDeviceObject);
		status = STATUS_SUCCESS;
		break;
	default:
		IoSkipCurrentIrpStackLocation(Irp);
		status = IoCallDriver(devExt->LowerDeviceObject, Irp);
	}
	return status;
}

/* ͨ���¼��������� */
NTSTATUS KSnifferDispatchGeneral(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
	PIO_STACK_LOCATION nextIrpStack = IoGetNextIrpStackLocation(Irp);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	if (DeviceObject == HookDeviceObject)
	{
		*nextIrpStack = *currentIrpStack;
		return IoCallDriver(kbdDevice, Irp);
	}
	else
		return STATUS_SUCCESS;
}



/** ��������ں��� **/
/* ����ֹͣ */
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);//�ֶ���Ŀ
	DbgPrint("\n%s->DriverUnload()\n", _KMDFKeyboard_H);
	IoDeleteDevice(HookDeviceObject);
	IoDeleteDevice(pDriver->DeviceObject);
	return;
}

/* �������� */
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);//�ֶ���Ŀ
	DbgPrint("\n%s->DriverEntry()\n", _KMDFKeyboard_H); 
	
	STRING ntNameString;
	UNICODE_STRING ntUnicodeString = { 256, 256 };
	RtlInitAnsiString(&ntNameString, "\\Device\\KeyboardClass0");
	RtlAnsiStringToUnicodeString(&ntUnicodeString, &ntNameString, TRUE);

	NTSTATUS status = IoCreateDevice(DriverObject,
		0,
		NULL,
		FILE_DEVICE_KEYBOARD,
		0,
		FALSE,
		&HookDeviceObject
	);//�����������豸

	if (!NT_SUCCESS(status))
	{
		DbgPrint("%s->IoCreateDevice()\n", _KMDFKeyboard_H);
		RtlFreeUnicodeString(&ntUnicodeString);
		return status;
	}

	HookDeviceObject->Flags |= DO_BUFFERED_IO;
	status = IoAttachDevice(HookDeviceObject, &ntUnicodeString, &kbdDevice);//�������ǵĹ����豸�� \\Device\\KeyboardClass0 �豸��
	if (!NT_SUCCESS(status))
	{
		DbgPrint("%s->Connect to keyboard->Failed! \n", _KMDFKeyboard_H);
		IoDeleteDevice(HookDeviceObject);
		RtlFreeUnicodeString(&ntUnicodeString);
		return status;
	}
	RtlFreeUnicodeString(&ntUnicodeString);

	for (ULONG i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
		DriverObject->MajorFunction[i] = KSnifferDispatchGeneral;
	DriverObject->MajorFunction[IRP_MJ_READ] = KSnifferDispatchRead;
	DriverObject->MajorFunction[IRP_MJ_POWER] = KSnifferDispatchPower;
	DriverObject->MajorFunction[IRP_MJ_PNP] = KSnifferDispatchPnP;

	DbgPrint("%s->Connect to keyboard->Successful! \n", _KMDFKeyboard_H);
	DbgPrint("%s->*** Please note that you can not stop this driver until you restart! ***\n", _KMDFKeyboard_H);
	DriverObject->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}