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
#ifndef INFO_SEP
#define INFO_SEP '\n'
#endif
#ifndef INFO_MEMBER_CNT
#define INFO_MEMBER_CNT 6
#endif
#ifndef INFO_SEP_CNT
#define INFO_SEP_CNT 5
#endif
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
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


/** �ṹ�� **/
typedef struct
{
	char uOpType;// flag = 0
	char uPath[MAX_PATH];// flag = 1
	char uSubKeyType;// flag = 2 (fveh)
	char uSubKey[MAX_PATH];// flag = 3
	char uValueType;// flag = 4
	char uValue[MAX_PATH];// flag = 5
} ZwOpRegStruct;


/** �ַ��������Ӻ��� **/
/* �Ƿ�Ϸ���Ϣ */
BOOLEAN isValidStructer(char buffer[], int newline_index[INFO_MEMBER_CNT], ULONG inlen)
{
	if (inlen >= MAX_PATH << 2)
		return FALSE;
	newline_index[INFO_MEMBER_CNT - 1] = inlen;
	ULONG cnt = 0;
	for (ULONG i = 0; i < inlen; ++i)
		if (INFO_SEP == buffer[i])
		{
			if (cnt >= INFO_SEP_CNT)//�ָ�������̫��
				return FALSE;
			newline_index[cnt++] = i;
		}
	if (INFO_SEP_CNT != cnt)//�ָ�������̫��
		return FALSE;
	if (newline_index[0] != 1 || newline_index[2] - newline_index[1] != 2 || newline_index[4] - newline_index[3] != 2)//������һ���ַ�
		return FALSE;
	for (int i = 1; i < INFO_MEMBER_CNT - 1; ++i)//�������ݳ��ȣ��� 1 ��ʼ��鼴�ɣ�
		if (newline_index[i + 1] - newline_index[i] > MAX_PATH)
			return FALSE;
	return TRUE;
}

/* R3 ת R0 ע���·�� */
BOOLEAN transReg(char uPath[MAX_PATH])
{
	if (strlen(uPath) >= 4)
	{
		if (('H' == uPath[0] || 'h' == uPath[0])
			&& ('K' == uPath[1] || 'k' == uPath[1])
			&& ('L' == uPath[2] || 'l' == uPath[2])
			&& ('M' == uPath[3] || 'm' == uPath[3])
		)
		{
			char uTmp[MAX_PATH] = { 0 };
			strcpy(uTmp, "\\registry\\machine");
			if (strlen(uPath) - 4 + strlen(uTmp) >= MAX_PATH)//������
				return FALSE;
			strcat_s(uTmp, MAX_PATH, &uPath[4]);
			strcpy(uPath, uTmp);
			return TRUE;
		}
	}
	return FALSE;
}


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
			ANSI_STRING NTPath, NTSubKey;
			UNICODE_STRING uPath, uSubKey;
			RtlInitUnicodeString(&uPath, L"");
			RtlInitUnicodeString(&uSubKey, L"");

			/* ����ǰ�涨��õ��豸�����ܺţ���ϸ�ж����뻹�����������������ֻ�ж����� */
			switch (irpsp->Parameters.DeviceIoControl.IoControlCode)
			{
			case CWK_DVC_SEND_STR:
				ASSERT(buffer != NULL && inlen < MAX_PATH << 2 && inlen > 0 && 0 == outlen);
				if (NULL == buffer || inlen >= MAX_PATH << 2 || inlen <= 0 || 0 != outlen)
				{
					DbgPrint("%s->ASSERT(buffer != NULL && inlen < MAX_PATH << 2 && inlen > 0 && 0 == outlen)", _ZwOpReg_H);
					break;
				}
				char bufIN[MAX_PATH << 2] = { 0 };
				int cur_index = 0, newline_index[INFO_MEMBER_CNT] = { 0 };
				strcpy_s(bufIN, inlen, (char*)buffer);
				bufIN[0] = ((char*)buffer)[0];
				if (!isValidStructer(bufIN, newline_index, inlen))//����Ƿ�Ϸ�����÷ָ�λ��
				{
					DbgPrint("%s->UnknownMsg->\"%s\"\n", _ZwOpReg_H, bufIN);//��Чָ��䵱������Ϣ
					break;
				}

				/* ��ʼ�и��յ������� */
				ZwOpRegStruct regInfo = { '0', { 0 }, '0', { 0 }, '0', { 0 } };
				regInfo.uOpType = bufIN[0];
				cur_index = 0;
				for (int i = newline_index[0] + 1; i < newline_index[1]; ++i)
					regInfo.uPath[cur_index++] = bufIN[i];
				regInfo.uSubKeyType = bufIN[newline_index[1] + 1];
				cur_index = 0;
				for (int i = newline_index[2] + 1; i < newline_index[3]; ++i)
					regInfo.uSubKey[cur_index++] = bufIN[i];
				regInfo.uValueType = bufIN[newline_index[3] + 1];
				cur_index = 0;
				for (int i = newline_index[4] + 1; i < newline_index[5]; ++i)
					regInfo.uValue[cur_index++] = bufIN[i];

				/* R3 ת R0 ���ַ������� */
				if (!transReg(regInfo.uPath))
				{
					DbgPrint("%s->InvalidPath->\"%s\"\n", _ZwOpReg_H, regInfo.uPath);
					break;
				}

				/* ��ӡ����ֶ���Ϣ */
				DbgPrint("%s->GetWmirMsg()->uOpType = \'%c\'\n", _ZwOpReg_H, regInfo.uOpType);
				DbgPrint("%s->GetWmirMsg()->uPath = \"%s\"\n", _ZwOpReg_H, regInfo.uPath);
				DbgPrint("%s->GetWmirMsg()->uSubKeyType = \'%c\'\n", _ZwOpReg_H, regInfo.uSubKeyType);
				DbgPrint("%s->GetWmirMsg()->uSubKey = \"%s\"\n", _ZwOpReg_H, regInfo.uSubKey);
				DbgPrint("%s->GetWmirMsg()->uValueType = \'%c\'\n", _ZwOpReg_H, regInfo.uValueType);
				DbgPrint("%s->GetWmirMsg()->uValue = \"%s\"\n", _ZwOpReg_H, regInfo.uValue);

				/* ִ����ز��� */
				RtlInitAnsiString(&NTPath, regInfo.uPath);
				RtlInitAnsiString(&NTSubKey, regInfo.uSubKey);
				RtlAnsiStringToUnicodeString(&uPath, &NTPath, TRUE);
				RtlAnsiStringToUnicodeString(&uSubKey, &NTSubKey, TRUE);

				switch (bufIN[0])
				{
				case 'A':
				case 'a'://ִ�� add
					ntIBinaryCreateKey(uPath, uSubKey);
					break;
				case 'D':
				case 'd'://ִ�� del
					break;
				case 'S':
				case 's'://ִ�� set
					break;
				case 'Q':
				case 'q'://ִ�� query
					break;
				default:
					DbgPrint("%s->UnknownMsg->\"%s\"\n", _ZwOpReg_H, bufIN);//��Чָ��䵱������Ϣ
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