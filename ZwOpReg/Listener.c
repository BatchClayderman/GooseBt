#include <ntifs.h>
#include <ntstrsafe.h>
#include "Listener.h"
#ifndef _ZwOpReg_H
#define _ZwOpReg_H "ZwOpReg"
#ifndef NULL
#define NULL ((void *)0)
#endif
#ifndef VOID
#defnie VOID void
#endif
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#ifdef DeviceName
#undef DeviceName
#endif
#define DeviceName L"\\Device\\GooseBtZwOpReg"
#ifdef CWK_CDO_SYB_NAME
#undef CWK_CDO_SYB_NAME
#endif
#define CWK_CDO_SYB_NAME  L"\\??\\GooseBtZwOpReg"//�� R0 ������ӷ���

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
#endif//_ZwOpReg_H
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
				ASSERT(buffer != NULL && inlen < MAX_PATH&& inlen > 0 && 0 == outlen);
				if (NULL == buffer || inlen >= MAX_PATH || inlen <= 0 || 0 != outlen)
				{
					DbgPrint("%s->ASSERT(buffer != NULL && inlen < MAX_PATH && inlen > 0 && 0 == outlen)", _ZwOpReg_H);
					break;
				}
				short int choice = 0;//���󷽰�
				char bufIN[MAX_PATH] = { 0 };
				strcpy_s(bufIN, sizeof(bufIN) / sizeof(char), (char*)buffer);
				if (strlen(bufIN) > 4//��ֹ����
					&& (bufIN[0] == 'A' || bufIN[0] == 'a')
					&& (bufIN[1] == 'D' || bufIN[1] == 'd')
					&& (bufIN[2] == 'D' || bufIN[2] == 'd')
					&& (bufIN[3] == ' ')
					)// add ָ��
				{
					choice = 1;//���� /add
					strcpy_s(bufIN, sizeof(bufIN) / sizeof(char), &bufIN[4]);
					DbgPrint("%s->GetWmirMsg()->add(\"%s\")\n", _ZwOpReg_H, bufIN);
				}
				else if (strlen(bufIN) > 4//��ֹ����
					&& (bufIN[0] == 'D' || bufIN[0] == 'd')
					&& (bufIN[1] == 'E' || bufIN[1] == 'e')
					&& (bufIN[2] == 'L' || bufIN[2] == 'l')
					&& (bufIN[3] == ' ')
					)// del ָ��
				{
					choice = 2;//���� del
					strcpy_s(bufIN, sizeof(bufIN) / sizeof(char), &bufIN[4]);
					DbgPrint("%s->GetWmirMsg()->del(\"%s\")\n", _ZwOpReg_H, bufIN);
				}
				else if (strlen(bufIN) > 4//��ֹ����
					&& (bufIN[0] == 'S' || bufIN[0] == 's')
					&& (bufIN[1] == 'E' || bufIN[1] == 'e')
					&& (bufIN[2] == 'T' || bufIN[2] == 't')
					&& (bufIN[3] == ' ')
					)// set ָ��
				{
					choice = 3;//���� set
					strcpy_s(bufIN, sizeof(bufIN) / sizeof(char), &bufIN[4]);
					DbgPrint("%s->GetWmirMsg()->set(\"%s\")\n", _ZwOpReg_H, bufIN);
				}
				else if (strlen(bufIN) > 6//��ֹ����
					&& (bufIN[0] == 'Q' || bufIN[0] == 'q')
					&& (bufIN[1] == 'U' || bufIN[1] == 'u')
					&& (bufIN[2] == 'E' || bufIN[2] == 'e')
					&& (bufIN[3] == 'R' || bufIN[3] == 'r')
					&& (bufIN[4] == 'Y' || bufIN[4] == 'y')
					&& (bufIN[5] == ' ')
					)// query ָ��
				{
					choice = 4;//���� query
					strcpy_s(bufIN, sizeof(bufIN) / sizeof(char), &bufIN[4]);
					DbgPrint("%s->GetWmirMsg()->query\"%s\")\n", _ZwOpReg_H, bufIN);
				}
				else
					DbgPrint("%s->UnknownMsg->\"%s\"\n", _ZwOpReg_H, bufIN);//��Чָ��䵱������Ϣ
				switch (choice)
				{
				case 1://ִ�� add
					break;
				case 2://ִ�� del
					break;
				case 3://ִ�� set
					break;
				case 4://ִ�� query
					break;
				default:
					break;
				}
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
		DbgPrint(_ZwOpReg_H);
		DbgPrint("->IoCreateDevice()->Failed!\n");
		return status;
	}

	/* �����еķַ���������Ϊ�Զ���� */
	for (ULONG i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
		pDriver->MajorFunction[i] = cwkDispatch;
	DbgPrint(_ZwOpReg_H);
	DbgPrint("->IoCreateDevice()->Successful!\n");

	/* ��������豸�ĳ�ʼ����� */
	g_cdo->Flags &= ~DO_DEVICE_INITIALIZING;
	return STATUS_SUCCESS;
}