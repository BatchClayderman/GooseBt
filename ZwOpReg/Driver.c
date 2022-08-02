#include <ntddk.h>
#include <ntstrsafe.h>
#include "Listener.h"
#ifndef _ZwOpReg_H
#define _ZwOpReg_H "ZwOpReg"
#define GooseBtKeyPath L"\\registry\\machine\\SoftWare\\GooseBt"
#define ZwOpRegSubKey L"ZwOpReg"
#endif


/**/
/*
 * uPath��ע���·��
 * uSubKey������·���µ�������Ӽ�
 * uType������
 * uValue��ֵ
 * uSize��ֵ���ʹ�С
 * isSubFolder��ָ���Ƿ�Ϊ����
 */


/** ע��� **/
/* ���� */
NTSTATUS ntIBinaryCreateKey(UNICODE_STRING uPath, UNICODE_STRING uSubKey)
{
	OBJECT_ATTRIBUTES attrPath, attrSubKey;
	HANDLE hPath, hSubKey;
	ULONG isRegStatus;//ע����״̬��������
	InitializeObjectAttributes(
		&attrPath, 
		&uPath, 
		OBJ_CASE_INSENSITIVE,//���ֻ���ں˷��ʶ���ֻ��һ����
		NULL, 
		NULL
	);
	NTSTATUS ntStatus = ZwCreateKey(&hPath,
		KEY_CREATE_SUB_KEY, 
		&attrPath, 
		0, 
		NULL, 
		REG_OPTION_BACKUP_RESTORE, 
		(PULONG)(&isRegStatus)
	);
	if (!NT_SUCCESS(ntStatus))
	{
		DbgPrint("%s->Create->uPath = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, ntStatus);
		ZwClose(hPath);
		return ntStatus;
	}

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
	InitializeObjectAttributes(&objAttri, &uSubKey, OBJ_CASE_INSENSITIVE, hKeyHandle, NULL);
	*/

	/* ��ʹ�ú���ֹ����и�ֵ */
	attrSubKey.Length = sizeof(OBJECT_ATTRIBUTES);
	attrSubKey.Attributes = OBJ_CASE_INSENSITIVE;
	attrSubKey.ObjectName = &uSubKey;
	attrSubKey.SecurityDescriptor = NULL;
	attrSubKey.SecurityQualityOfService = NULL;
	attrSubKey.RootDirectory = hPath;//ע�����︸Ŀ¼���� uPath �ľ��
	ntStatus = ZwCreateKey(&hSubKey, //���������� Key
		KEY_CREATE_SUB_KEY,        //Ȩ�ޣ���СȨ��ԭ��
		&attrSubKey,               //·��
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,   //������ Key �����Ƿ���ڻ�����ʱ��
		&isRegStatus               //���� Key ��״̬���������ɹ����Ǵ�
	);
	ZwClose(hSubKey);
	ZwClose(hPath);
	if (NT_SUCCESS(ntStatus))
		DbgPrint("%s->Create->uPath = \"%wZ\"->uSubKey = \"%wZ\"->Successful\n", _ZwOpReg_H, &uPath, &uSubKey);
	else
		DbgPrint("%s->Create->uPath = \"%wZ\"->uSubKey = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, &uSubKey, ntStatus);
	return ntStatus;
}

/* ɾ�� */
NTSTATUS ntIBinaryDeleteKey(UNICODE_STRING uPath, UNICODE_STRING uSubKey, BOOLEAN isSubFolder)
{
	NTSTATUS ntStatus = STATUS_FAILED_STACK_SWITCH;
	HANDLE hPath;
	OBJECT_ATTRIBUTES attrPath;
	attrPath.Length = sizeof(OBJECT_ATTRIBUTES);
	attrPath.Attributes = OBJ_CASE_INSENSITIVE;
	attrPath.ObjectName = &uPath;
	attrPath.RootDirectory = NULL;
	attrPath.SecurityDescriptor = NULL;
	attrPath.SecurityQualityOfService = NULL;
	__try
	{
		ntStatus = ZwOpenKey(&hPath, KEY_ALL_ACCESS, &attrPath);//�� Key �ٽ���ɾ��
		if (!NT_SUCCESS(ntStatus))
		{
			DbgPrint("%s->Delete->uPath = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, ntStatus);
			ZwClose(hPath);
			return ntStatus;
		}
		ntStatus = isSubFolder ? ZwDeleteKey(hPath) : ZwDeleteValueKey(hPath, &uSubKey);
		ZwClose(hPath);
		if (NT_SUCCESS(ntStatus))
			DbgPrint("%s->Delete->uPath = \"%wZ\"->uSubKey = \"%wZ\"->Successful\n", _ZwOpReg_H, &uPath, &uSubKey);
		else
			DbgPrint("%s->Delete->uPath = \"%wZ\"->uSubKey = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, &uSubKey, ntStatus);
	}
	__except (GetExceptionCode())
	{
		DbgPrint("%s->Delete->uPath = \"%wZ\"->uSubKey = \"%wZ\"->Unexpected(%d)\n", _ZwOpReg_H, &uPath, &uSubKey, GetExceptionCode());
	}
	return ntStatus;
}

/* �޸� */
NTSTATUS ntIBinarySetKeyValue(UNICODE_STRING uPath, UNICODE_STRING uSubKey, ULONG uType, PVOID uValue, ULONG uSize)
{
	OBJECT_ATTRIBUTES attrPath;
	HANDLE hPath;
	InitializeObjectAttributes(&attrPath, 
		&uPath, 
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 
		NULL, 
		NULL
	);
	NTSTATUS ntStatus = ZwOpenKey(&hPath, KEY_SET_VALUE, &attrPath);
	if (!NT_SUCCESS(ntStatus))
	{
		DbgPrint("%s->Set->uPath = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, ntStatus);
		return ntStatus;
	}
	ntStatus = ZwSetValueKey(hPath, 
		&uSubKey, 
		0, 
		uType, 
		uValue, 
		uSize
	);
	ZwClose(hPath);
	if (NT_SUCCESS(ntStatus))
		DbgPrint("%s->Set->uPath = \"%wZ\"->uSubKey = \"%wZ\"->Successful\n", _ZwOpReg_H, &uPath, &uSubKey);
	else
		DbgPrint("%s->Set->uPath = \"%wZ\"->uSubKey = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, &uSubKey, ntStatus);
	return ntStatus;
}

/* ��ѯ */
NTSTATUS ntIBinaryQueryKey(UNICODE_STRING uPath)
{
	NTSTATUS ntStatus = STATUS_FAILED_STACK_SWITCH;
	HANDLE hKey;
	OBJECT_ATTRIBUTES objAttri = { 0 };
	PKEY_FULL_INFORMATION pkfinfo = NULL;
	ULONG uSize = 0;
	PKEY_BASIC_INFORMATION pBaseinfo = NULL;
	UNICODE_STRING uDbgValue = { 0 };//������������Ϣ���浽 UNICODE_STRING �ṹ����
	__try//���ȴ� Key Ȼ����� Key
	{
		InitializeObjectAttributes(
			&objAttri,
			&uPath,
			OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
			NULL,
			NULL
		);
		ntStatus = ZwOpenKey(&hKey, KEY_ENUMERATE_SUB_KEYS, &objAttri);
		if (!NT_SUCCESS(ntStatus))
		{
			DbgPrint("%s->Query->uPath = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, ntStatus);
			return ntStatus;
		}
		//���� Key ��Ҫ���ε��ã���һ�ε��õó����ݴ�С���ڶ��ε��������������
		ntStatus = ZwQueryKey(hKey, KeyFullInformation, NULL, 0, &uSize);
		//�ó� KEY_FUN_INFOMATION �ṹ�Ĵ�С�������ڴ����뼴��
		//��ѯ MSDN �ó���ZwQuery �����ݲ���᷵������״̬�������ж�һ�¼���
		
		/* STATUS_BUFFER_OVERFLOW �� STATUS_BUFFER_TOO_SMALL */
		if (ntStatus != STATUS_BUFFER_OVERFLOW && ntStatus != STATUS_BUFFER_TOO_SMALL)
		{
			ZwClose(hKey);
			DbgPrint("%s->Query->uPath = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, ntStatus);
			return ntStatus;
		}
		pkfinfo = (PKEY_FULL_INFORMATION)ExAllocatePool2(PagedPool, uSize, 'niBI');
		if (NULL == pkfinfo)
			pkfinfo = (PKEY_FULL_INFORMATION)ExAllocatePoolZero(PagedPool, uSize, 'niBI');
		if (NULL == pkfinfo)
		{
			DbgPrint("%s->Query->uPath = \"%wZ\"->Failed(pkfinfo = %d)\n", _ZwOpReg_H, &uPath, STATUS_MEMORY_NOT_ALLOCATED);
			ZwClose(hKey);
			return STATUS_MEMORY_NOT_ALLOCATED;
		}
		
		/* ������ KEY_FULL_INFOMATION �ṹ�����С��Ȼ����л�ȡ��С */
		ntStatus = ZwQueryKey(hKey, KeyFullInformation, pkfinfo, uSize, &uSize);
		if (!NT_SUCCESS(ntStatus))
		{
			ExFreePoolWithTag(pkfinfo, 'niBI');
			ZwClose(hKey);
			return ntStatus;
		}
		for (ULONG iteratorValue = 0; iteratorValue < pkfinfo->SubKeys; ++iteratorValue)
		{
			ntStatus = ZwEnumerateKey(hKey,
				0,
				KeyBasicInformation,
				NULL,
				0,
				&uSize
			);//������ Key ��Ҫ����ö�ٳ� Key ����ϸ��Ϣ��ʹ�� ZwEnumerateKey ���ɣ�Ҳ��ö��һ���ṹ
			if (ntStatus != STATUS_BUFFER_OVERFLOW && ntStatus != STATUS_BUFFER_TOO_SMALL)
			{
				ZwClose(hKey);
				DbgPrint("%s->Query->uPath = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, ntStatus);
				return ntStatus;
			}
			pBaseinfo = (PKEY_BASIC_INFORMATION)ExAllocatePool2(PagedPool, uSize, 'niBI');
			if (NULL == pBaseinfo)
				pBaseinfo = (PKEY_BASIC_INFORMATION)ExAllocatePoolZero(PagedPool, uSize, 'niBI');
			if (NULL == pBaseinfo)
			{
				ZwClose(hKey);
				DbgPrint("%s->Query->uPath = \"%wZ\"->Failed(pBaseinfo = %d)\n", _ZwOpReg_H, &uPath, STATUS_MEMORY_NOT_ALLOCATED);
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
				DbgPrint("%s->Query->uPath = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, ntStatus);
				return ntStatus;
			}

			/* �ó���Ϣ����Խ��н�һ�������ˣ���ʼ�� UNICODE �ṹ���д�ӡ������� */
			if (!pBaseinfo)
			{
				DbgPrint("%s->Query->uPath = \"%wZ\"->Failed(pBaseinfo = %d)\n", _ZwOpReg_H, &uPath, STATUS_MEMORY_NOT_ALLOCATED);
				return ntStatus;
			}
			uDbgValue.Length = (USHORT)pBaseinfo->NameLength;
			uDbgValue.MaximumLength = (USHORT)pBaseinfo->NameLength;
			uDbgValue.Buffer = pBaseinfo->Name;
			DbgPrint("%s->Query->uPath = \"%wZ\"->Key = \"%wZ\"->Successful\n", _ZwOpReg_H, &uPath, &uDbgValue);
			ExFreePool(pBaseinfo);//ͬ���ͷ��ڴ�
		}
		if (NULL != pkfinfo)//�ͷ���Դ
			ExFreePool(pkfinfo);
		ZwClose(hKey);
	}
	__except (GetExceptionCode())
	{
		DbgPrint("%s->Query->uPath = \"%wZ\"->Unexpected(%d)\n", _ZwOpReg_H, &uPath, GetExceptionCode());
	}
	return ntStatus;
}



/** ��������ں��� **/
/* ��ʼ�� */
NTSTATUS ntIBinaryInit()
{
	UNICODE_STRING uPath, uSubKey;
	RtlUnicodeStringInit(&uPath, GooseBtKeyPath);
	RtlUnicodeStringInit(&uSubKey, ZwOpRegSubKey);
	ULONG ul = 3;//������
	NTSTATUS status = ntIBinarySetKeyValue(uPath, uSubKey, REG_DWORD, &ul, sizeof(ULONG));
	if (NT_SUCCESS(status))
		DbgPrint("%s->Initial->%ws->%ws->Successful\n", _ZwOpReg_H, GooseBtKeyPath, ZwOpRegSubKey);
	else
		DbgPrint("%s->Initial->%ws->%ws->Failed(%d)\n", _ZwOpReg_H, GooseBtKeyPath, ZwOpRegSubKey, status);
	return status;
}

/* ����ֹͣ */
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	DbgPrint("\n%s->DriverUnload()\n", _ZwOpReg_H);
	listenerUnload();
	return;
}

/* �������� */
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgPrint("\n%s->DriverEntry()\n", _ZwOpReg_H);
	pDriver->DriverUnload = DriverUnload;
	NTSTATUS bRet = listenerEntry(pDriver);
	if (!NT_SUCCESS(bRet))
	{
		DbgPrint("%s->Initial->Listener->Failed! ", _ZwOpReg_H);
		return bRet;
	}
	UNICODE_STRING uPath, uSubKey;
	RtlInitUnicodeString(&uPath, GooseBtKeyPath);
	RtlInitUnicodeString(&uSubKey, ZwOpRegSubKey);
	return ntIBinaryInit();
}