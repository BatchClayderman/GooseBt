#include <ntddk.h>
#include <ntstrsafe.h>
#ifndef _ZwOpReg_H
#define _ZwOpReg_H "ZwOpReg"
#endif

//************************************
// Method:    ntIBinaryCreateKey
// FullName:  ntIBinaryCreateKey
// Access:    public 
// Returns:   NTSTATUS
// Qualifier: ����ע����ֵ
// Parameter: UNICODE_STRING uPathKeyName
//************************************


/** ע��� **/
/* ���� */
NTSTATUS ntIBinaryCreateKey(UNICODE_STRING uPathKeyName)
{
	NTSTATUS status = STATUS_ERROR_PROCESS_NOT_IN_JOB;
	OBJECT_ATTRIBUTES objAttri;
	HANDLE hKeyHandle;
	UNICODE_STRING uSubKey;
	HANDLE hSubKey;
	OBJECT_ATTRIBUTES objSubAttri;
	ULONG isRegStatus;//ע����״̬��������
	InitializeObjectAttributes(
		&objAttri,
		&uPathKeyName,
		OBJ_CASE_INSENSITIVE,//���ֻ���ں˷��ʶ���ֻ��һ����
		NULL,
		NULL
	);
	status = ZwCreateKey(&hKeyHandle,
		KEY_ALL_ACCESS,
		&objAttri,
		0,
		NULL,
		REG_OPTION_BACKUP_RESTORE,
		(PULONG)(&isRegStatus)
	);
	if (!NT_SUCCESS(status))
	{
		ZwClose(hKeyHandle);
		return status;
	}
	RtlUnicodeStringInit(&uSubKey, L"MyReg");//������Key
	/*
	InitializeObjectAttributes(p, n, a, r, s)        \
	{                                                \
		(p)->Length = sizeof(OBJECT_ATTRIBUTES);     \
		(p)->RootDirectory = r;                      \
		(p)->Attributes = a;                         \
		(p)->ObjectName = n;                         \
		(p)->SecurityDescriptor = s;                 \
		(p)->SecurityQualityOfService = NULL;        \
	}
	*/
	//InitializeObjectAttributes(&objAttri, &uSubKey, OBJ_CASE_INSENSITIVE, hKeyHandle, NULL);
	/* ��ʹ�ú���ֹ����и�ֵ */
	objSubAttri.Length = sizeof(OBJECT_ATTRIBUTES);
	objSubAttri.Attributes = OBJ_CASE_INSENSITIVE;
	objSubAttri.ObjectName = &uSubKey;
	objSubAttri.SecurityDescriptor = NULL;
	objSubAttri.SecurityQualityOfService = NULL;
	objSubAttri.RootDirectory = hKeyHandle;//ע�����︸Ŀ¼����Ϊ�������洴���� key
	status = ZwCreateKey(&hSubKey,//���������� Key
		KEY_ALL_ACCESS,           //Ȩ��
		&objSubAttri,             //·��
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,  //������ Key �����Ƿ���ڻ�����ʱ��
		&isRegStatus              //���� Key ��״̬���������ɹ����Ǵ�
	);
	if (!NT_SUCCESS(status))
	{
		ZwClose(hSubKey);
		ZwClose(hKeyHandle);
		return status;
	}
	ZwClose(hSubKey);
	ZwClose(hKeyHandle);
	DbgPrint(_ZwOpReg_H);
	KdPrint(("->Create Key Successfully!\n"));
	return status;
}

NTSTATUS ntIBinaryInit()
{
	NTSTATUS status = STATUS_ERROR_PROCESS_NOT_IN_JOB;
	UNICODE_STRING uKeyPath;
	RtlUnicodeStringInit(&uKeyPath, L"\\registry\\machine\\SoftWare\\IBinary");
	status = ntIBinaryCreateKey(uKeyPath);
	if (!NT_SUCCESS(status))
	{
		DbgPrint(_ZwOpReg_H);
		KdPrint(("->Create Key Failed!\n"));
	}
	return status;
}

