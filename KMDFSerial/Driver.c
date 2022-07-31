#include <ntddk.h>
#include <ntstrsafe.h>
#ifndef _KMDFSerial_H
#define _KMDFSerial_H "KMDFSerial"
#define DELAY_SECOND_CNT 5
#define DELAY_ONE_MICROSECOND (-10)
#define DELAY_ONE_MILLISECOND (DELAY_ONE_MICROSECOND  * 1000)
#define DELAY_ONE_SECOND (DELAY_ONE_MILLISECOND * 1000)
//#define NTSTRSAFE_LIB
//#define NTSTRSAFE_LIB "ntstrsafe.lib"
//#pragma comment(lib, "ntstrsafe.lib")
#ifdef NTSTRSAFE_LIB
#undef NTSTRSAFE_LIB
#endif//��ֹ����ʧ��
#ifndef SetFlag
#define SetFlag(_F,_SF) ((_F) |= (_SF))
#endif
#ifndef ClearFlag
#define ClearFlag(_F,_SF) ((_F) &= ~(_SF))
#endif
#ifndef CCP_MAX_COM_ID
#define CCP_MAX_COM_ID 32
#endif
#ifndef MAX_NAME_LEN
#define MAX_NAME_LEN 32
#endif
#endif//_KMDFSerial_H


/* �����豸����ʵ�豸��ȫ�ֱ��� */
static PDEVICE_OBJECT s_fltobj[CCP_MAX_COM_ID] = { 0 };
static PDEVICE_OBJECT s_nextobj[CCP_MAX_COM_ID] = { 0 };


/* ��һ���˿��豸 */
PDEVICE_OBJECT ccpOpenCom(ULONG id, NTSTATUS* status)
{
	UNICODE_STRING name_str;
	static WCHAR name[MAX_NAME_LEN] = { 0 };
	PFILE_OBJECT fileobj = NULL;
	PDEVICE_OBJECT devobj = NULL;

	memset(name, 0, sizeof(WCHAR) * 32);//�����ַ���
	RtlStringCchPrintfW(name, 
		MAX_NAME_LEN, 
		L"\\Device\\Serial%d", 
		id
	);
	RtlInitUnicodeString(&name_str, name);

	/* ���豸���� */
	*status = IoGetDeviceObjectPointer(&name_str, FILE_ALL_ACCESS, &fileobj, &devobj);
	if (*status == STATUS_SUCCESS)
		ObDereferenceObject(fileobj);

	return devobj;
}

NTSTATUS ccpAttachDevice(PDRIVER_OBJECT driver, PDEVICE_OBJECT oldobj, PDEVICE_OBJECT* fltobj, PDEVICE_OBJECT* next)
{
	NTSTATUS status;
	PDEVICE_OBJECT topdev = NULL;

	/* �����豸���� */
	status = IoCreateDevice(driver,
		0,
		NULL,
		oldobj->DeviceType,
		0,
		FALSE,
		fltobj
	);

	if (status != STATUS_SUCCESS)
		return status;

	/* ������Ҫ��־λ */
	if (oldobj->Flags & DO_BUFFERED_IO)
		(*fltobj)->Flags |= DO_BUFFERED_IO;
	if (oldobj->Flags & DO_DIRECT_IO)
		(*fltobj)->Flags |= DO_DIRECT_IO;
	if (oldobj->Flags & DO_BUFFERED_IO)
		(*fltobj)->Flags |= DO_BUFFERED_IO;
	if (oldobj->Characteristics & FILE_DEVICE_SECURE_OPEN)
		(*fltobj)->Characteristics |= FILE_DEVICE_SECURE_OPEN;
	(*fltobj)->Flags |= DO_POWER_PAGABLE;
	
	/* ��һ���豸����һ���豸�� */
	topdev = IoAttachDeviceToDeviceStack(*fltobj, oldobj);
	if (NULL == topdev)//��ʧ���������豸
	{
		IoDeleteDevice(*fltobj);
		*fltobj = NULL;
		status = STATUS_UNSUCCESSFUL;
		return status;
	}
	*next = topdev;

	/* ��������豸�Ѿ����� */
	(*fltobj)->Flags = (*fltobj)->Flags & ~DO_DEVICE_INITIALIZING;
	return STATUS_SUCCESS;
}

