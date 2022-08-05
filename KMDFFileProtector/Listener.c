#include <ntifs.h>
#include <ntddk.h>
#include <ntstrsafe.h>
#include "Listener.h"
#ifndef _KMDFFileProtector_H
#define _KMDFFileProtector_H "KMDFFileProtector"
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#ifdef DeviceName
#undef DeviceName
#endif
#define DeviceName L"\\Device\\GooseBtKMDFFileProtector"
#ifdef CWK_CDO_SYB_NAME
#undef CWK_CDO_SYB_NAME
#endif
#define CWK_CDO_SYB_NAME  L"\\??\\GooseBtKMDFFileProtector"//�� R0 ������ӷ���

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
#endif//_KMDFFileProtector_H
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
			ULONG inlen = irpsp->Parameters.DeviceIoControl.InputBufferLength;//���뻺��������
			ULONG outlen = irpsp->Parameters.DeviceIoControl.OutputBufferLength;//�������������

			/* ����ǰ�涨��õ��豸�����ܺţ���ϸ�ж����뻹�����������������ֻ�ж����� */
			switch (irpsp->Parameters.DeviceIoControl.IoControlCode)
			{
			case CWK_DVC_SEND_STR:
				ASSERT(buffer != NULL && inlen < MAX_PATH && inlen > 0 && 0 == outlen);
				if (NULL == buffer || inlen >= MAX_PATH || inlen <= 0 || 0 != outlen)
				{
					DbgPrint("%s->ASSERT(buffer != NULL && inlen < MAX_PATH && inlen > 0 && 0 == outlen)\n", _KMDFFileProtector_H);
					break;
				}
				wchar_t tmp[MAX_PATH] = { 0 };
				swprintf_s(tmp, MAX_PATH, L"\\??\\%hs", (char*)buffer);
				UNICODE_STRING uProtectPathName;
				uProtectPathName.Buffer = tmp;
				uProtectPathName.Length = (USHORT)((inlen + 4) * sizeof(wchar_t));
				DbgPrint("%s->GetWmifMsg()->KMDFProtect(\"%wZ\")\n", _KMDFFileProtector_H, &uProtectPathName);
				NtOccurFile(uProtectPathName);
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
	{
		DbgPrint("%s->IoCreateDevice()->Failed! \n", _KMDFFileProtector_H);
		return status;
	}

	/* �������ӷ��Ų����豸�� */
	IoDeleteSymbolicLink(&cdo_syb);//Ϊ��ֹ��ͻ��Ԥ��ɾ��
	status = IoCreateSymbolicLink(&cdo_syb, &cdo_name);//�����ӷ������豸��
	if (!NT_SUCCESS(status))
	{
		DbgPrint("%s->IoCreateSymbolicLink()->Failed! \n", _KMDFFileProtector_H);
		IoDeleteDevice(g_cdo);//ɾ���豸
		return status;
	}

	/* �����еķַ���������Ϊ�Զ���� */
	for (ULONG i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
		pDriver->MajorFunction[i] = cwkDispatch;
	DbgPrint("%s->listenerEntry()->Successful! \n", _KMDFFileProtector_H);

	/* ��������豸�ĳ�ʼ����� */
	g_cdo->Flags &= ~DO_DEVICE_INITIALIZING;
	return STATUS_SUCCESS;
}