/* ɾ�� */
NTSTATUS ntIBinaryDeleteKey(UNICODE_STRING uPathKeyName)
{
	NTSTATUS ntStatus = STATUS_FAILED_STACK_SWITCH;
	HANDLE hKey;
	OBJECT_ATTRIBUTES ObjAttr;
	ObjAttr.Length = sizeof(OBJECT_ATTRIBUTES);
	ObjAttr.Attributes = OBJ_CASE_INSENSITIVE;
	ObjAttr.ObjectName = &uPathKeyName;
	ObjAttr.RootDirectory = NULL;
	ObjAttr.SecurityDescriptor = NULL;
	ObjAttr.SecurityQualityOfService = NULL;
	__try
	{
		ntStatus = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &ObjAttr);//��Key�ڽ���ɾ��
		if (!NT_SUCCESS(ntStatus))
		{
			ZwClose(hKey);
			return ntStatus;
		}
		ntStatus = ZwDeleteKey(hKey);
		if (!NT_SUCCESS(ntStatus))
		{
			ZwClose(hKey);
			return ntStatus;
		}
		DbgPrint(_ZwOpReg_H);
		KdPrint(("->Delete Key Successfully!\n"));
	}
	__except (GetExceptionCode())
	{
		DbgPrint(_ZwOpReg_H);
		KdPrint(("->Delete Key Successfully!\n"));
	}
	return ntStatus;
}

