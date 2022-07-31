#include <ntifs.h>
#include <ntstrsafe.h>
#include "Listener.h"
#ifndef _ZwTerminateProcess_H
#define _ZwTerminateProcess_H "ZwTerminateProcess"
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
#define DeviceName L"\\Device\\GooseBtZwTerminateProcess"
#ifdef CWK_CDO_SYB_NAME
#undef CWK_CDO_SYB_NAME
#endif
#define CWK_CDO_SYB_NAME  L"\\??\\GooseBtZwTerminateProcess"//�� R0 ������ӷ���

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
#endif//_ZwTerminateProcess_H
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
					DbgPrint("%s->ASSERT(buffer != NULL && inlen < MAX_PATH && inlen > 0 && 0 == outlen)", _ZwTerminateProcess_H);
					break;
				}
				short int choice = 0;//���󷽰�
				char bufIN[MAX_PATH] = { 0 };
				DWORD pid = 0;//Ŀ�� PID
				DWORD* pointer;
				pointer = &pid;
				strcpy_s(bufIN, sizeof(bufIN) / sizeof(char), (char*)buffer);
				DbgPrint("%s->GetwmipMsg()->", _ZwTerminateProcess_H);
				if (strlen(bufIN) == 3//��ֹ����
					&& (bufIN[0] == '/' || bufIN[0] == '-')
					&& (bufIN[1] == 'D' || bufIN[1] == 'd')
					&& (bufIN[2] == 'R' || bufIN[2] == 'r')
					)// /dr ָ��
				{
					DbgPrint("DrivenReboot()\n");
					DrivenReboot();
					return STATUS_SUCCESS;
				}
				else if (strlen(bufIN) == 3//��ֹ����
					&& (bufIN[0] == '/' || bufIN[0] == '-')
					&& (bufIN[1] == 'D' || bufIN[1] == 'd')
					&& (bufIN[2] == 'S' || bufIN[2] == 's')
					)// /df ָ��
				{
					DbgPrint("DrivenShutdown()");
#if (defined _WIN64 || defined WIN64)
					DbgPrint("->x86 Only!\n");
					return STATUS_ABANDONED;
#else
					DbgPrint("\n");
					DrivenShutdown();
					return STATUS_SUCCESS;
#endif
				}
				else if (strlen(bufIN) == 3//��ֹ����
					&& (bufIN[0] == '/' || bufIN[0] == '-')
					&& (bufIN[1] == 'D' || bufIN[1] == 'd')
					&& (bufIN[2] == 'E' || bufIN[2] == 'e')
					)// /df ָ��
				{
					DbgPrint("DrivenError()\n");
					DrivenError();
					return STATUS_SUCCESS;
				}
				else if (strlen(bufIN) > 4//��ֹ����
					&& (bufIN[0] == '/' || bufIN[0] == '-')
					&& (bufIN[1] == 'I' || bufIN[1] == 'i')
					&& (bufIN[2] == 'M' || bufIN[2] == 'm')
					&& (bufIN[3] == ' ')
					)// /im ָ��
				{
					choice = 1;//���� /im
					strcpy_s(bufIN, sizeof(bufIN) / sizeof(char), &bufIN[4]);
					DbgPrint("ZwTerminateProcessByProcessName(\"%s\")", bufIN);
				}
				else if (strlen(bufIN) > 5//��ֹ����
					&& (bufIN[0] == '/' || bufIN[0] == '-')
					&& (bufIN[1] == 'P' || bufIN[1] == 'p')
					&& (bufIN[2] == 'I' || bufIN[2] == 'i')
					&& (bufIN[3] == 'D' || bufIN[3] == 'd')
					&& (bufIN[4] == ' ')
					)// /pid ָ��
				{
					strcpy_s(bufIN, sizeof(bufIN) / sizeof(char), &bufIN[5]);
					if (StringToULONG(bufIN, pointer))
					{
						choice = 2;//���� /pid
						DbgPrint("ZwTerminateProcessByPID(%s)", bufIN);
					}
					else
						DbgPrint("UnknownMsg->\"%s\"", bufIN);//�䵱��Чָ��
				}
				else if (strlen(bufIN) == 4//ֱ��ָ��
					&& (bufIN[0] == '/' || bufIN[0] == '-')
					&& (bufIN[1] == 'A' || bufIN[1] == 'a')
					&& (bufIN[2] == 'L' || bufIN[2] == 'l')
					&& (bufIN[3] == 'L' || bufIN[3] == 'l')
					)// /pid ָ��
				{
					choice = 3;//���� /all
					DbgPrint("ZwTerminateAll()");
				}
				else
					DbgPrint("UnknownMsg->\"%s\"", bufIN);//��Чָ��䵱������Ϣ
				DbgPrint("\n");
				switch (choice)
				{
				case 1://ִ�� /im
					ZwKillImage(bufIN);
					break;
				case 2://ִ�� /pid
					PIDCallTerminate(pid);
					break;
				case 3:
					for (DWORD i = 0; i <= 240000; i += 4)
						PIDCallTerminate(240000 - i);//�Ӻ���ǰɱ
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
		DbgPrint(_ZwTerminateProcess_H);
		DbgPrint("->IoCreateDevice()->Failed!\n");
		return status;
	}

	/* �����еķַ���������Ϊ�Զ���� */
	for (ULONG i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
		pDriver->MajorFunction[i] = cwkDispatch;
	DbgPrint(_ZwTerminateProcess_H);
	DbgPrint("->IoCreateDevice()->Successful!\n");

	/* ��������豸�ĳ�ʼ����� */
	g_cdo->Flags &= ~DO_DEVICE_INITIALIZING;
	return STATUS_SUCCESS;
}