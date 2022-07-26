#include "ntddk.h"
#include "ntddkbd.h"
#include "stdio.h"        

#define KEY_UP        1
#define KEY_DOWN      0  

#define LCONTROL      ((USHORT)0x1D)
#define CAPS_LOCK      ((USHORT)0x3A)  

PDEVICE_OBJECT HookDeviceObject;
PDEVICE_OBJECT kbdDevice;

NTSTATUS KSnifferDispatchRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS KSnifferReadComplete(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context);
NTSTATUS KSnifferDispatchGeneral(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

VOID DriverUnload(PDRIVER_OBJECT driver)
{
	UNREFERENCED_PARAMETER(driver);//�ֶ���Ŀ
	// ����ʵ��������ʲô��������ֻ��ӡһ�仰:
	DbgPrint("\ngoodbye!\n");
	return;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);//�ֶ���Ŀ
	CCHAR         ntNameBuffer[64];
	STRING         ntNameString;
	UNICODE_STRING      ntUnicodeString = { 256,256 };
	NTSTATUS            status;

	DbgPrint("DriverEntry \n");
	DriverObject->MajorFunction[IRP_MJ_READ] = KSnifferDispatchRead;
	DriverObject->MajorFunction[IRP_MJ_CREATE] =
		DriverObject->MajorFunction[IRP_MJ_CLOSE] =
		DriverObject->MajorFunction[IRP_MJ_FLUSH_BUFFERS] =
		DriverObject->MajorFunction[IRP_MJ_CLEANUP] =
		DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = KSnifferDispatchGeneral;

	sprintf(ntNameBuffer, "\\Device\\KeyboardClass0");
	RtlInitAnsiString(&ntNameString, ntNameBuffer);
	RtlAnsiStringToUnicodeString(&ntUnicodeString, &ntNameString, TRUE);

	status = IoCreateDevice(DriverObject,
		0,
		NULL,
		FILE_DEVICE_KEYBOARD,
		0,
		FALSE,
		&HookDeviceObject);            //����һ�������豸

	if (!NT_SUCCESS(status))
	{
		DbgPrint("Init Error\n");
		RtlFreeUnicodeString(&ntUnicodeString);
		return STATUS_SUCCESS;
	}
	HookDeviceObject->Flags |= DO_BUFFERED_IO;
	status = IoAttachDevice(HookDeviceObject, &ntUnicodeString, &kbdDevice);      //�������ǵĹ����豸��\\Device\\KeyboardClass0�豸��
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Connect with keyboard failed!\n");
		IoDeleteDevice(HookDeviceObject);
		RtlFreeUnicodeString(&ntUnicodeString);
		return STATUS_SUCCESS;
	}
	RtlFreeUnicodeString(&ntUnicodeString);
	DbgPrint("Successfully connected to keyboard device\n");
	DbgPrint("*** Please note that you can not stop this driver until you restart!\n");
	//DriverObject->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}


NTSTATUS KSnifferDispatchRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)        //�а�������ʱִ��
{
	PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);        //��ȡ��ǰ��IRP��
	PIO_STACK_LOCATION nextIrpStack = IoGetNextIrpStackLocation(Irp);
	*nextIrpStack = *currentIrpStack;
	IoSetCompletionRoutine(Irp, KSnifferReadComplete, DeviceObject, TRUE, TRUE, TRUE);     //�����������
	return IoCallDriver(kbdDevice, Irp);
}

NTSTATUS KSnifferReadComplete(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context)
{
	UNREFERENCED_PARAMETER(DeviceObject);//�ֶ���Ŀ
	UNREFERENCED_PARAMETER(Context);//�ֶ���Ŀ
	PIO_STACK_LOCATION        IrpSp;
	PKEYBOARD_INPUT_DATA      KeyData;

	IrpSp = IoGetCurrentIrpStackLocation(Irp);
	if (NT_SUCCESS(Irp->IoStatus.Status))
	{
		KeyData = Irp->AssociatedIrp.SystemBuffer;
		DbgPrint("ScanCode: %x ", KeyData->MakeCode);
		DbgPrint("%s\n", KeyData->Flags ? "Up" : "Down");      //���������ɨ����

		if (KeyData->MakeCode == CAPS_LOCK)
		{
			KeyData->MakeCode = LCONTROL;                  //�޸İ���
		}
	}
	if (Irp->PendingReturned)
	{
		IoMarkIrpPending(Irp);
	}

	return Irp->IoStatus.Status;
}

NTSTATUS KSnifferDispatchGeneral(                //ͨ���¼���������
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP          Irp)
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
	{
		return STATUS_SUCCESS;
	}
}