/* ��ѯ */
NTSTATUS ntIBinaryQueryKey(UNICODE_STRING uPathKeyName)
{
	NTSTATUS ntStatus = STATUS_FAILED_STACK_SWITCH;
	HANDLE hKey;
	OBJECT_ATTRIBUTES objAttri = { 0 };
	PKEY_FULL_INFORMATION pkfinfo = NULL;
	ULONG uSize = 0;
	ULONG iteratorValue = 0; //�����ı���
	PKEY_BASIC_INFORMATION pBaseinfo = NULL;
	UNICODE_STRING uDbgValue = { 0 };//������������Ϣ���浽 UNICODE_STRING �ṹ����
	__try//���ȴ� Key Ȼ����� Key
	{
		InitializeObjectAttributes(
			&objAttri,
			&uPathKeyName,
			OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
			NULL,
			NULL);
		ntStatus = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &objAttri);
		if (!NT_SUCCESS(ntStatus))
			return ntStatus;
		//���� Key ��Ҫ���ε��ã���һ�ε��õó����ݴ�С���ڶ��ε��������������
		ntStatus = ZwQueryKey(hKey, KeyFullInformation, NULL, 0, &uSize);
		//�ó� KEY_FUN_INFOMATION �ṹ�Ĵ�С�������ڴ����뼴��
		//��ѯ MSDN �ó���ZwQuery �����ݲ���᷵������״̬�������ж�һ�¼���
		//STATUS_BUFFER_OVERFLOW �� STATUS_BUFFER_TOO_SMALL
		if (ntStatus != STATUS_BUFFER_OVERFLOW && ntStatus != STATUS_BUFFER_TOO_SMALL)
		{
			ZwClose(hKey);
			return ntStatus;
		}
		pkfinfo = (PKEY_FULL_INFORMATION)ExAllocatePool2(PagedPool, uSize, 'niBI');
		if (NULL == pkfinfo)
			pkfinfo = (PKEY_FULL_INFORMATION)ExAllocatePoolZero(PagedPool, uSize, 'niBI');
		if (NULL == pkfinfo)
		{
			ZwClose(hKey);
			return ntStatus;
		}
		//������ KEY_FULL_INFOMATION �ṹ�����С��Ȼ����л�ȡ��С
		ntStatus = ZwQueryKey(hKey, KeyFullInformation, pkfinfo, uSize, &uSize);
		if (!NT_SUCCESS(ntStatus))
		{
			ExFreePoolWithTag(pkfinfo, 'niBI');
			ZwClose(hKey);
			return ntStatus;
		}
		for (iteratorValue = 0; iteratorValue < pkfinfo->SubKeys; iteratorValue++)
		{
			//������ Key ��Ҫ����ö�ٳ� Key ����ϸ��Ϣ��ʹ�� ZwEnumerateKey ���ɣ�Ҳ��ö��һ���ṹ
			ntStatus = ZwEnumerateKey(hKey,
				0,
				KeyBasicInformation,
				NULL,
				0,
				&uSize
			);
			if (ntStatus != STATUS_BUFFER_OVERFLOW && ntStatus != STATUS_BUFFER_TOO_SMALL)
			{
				ZwClose(hKey);
				return ntStatus;
			}
			pBaseinfo = (PKEY_BASIC_INFORMATION)ExAllocatePool2(PagedPool, uSize, 'niBI');
			if (NULL == pBaseinfo)
				pBaseinfo = (PKEY_BASIC_INFORMATION)ExAllocatePoolZero(PagedPool, uSize, 'niBI');
			if (NULL == pkfinfo)
			{
				ZwClose(hKey);
				return ntStatus;
			}
			ntStatus = ZwEnumerateKey(hKey,//��������һ�εó���Ҫ��
				0,
				KeyBasicInformation,
				pBaseinfo,
				uSize,
				&uSize
			);
			if (!NT_SUCCESS(ntStatus))
			{
				if (NULL != pBaseinfo)
					ExFreePoolWithTag(pBaseinfo, 'niBI');
				if (NULL != pkfinfo)
					ExFreePoolWithTag(pkfinfo, 'niBI');
				ZwClose(hKey);
				return ntStatus;
			}
			//�ó���Ϣ����Խ��н�һ�������ˣ���ʼ�� UNICODE �ṹ���д�ӡ�������
			if (!pBaseinfo)
				return ntStatus;
			uDbgValue.Length = (USHORT)pBaseinfo->NameLength;
			uDbgValue.MaximumLength = (USHORT)pBaseinfo->NameLength;
			uDbgValue.Buffer = pBaseinfo->Name;
			DbgPrint(_ZwOpReg_H);
			KdPrint(("->Key = %wZ\n", &uDbgValue));
			ExFreePool(pBaseinfo); //ͬ���ͷ��ڴ�
		}
		if (NULL != pkfinfo)//�ͷ���Դ
			ExFreePool(pkfinfo);
		ZwClose(hKey);
	}
	__except (GetExceptionCode())
	{
		DbgPrint(_ZwOpReg_H);
		KdPrint(("->ntIBinaryQueryKey()->Error:%ld\n", GetExceptionCode()));
	}
	return ntStatus;
}

/* �޸� */
NTSTATUS ntIBinarySetKeyValue(UNICODE_STRING uPathKeyName)
{
	NTSTATUS ntStatus;
	OBJECT_ATTRIBUTES objAttri;
	HANDLE hKey;
	UNICODE_STRING uSetValueKeyName;
	ULONG Value = 10;
	InitializeObjectAttributes(&objAttri,
		&uPathKeyName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		NULL
	);
	ntStatus = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &objAttri);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;
	RtlUnicodeStringInit(&uSetValueKeyName, L"IBinaryFrist");//���� KEY value ��ֵ
	ntStatus = ZwSetValueKey(hKey,
		&uSetValueKeyName,
		0,
		REG_DWORD,
		&Value,
		sizeof(ULONG));
	if (!NT_SUCCESS(ntStatus))
	{
		ZwClose(hKey);
		return ntStatus;
	}
	DbgPrint(_ZwOpReg_H);
	KdPrint(("Set Key Successfully!\n"));
	ZwClose(hKey);
	return ntStatus;
}


/** ��������ں��� **/
/* ����ֹͣ */
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	DbgPrint("\n");
	DbgPrint(_ZwOpReg_H);
	DbgPrint("->DriverUnload()\n");
	return;
}

/* �������� */
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgPrint("\n");
	DbgPrint(_ZwOpReg_H);
	DbgPrint("->DriverEntry()\n");
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}