#include <ntifs.h>
#include <ntddk.h>
#include <ntstrsafe.h>
#include "Listener.h"
#ifndef _ZwDeleteFile_H
#define _ZwDeleteFile_H "ZwDeleteFile"
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#ifdef DeviceName
#undef DeviceName
#endif
#define DeviceName L"\\Device\\GooseBtZwDeleteFile"
#ifdef CWK_CDO_SYB_NAME
#undef CWK_CDO_SYB_NAME
#endif
#define CWK_CDO_SYB_NAME  L"\\??\\GooseBtZwDeleteFile"//�� R0 ������ӷ���

/* ��Ӧ�ò����������һ���ַ��� */
#ifndef CWK_DVC_SEND_STR
#define CWK_DVC_SEND_STR       \
	(ULONG)CTL_CODE(           \
	FILE_DEVICE_UNKNOWN,       \
	0x911,METHOD_BUFFERED,     \
	FILE_WRITE_DATA            \
)
#endif

/* ��������ȡһ���ַ��� */
#ifndef CWK_DVC_RECV_STR
#define CWK_DVC_RECV_STR       \
	(ULONG)CTL_CODE(           \
	FILE_DEVICE_UNKNOWN,       \
	0x912,METHOD_BUFFERED,     \
	FILE_READ_DATA             \
)
#endif
#endif//_ZwDeleteFile_H
PDEVICE_OBJECT g_cdo = NULL;//�����豸


/** ͨ�ź��� **/
NTSTATUS cwkDispatch(IN PDEVICE_OBJECT dev, IN PIRP irp)//�ַ�����
{
	/* ��ȡ��ǰ irp ջ�ռ� */
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(irp);
	NTSTATUS status = STATUS_SUCCESS;
	ULONG ret_len = 0;//���󳤶�

	/* ѭ�������͸� g_cdo ������������Ƿ��͸� g_cdo�������Դ��� */
	while (dev == g_cdo)
	{
		if (irpsp->MajorFunction == IRP_MJ_CREATE || irpsp->MajorFunction == IRP_MJ_CLOSE)// ���ɺ͹ر�����һ����������
			break;
		if (irpsp->MajorFunction == IRP_MJ_DEVICE_CONTROL)
		{
			/* ���� DeviceIoControl */
			PVOID buffer = irp->AssociatedIrp.SystemBuffer;//��������ַ
			//ULONG inlen = irpsp->Parameters.DeviceIoControl.InputBufferLength;//���뻺��������
			//ULONG outlen = irpsp->Parameters.DeviceIoControl.OutputBufferLength;//�������������

			/* ����ǰ�涨��õ��豸�����ܺţ���ϸ�ж����뻹�����������������ֻ�ж����� */
			switch (irpsp->Parameters.DeviceIoControl.IoControlCode)
			{
			case CWK_DVC_SEND_STR:
				ASSERT(buffer != NULL);
				ASSERT(inlen > 0);
				ASSERT(outlen == 0);
				char temp[MAX_PATH] = { 0 };
				strcpy_s(temp, MAX_PATH, (char*)buffer);
				DbgPrint(_ZwDeleteFile_H);
				DbgPrint("->GetScannerMsg()->\\??\\%s\n", temp);
				wchar_t tmp[MAX_PATH] = { 0 };
				swprintf_s(tmp, MAX_PATH, L"\\??\\%hs", temp);
				UNICODE_STRING uDeletePathName;
				uDeletePathName.Buffer = tmp;
				uDeletePathName.Length = (USHORT)((wcslen(tmp)) * sizeof(wchar_t));
				IBinaryNtZwDeleteFile(uDeletePathName);
				IBinaryNtSetInformationFileDeleteFile(uDeletePathName);
				break;
			case CWK_DVC_RECV_STR:
			default://������������ǲ����յģ�����û���ý��գ�
				status = STATUS_INVALID_PARAMETER;//δ֪������һ�ɷ��طǷ���������
				break;
			}
		}
		break;
	}

	/* ���Ƿ��͸� g_cdo�����Դ��� */
	irp->IoStatus.Information = ret_len;//��д������Ϣ
	irp->IoStatus.Status = status;//��д���ؽ��
	IoCompleteRequest(irp, IO_NO_INCREMENT);//�������
	return status;
}

VOID listenerUnload()//���ͨѶ����
{
	UNICODE_STRING cdo_syb = RTL_CONSTANT_STRING(CWK_CDO_SYB_NAME);//���������ַ���
	ASSERT(g_cdo != NULL);
	IoDeleteSymbolicLink(&cdo_syb);//ɾ����������
	IoDeleteDevice(g_cdo);//ɾ�������豸
	return;
}

NTSTATUS listenerEntry(PDRIVER_OBJECT pDriver)
{
	/* ����һ�������豸Ȼ�����ɷ������� */
	//UNICODE_STRING sddl = RTL_CONSTANT_STRING(L"D:P(A;;GA;;;WD)");//ͨ�ã�ֱ�Ӹ��Ƽ��ɣ�
	UNICODE_STRING cdo_name = RTL_CONSTANT_STRING(DeviceName);//�豸��
	UNICODE_STRING cdo_syb = RTL_CONSTANT_STRING(CWK_CDO_SYB_NAME);//��������

	/* ����һ���豸���� */
	NTSTATUS status = IoCreateDevice(
		pDriver,
		0, &cdo_name,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&g_cdo
	);
	if (!NT_SUCCESS(status))
		return status;

	/* �������ӷ��Ų����豸�� */
	IoDeleteSymbolicLink(&cdo_syb);//Ϊ��ֹ��ͻ��Ԥ��ɾ��
	status = IoCreateSymbolicLink(&cdo_syb, &cdo_name);//�����ӷ������豸��
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(g_cdo);//ɾ���豸
		DbgPrint(_ZwDeleteFile_H);
		DbgPrint("->IoCreateDevice()->Failed!\n");
		return status;
	}

	/* �����еķַ���������Ϊ�Զ���� */
	for (ULONG i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
		pDriver->MajorFunction[i] = cwkDispatch;
	DbgPrint(_ZwDeleteFile_H);
	DbgPrint("->IoCreateDevice()->Successful!\n");

	/* ��������豸�ĳ�ʼ����� */
	g_cdo->Flags &= ~DO_DEVICE_INITIALIZING;
	return STATUS_SUCCESS;
}