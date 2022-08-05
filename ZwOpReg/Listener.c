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
	while ('\\' == uPath[strlen(uPath) - 1])//������һ���ַ���'\\'��ֱ��ȥ��
		uPath[strlen(uPath) - 1] = 0;
	while ('\\' == uPath[0])//�����һ���ַ���'\\'��ֱ��ȥ��
	{
		char uTmp[MAX_PATH] = { 0 };
		strcpy_s(uTmp, MAX_PATH, uPath);
		strcpy_s(uPath, MAX_PATH, &uTmp[1]);
	}
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
		else if (('H' == uPath[0] || 'h' == uPath[0])
			&& ('K' == uPath[1] || 'k' == uPath[1])
			&& ('C' == uPath[2] || 'c' == uPath[2])
			&& ('U' == uPath[3] || 'u' == uPath[3])
			)
		{
			char uTmp[MAX_PATH] = { 0 };
			strcpy(uTmp, "\\registry\\user");
			if (strlen(uPath) - 4 + strlen(uTmp) >= MAX_PATH)//������
				return FALSE;
			strcat_s(uTmp, MAX_PATH, &uPath[4]);
			strcpy(uPath, uTmp);
			return TRUE;
		}
		else if (('H' == uPath[0] || 'h' == uPath[0])
			&& ('K' == uPath[1] || 'k' == uPath[1])
			&& ('C' == uPath[2] || 'c' == uPath[2])
			&& ('R' == uPath[3] || 'r' == uPath[3])
			)
		{
			char uTmp[MAX_PATH] = { 0 };
			strcpy(uTmp, "\\registry\\root");
			if (strlen(uPath) - 4 + strlen(uTmp) >= MAX_PATH)//������
				return FALSE;
			strcat_s(uTmp, MAX_PATH, &uPath[4]);
			strcpy(uPath, uTmp);
			return TRUE;
		}
		else if (('H' == uPath[0] || 'h' == uPath[0])
			&& ('K' == uPath[1] || 'k' == uPath[1])
			&& ('C' == uPath[2] || 'c' == uPath[2])
			&& ('C' == uPath[3] || 'c' == uPath[3])
			)
		{
			char uTmp[MAX_PATH] = { 0 };
			strcpy(uTmp, "\\registry\\config");
			if (strlen(uPath) - 4 + strlen(uTmp) >= MAX_PATH)//������
				return FALSE;
			strcat_s(uTmp, MAX_PATH, &uPath[4]);
			strcpy(uPath, uTmp);
			return TRUE;
		}
	}
	else if (strlen(uPath) >= 3)
	{
		if (('H' == uPath[0] || 'h' == uPath[0])
			&& ('K' == uPath[1] || 'k' == uPath[1])
			&& ('U' == uPath[2] || 'u' == uPath[2])
			)
		{
			char uTmp[MAX_PATH] = { 0 };
			strcpy(uTmp, "\\registry\\users");
			if (strlen(uPath) - 3 + strlen(uTmp) >= MAX_PATH)//������
				return FALSE;
			strcat_s(uTmp, MAX_PATH, &uPath[3]);
			strcpy(uPath, uTmp);
			return TRUE;
		}
	}
	return FALSE;
}