/* �����еĴ��� */
VOID ccpAttachAllComs(PDRIVER_OBJECT driver)
{
	ULONG i;
	PDEVICE_OBJECT com_ob;
	NTSTATUS status;
	for (i = 0; i < CCP_MAX_COM_ID; ++i)
	{
		com_ob = ccpOpenCom(i, &status);//��� object ����
		if (NULL == com_ob)//��ȡʧ��
			continue;
		ccpAttachDevice(driver, com_ob, &s_fltobj[i], &s_nextobj[i]);//ȡ�� object ����
	}
	return;
}

NTSTATUS ccpDispatch(PDEVICE_OBJECT device, PIRP irp)
{
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(irp);

	for (ULONG i = 0; i < CCP_MAX_COM_ID; ++i)//���ȵ�֪�����͸����ĸ��豸���豸һ����� CCP_MAX_COM_ID ������ǰ��Ĵ��뱣��õģ����� s_fltobj ��
		if (s_fltobj[i] == device)
		{
			if (irpsp->MajorFunction == IRP_MJ_POWER)//���е�Դ����ȫ��ֱ�ӷŹ�
			{
				PoStartNextPowerIrp(irp);//ֱ�ӷ���Ȼ�󷵻�˵�Ѿ���������
				IoSkipCurrentIrpStackLocation(irp);
				return PoCallDriver(s_nextobj[i], irp);
			}

			if (irpsp->MajorFunction == IRP_MJ_WRITE)//����ʾд����
			{
				ULONG len = irpsp->Parameters.Write.Length;//�Ȼ�ó���
				PUCHAR buf = NULL;
				if (irp->MdlAddress != NULL)//Ȼ���û�����
					buf = (PUCHAR)MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
				else
					buf = (PUCHAR)irp->UserBuffer;
				if (NULL == buf)
					buf = (PUCHAR)irp->AssociatedIrp.SystemBuffer;
				if (NULL == buf)
				{
					DbgPrint("%s->%wZ->Get buffer failed! \n", _KMDFSerial_H, device->DriverObject->DriverName);
					continue;
				}

				for (ULONG j = 0; j < len; ++j)//��ӡ����
					DbgPrint("%s->%wZ->Send Data->%2x\n", _KMDFSerial_H, device->DriverObject->DriverName, buf[j]);
			}

			IoSkipCurrentIrpStackLocation(irp);//������ش���
			return IoCallDriver(s_nextobj[i], irp);
		}

	/* ��������Ͳ��ڱ��󶨵��豸�У�����������ģ�ֱ�ӷ��ز������� */
	irp->IoStatus.Information = 0;
	irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}



/** ��������ں��� **/
/* ����ֹͣ */
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);//�ֶ���Ŀ
	LARGE_INTEGER interval;

	DbgPrint("%s->DriverUnload()->IoDetachDevice\n", _KMDFSerial_H);
	for (ULONG i = 0; i < CCP_MAX_COM_ID; ++i)//�����
		if (s_nextobj[i] != NULL)//�ǿ�������
			IoDetachDevice(s_nextobj[i]);

	DbgPrint("%s->DriverUnload()->WaitingforIrpEnding->%ds\n", _KMDFSerial_H, DELAY_SECOND_CNT);
	interval.QuadPart = (DELAY_SECOND_CNT * DELAY_ONE_SECOND);//�ȴ����� irp �������
	KeDelayExecutionThread(KernelMode, FALSE, &interval);

	DbgPrint("%s->DriverUnload()->IoDeleteDevice\n", _KMDFSerial_H);
	for (ULONG i = 0; i < CCP_MAX_COM_ID; ++i)//ɾ����Щ�豸
		if (s_fltobj[i] != NULL)//�ǿ�������
			IoDeleteDevice(s_fltobj[i]);

	DbgPrint("%s->DriverUnload()\n", _KMDFSerial_H);
	return;
}

/* �������� */
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
	UNREFERENCED_PARAMETER(reg_path);//�ֶ���Ŀ
	DbgPrint("%s->DriverEntry()\n", _KMDFSerial_H);

	for (size_t i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)//�����ɷ����������ó�һ���ġ�
		driver->MajorFunction[i] = ccpDispatch;
	
	driver->DriverUnload = DriverUnload;//֧�ֶ�̬ж��
	ccpAttachAllComs(driver);//�����еĴ���

	return STATUS_SUCCESS;
}