/* �ָ�·�� */
BOOLEAN separatePath(char uPath[MAX_PATH], char uSubKey[MAX_PATH])
{
	int index = (int)strlen(uPath) - 1, cnt = 0;// cnt ��¼��б������
	for (; index > -1 && uPath[index] != '\\'; --index);//��λ���һ���ַ�'\\'��λ��
	for (int i = index - 1; i > -1; --i)//����ʣ�෴б������������Ϊ 2��
		if ('\\' == uPath[i] && ++cnt >= 2)//�ѳ��� 2
			break;
	if (cnt < 2)
		return FALSE;
	strcpy_s(uSubKey, MAX_PATH, &uPath[index + 1]);//���Ƴ�����
	for (int i = (int)strlen(uPath) - 1; i >= index; --i)//�Ӻ���ǰ����������б�ܣ�
		uPath[i] = 0;
	return TRUE;
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
			ANSI_STRING NTPath, NTSubKey, NTValue;
			UNICODE_STRING uPath, uSubKey, uValue;
			ULONG ul = 0;
			LONG64 ull = 0;
			PWSTR endptr;
			RtlInitUnicodeString(&uPath, L"");
			RtlInitUnicodeString(&uSubKey, L"");
			RtlInitUnicodeString(&uValue, L"");

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
				RtlInitAnsiString(&NTValue, regInfo.uValue);
				RtlAnsiStringToUnicodeString(&uPath, &NTPath, TRUE);
				RtlAnsiStringToUnicodeString(&uSubKey, &NTSubKey, TRUE);
				RtlAnsiStringToUnicodeString(&uValue, &NTValue, TRUE);

				/* ������Ӧ�طַ� */
				switch (bufIN[0])
				{
				case 'A':
				case 'a'://ִ�� add
					switch (regInfo.uSubKeyType)
					{
					case 'F':
					case 'f':
						if (!separatePath(regInfo.uPath, regInfo.uSubKey))
						{
							DbgPrint("%s->Invalid RegistryPath->\"%s\"\n", _ZwOpReg_H, regInfo.uPath);//�쳣�ָ�·��
							break;
						}
						RtlInitAnsiString(&NTPath, regInfo.uPath);
						RtlInitAnsiString(&NTSubKey, regInfo.uSubKey);
						RtlAnsiStringToUnicodeString(&uPath, &NTPath, TRUE);
						RtlAnsiStringToUnicodeString(&uSubKey, &NTSubKey, TRUE);
					case 'I':
					case 'i':
						ntIBinaryCreateKey(uPath, uSubKey);
						break;
					case 'E':
					case 'e':
						RtlInitUnicodeString(&uSubKey, L"");//Ĭ�ϼ����������볣���һ��
					case 'K':
					case 'k':
						switch (regInfo.uValueType)
						{
						case 'S':
						case 's':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_SZ, uValue.Buffer, uValue.Length);//һ��Ҫ�� wchar_t
							break;
						case 'B':
						case 'b':
							if (!NT_SUCCESS(RtlUnicodeStringToInteger(&uValue, 0, &ul)))
							{
								DbgPrint("%s->Invalid Binary->\"%s\"", _ZwOpReg_H, regInfo.uValue);
								break;
							}
							ntIBinarySetKeyValue(uPath, uSubKey, REG_BINARY, regInfo.uValue, sizeof(ULONG));
							break;
						case 'D':
						case 'd':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_DWORD, regInfo.uValue, (ULONG)strlen(regInfo.uValue));
							break;
						case 'Q':
						case 'q':
							if (!NT_SUCCESS(RtlUnicodeStringToInt64(&uValue, 0, &ull, &endptr)))
							{
								DbgPrint("%s->Invalid UnsignedLongLong->\"%s\"", _ZwOpReg_H, regInfo.uValue);
								break;
							}
							ntIBinarySetKeyValue(uPath, uSubKey, REG_QWORD,&ull, sizeof(ULONG64));
							break;
						case 'M':
						case 'm':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_MULTI_SZ, uValue.Buffer, uValue.Length);//һ��Ҫ�� wchar_t
							break;
						case 'E':
						case 'e':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_EXPAND_SZ, uValue.Buffer, uValue.Length);//һ��Ҫ�� wchar_t
							break;
						case 'N':
						case 'n':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_NONE, regInfo.uValue, (ULONG)strlen(regInfo.uValue));//һ��Ҫ�� char
							break;
						default:
							DbgPrint("%s->Invalid ValueType->\'%c\'\n", _ZwOpReg_H, regInfo.uValueType);//��Чֵ����
							break;
						}
						break;
					default:
						DbgPrint("%s->Invalid SubKeyType->\'%c\'\n", _ZwOpReg_H, regInfo.uSubKeyType);//��Ч������
						break;
					}
					break;
				case 'D':
				case 'd'://ִ�� del
					switch (regInfo.uSubKeyType)
					{
					case 'E':
					case 'e':
						RtlInitUnicodeString(&uSubKey, L"");
					case 'K':
					case 'k':
						ntIBinaryDeleteKey(uPath, uSubKey, FALSE);
						break;
					case 'F':
					case 'f':
						if (!separatePath(regInfo.uPath, regInfo.uSubKey))
						{
							DbgPrint("%s->Invalid RegistryPath->\"%s\"\n", _ZwOpReg_H, regInfo.uPath);//�쳣�ָ�·��
							break;
						}
						RtlInitAnsiString(&NTPath, regInfo.uPath);
						RtlInitAnsiString(&NTSubKey, regInfo.uSubKey);
						RtlAnsiStringToUnicodeString(&uPath, &NTPath, TRUE);
						RtlAnsiStringToUnicodeString(&uSubKey, &NTSubKey, TRUE);
					case 'I':
					case 'i':
						ntIBinaryDeleteKey(uPath, uSubKey, TRUE);
						break;
					default:
						DbgPrint("%s->Invalid SubKeyType->\'%c\'\n", _ZwOpReg_H, regInfo.uSubKeyType);//��Ч������
						break;
					}
					break;
				case 'S':
				case 's'://ִ�� set
					switch (regInfo.uSubKeyType)
					{
					case 'E':
					case 'e':
						RtlInitUnicodeString(&uSubKey, L"");//Ĭ�ϼ����������볣���һ��
					case 'K':
					case 'k':
						switch (regInfo.uValueType)
						{
						case 'S':
						case 's':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_SZ, uValue.Buffer, uValue.Length);//һ��Ҫ�� wchar_t
							break;
						case 'B':
						case 'b':
							if (!NT_SUCCESS(RtlUnicodeStringToInteger(&uValue, 0, &ul)))
							{
								DbgPrint("%s->Invalid Binary->\"%s\"", _ZwOpReg_H, regInfo.uValue);
								break;
							}
							ntIBinarySetKeyValue(uPath, uSubKey, REG_BINARY, regInfo.uValue, sizeof(ULONG));
							break;
						case 'D':
						case 'd':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_DWORD, regInfo.uValue, (ULONG)strlen(regInfo.uValue));
							break;
						case 'Q':
						case 'q':
							if (!NT_SUCCESS(RtlUnicodeStringToInt64(&uValue, 0, &ull, &endptr)))
							{
								DbgPrint("%s->Invalid UnsignedLongLong->\"%s\"", _ZwOpReg_H, regInfo.uValue);
								break;
							}
							ntIBinarySetKeyValue(uPath, uSubKey, REG_QWORD, &ull, sizeof(ULONG64));
							break;
						case 'M':
						case 'm':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_MULTI_SZ, uValue.Buffer, uValue.Length);//һ��Ҫ�� wchar_t
							break;
						case 'E':
						case 'e':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_EXPAND_SZ, uValue.Buffer, uValue.Length);//һ��Ҫ�� wchar_t
							break;
						case 'N':
						case 'n':
							ntIBinarySetKeyValue(uPath, uSubKey, REG_NONE, regInfo.uValue, (ULONG)strlen(regInfo.uValue));//һ��Ҫ�� char
							break;
						default:
							DbgPrint("%s->Invalid ValueType->\'%c\'\n", _ZwOpReg_H, regInfo.uValueType);//��Чֵ����
							break;
						}
						break;
					default:
						DbgPrint("%s->Invalid SubKeyType->\'%c\'\n", _ZwOpReg_H, regInfo.uSubKeyType);//��Ч������
						break;
					}
					break;
				case 'Q':
				case 'q'://ִ�� query
					switch (regInfo.uSubKeyType)
					{
					case 'I':
					case 'i':
						while ('\\' == regInfo.uSubKey[strlen(regInfo.uSubKey) - 1])//�����β�ķ�б��
							regInfo.uSubKey[strlen(regInfo.uSubKey) - 1] = 0;
						while ('\\' == regInfo.uSubKey[0])//�����ͷ�ķ�б��
						{
							char uTmp[MAX_PATH] = { 0 };
							strcpy_s(uTmp, MAX_PATH, regInfo.uSubKey);
							strcpy_s(regInfo.uSubKey, MAX_PATH, &uTmp[1]);
						}
						strcat_s(regInfo.uPath, MAX_PATH, "\\");
						strcat_s(regInfo.uPath, MAX_PATH, regInfo.uSubKey);
						RtlInitAnsiString(&NTPath, regInfo.uPath);
						RtlAnsiStringToUnicodeString(&uPath, &NTPath, TRUE);
					case 'F':
					case 'f':
						ntIBinaryQueryKey(uPath);
						break;
					default:
						DbgPrint("%s->Invalid SubKeyType->\'%c\'\n", _ZwOpReg_H, regInfo.uSubKeyType);//��Ч������
						break;
					}
					break;
				default:
					DbgPrint("%s->Invalid Operation->\"%c\"\n", _ZwOpReg_H, regInfo.uOpType);//��Ч����ָ��
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
	{
		DbgPrint("%s->IoCreateDevice()->Failed(%d)! \n", _ZwOpReg_H, status);
		return status;
	}

	/* �������ӷ��Ų����豸�� */
	IoDeleteSymbolicLink(&cdo_syb);//Ϊ��ֹ��ͻ��Ԥ��ɾ��
	status = IoCreateSymbolicLink(&cdo_syb, &cdo_name);//�����ӷ������豸��
	if (!NT_SUCCESS(status))
	{
		DbgPrint("%s->IoCreateSymbolicLink()->Failed(%d)! \n", _ZwOpReg_H, status);
		IoDeleteDevice(g_cdo);//ɾ���豸
		return status;
	}

	/* �����еķַ���������Ϊ�Զ���� */
	for (ULONG i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
		pDriver->MajorFunction[i] = cwkDispatch;
	DbgPrint("%s->listenerEntry()->Successful! \n", _ZwOpReg_H);

	/* ��������豸�ĳ�ʼ����� */
	g_cdo->Flags &= ~DO_DEVICE_INITIALIZING;
	return STATUS_SUCCESS;
}