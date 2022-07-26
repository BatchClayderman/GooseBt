#include <ntifs.h>
#include <ntddk.h>
#include <ntstrsafe.h>
#include "Listener.h"
#ifndef _ZwDeleteFile_H
#define _ZwDeleteFile_H "ZwDeleteFile"
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#endif//_ZwDeleteFile_H


/** �������ļ��������� **/
/* Ŀ¼���ļ��Ĵ��� */
NTSTATUS IBinaryNtCreateFile(UNICODE_STRING ustr)//�����ļ�
{
	/*
	#define InitializeObjectAttributes(p, n, a, r, s)     \
	{                                                     \
		(p)->Length = sizeof( OBJECT_ATTRIBUTES );        \
		(p)->RootDirectory = r;                           \
		(p)->Attributes = a;                              \
		(p)->ObjectName = n;                              \
		(p)->SecurityDescriptor = s;                      \
		(p)->SecurityQualityOfService = NULL;             \
	}
	*/
	NTSTATUS NtStatus = 0;
	HANDLE hFile;
	IO_STATUS_BLOCK io_Status = { 0 };
	OBJECT_ATTRIBUTES ObjAttus = { 0 };
	InitializeObjectAttributes(&ObjAttus,//��ʼ�� ObjAttus �ṹ
		&ustr,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		NULL);
	NtStatus = ZwCreateFile(&hFile,
		GENERIC_WRITE,
		&ObjAttus,
		&io_Status,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_ALERT | FILE_NON_DIRECTORY_FILE,
		NULL,
		0);
	if (NT_SUCCESS(NtStatus))
		ZwClose(hFile);
	return NtStatus;
}

NTSTATUS IBinaryNtCreateDirectory(UNICODE_STRING uPathName)//����Ŀ¼
{
	wchar_t tmp[MAX_PATH] = { 0 };
	wcscpy_s(tmp, MAX_PATH, uPathName.Buffer);
	if (uPathName.Length > 0 && tmp[uPathName.Length - 1] != '\\')//������һ���ַ�����'\\'
	{
		wcscat_s(tmp, MAX_PATH, L"\\");
		uPathName.Buffer = tmp;
		++uPathName.Length;
	}
	NTSTATUS ntStatus;
	HANDLE hFile;
	OBJECT_ATTRIBUTES objAttus = { 0 };
	IO_STATUS_BLOCK ioStatus = { 0 };
	InitializeObjectAttributes(&objAttus,//��ʼ���ļ����Խṹ��
		&uPathName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		NULL);
	ntStatus = ZwCreateFile(&hFile,
		GENERIC_READ | GENERIC_WRITE,
		&objAttus,
		&ioStatus,
		NULL,
		FILE_ATTRIBUTE_DIRECTORY,//�������ô����ļ�
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN_IF,
		FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,//��ʾ��������Ŀ¼������ͬ��ִ��
		NULL,
		0);
	if (NT_SUCCESS(ntStatus))
		ZwClose(hFile);
	return ntStatus;
}

/* �ļ���д */
ULONG64 GetFileSize(HANDLE hfile)//��ȡ�ļ���С
{
	IO_STATUS_BLOCK iostatus = { 0 };
	NTSTATUS ntStatus = 0;
	FILE_STANDARD_INFORMATION fsi = { 0 };
	ntStatus = ZwQueryInformationFile(hfile,
		&iostatus,
		&fsi,
		sizeof(FILE_STANDARD_INFORMATION),
		FileStandardInformation);
	if (!NT_SUCCESS(ntStatus))
		return 0;
	return fsi.EndOfFile.QuadPart;
}

NTSTATUS IBinaryNtReadFile(PVOID pszBuffer, UNICODE_STRING uPathName)//��ȡ�ļ�
{
	OBJECT_ATTRIBUTES objAttri = { 0 };
	NTSTATUS ntStaus;
	HANDLE hFile;
	IO_STATUS_BLOCK ioStatus = { 0 };
	PVOID pReadBuffer = NULL;
	if (NULL == pszBuffer)
		return STATUS_INTEGER_DIVIDE_BY_ZERO;
	InitializeObjectAttributes(&objAttri,//���ļ���ȡ�ļ�
		&uPathName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		0);
	ntStaus = ZwCreateFile(&hFile,
		GENERIC_READ | GENERIC_WRITE,
		&objAttri,
		&ioStatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0);
	if (!NT_SUCCESS(ntStaus))
	{
		ZwClose(hFile);
		if (NULL != pReadBuffer)
			ExFreePoolWithTag(pReadBuffer, (ULONG)"niBI");
		return STATUS_INTEGER_DIVIDE_BY_ZERO;
	}
	pReadBuffer = ExAllocatePoolWithTag(PagedPool, 100, (ULONG)"niBI");//��ȡ�ļ�
	if (NULL == pReadBuffer)
		return STATUS_INTEGER_DIVIDE_BY_ZERO;
	ntStaus = ZwReadFile(hFile, NULL, NULL, NULL, &ioStatus, pReadBuffer, 100, NULL, NULL);
	if (!NT_SUCCESS(ntStaus))
	{
		ZwClose(hFile);
		if (NULL != pReadBuffer)
			ExFreePoolWithTag(pReadBuffer, (ULONG)"niBI");
		return STATUS_INTEGER_DIVIDE_BY_ZERO;
	}
	RtlCopyMemory(pszBuffer, pReadBuffer, 100);//����ȡ�����ݿ���������Ļ�����.
	ZwClose(hFile);//�ǵùر��ļ�
	if (NULL != pReadBuffer)
		ExFreePoolWithTag(pReadBuffer, (ULONG)"niBI");
	return ntStaus;
}

NTSTATUS IBinaryNtWriteFile(UNICODE_STRING uPathName)//д�ļ�
{
	OBJECT_ATTRIBUTES objAttri = { 0 };
	NTSTATUS ntStatus;
	HANDLE hFile;
	IO_STATUS_BLOCK ioStatus = { 0 };
	PVOID pWriteBuffer = NULL;
	KdBreakPoint();
	InitializeObjectAttributes(&objAttri,
		&uPathName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		0);
	ntStatus = ZwCreateFile(&hFile,
		GENERIC_WRITE | GENERIC_WRITE,
		&objAttri,
		&ioStatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_OPEN,//ע��˱�־�������ļ��ļ���������ʧ��
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;
	pWriteBuffer = ExAllocatePoolWithTag(PagedPool, 0x20, (ULONG)"niBI");//��ʼд�ļ�
	if (pWriteBuffer == NULL)
	{
		DbgPrint(_ZwDeleteFile_H);
		DbgPrint("->IBinaryNtWriteFile(%wZ)->Memory Error!\n", uPathName);
		ZwClose(hFile);
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	RtlZeroMemory(pWriteBuffer, 0x20);
	RtlCopyMemory(pWriteBuffer, L"HelloIBinary", wcslen(L"HelloIBinary"));
	ntStatus = ZwWriteFile(hFile,
		NULL,
		NULL,
		NULL,
		&ioStatus,
		pWriteBuffer,
		0x20,
		NULL,
		NULL);
	if (!NT_SUCCESS(ntStatus))
	{
		ZwClose(hFile);
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	ZwClose(hFile);
	ExFreePoolWithTag(pWriteBuffer, (ULONG)"niBI");
	return ntStatus;
}

/* �ļ�ɾ�� */
NTSTATUS IBinaryNtZwDeleteFile(UNICODE_STRING uDeletePathName)//ֱ���������ļ�ɾ��
{
	OBJECT_ATTRIBUTES obAttri = { 0 };
	InitializeObjectAttributes(&obAttri,//��ʼ��Դ�ļ�·�����Ҵ�
		&uDeletePathName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		NULL
	);
	NTSTATUS bRet = ZwDeleteFile(&obAttri);
	DbgPrint(_ZwDeleteFile_H);
	DbgPrint((NT_SUCCESS(bRet) ? "->IBinaryNtZwDeleteFile(%wZ)->Successful!\n" : "->IBinaryNtZwDeleteFile(%wZ)->Failed!\n"), &uDeletePathName);
	return bRet;
}

NTSTATUS IBinaryNtSetInformationFileDeleteFile(UNICODE_STRING uDeletePathName)//��д�ļ���Ϣ��ִ������ɾ��
{
	/** ɾ���ļ��ĵڶ��ַ�ʽ
	  * ˼·��
	  * ��1����ʼ���ļ�·��
	  * ��2���ڹ���ģʽ��ʹ�ö�д��ʽ���ļ�
	  * ��3������Ƿ����⵽�ܾ�������һ�ַ�ʽ���ļ������ø��ļ�����Ϣ
	  * ��4�����óɹ�֮��Ϳ���ִ��������ɾ��������
	  */
	
	OBJECT_ATTRIBUTES objAttri;
	NTSTATUS ntStatus = STATUS_FAILED_STACK_SWITCH;
	HANDLE hFile;
	IO_STATUS_BLOCK ioStatus;
	FILE_DISPOSITION_INFORMATION IBdelPostion = { 0 };//ͨ�� ZwSetInformationFile ɾ����Ҫʹ������ṹ��
	__try
	{
		InitializeObjectAttributes(&objAttri,
			&uDeletePathName,
			OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
			NULL,
			NULL
		);
		ntStatus = ZwCreateFile(&hFile,
			DELETE | FILE_WRITE_DATA | SYNCHRONIZE,//�ļ�Ȩ�ޣ�ɾ��Ȩ�޺�дȨ��
			&objAttri,
			&ioStatus,
			NULL,
			FILE_ATTRIBUTE_NORMAL,//�ļ���������Ĭ��
			FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,//�ļ��Ĺ���ģʽ��ɾ���Ͷ�д
			FILE_OPEN,//�ļ��Ĵ򿪷�ʽ�ǡ��򿪡�������������򷵻�ʧ��
			FILE_SYNCHRONOUS_IO_NONALERT | FILE_DELETE_ON_CLOSE,//�ļ���Ӧ��ѡ������ FILE_DELETE_ON_CLOSE ��ʹ�� ZwClose �ر��ļ������ʱ��ɾ������ļ�
			NULL,
			0
		);
		if (!NT_SUCCESS(ntStatus))
		{
			if (STATUS_ACCESS_DENIED == ntStatus)//������ɹ����ж��ļ��Ƿ�ܾ����ʣ��ǵĻ����Ǿ�����Ϊ���Է��ʲ�����ɾ��
			{
				ntStatus = ZwCreateFile(&hFile,
					SYNCHRONIZE | FILE_READ_ATTRIBUTES | FILE_WRITE_ATTRIBUTES,//ɾ��Ȩ��ʧ�ܾ��Զ�дģʽ
					&objAttri,
					&ioStatus,
					NULL,
					FILE_ATTRIBUTE_NORMAL,//�ļ�������ΪĬ��
					FILE_SHARE_DELETE | FILE_SHARE_WRITE | FILE_SHARE_READ,//�ļ��Ĺ�������Ϊ��дɾ��
					FILE_OPEN,//�ļ��Ĵ򿪷�ʽΪ���򿪡�����������ʧ��
					FILE_SYNCHRONOUS_IO_NONALERT,//�ļ���Ӧ��ѡ��
					NULL,
					0
				);
				if (NT_SUCCESS(ntStatus))//����򿪳ɹ�������������ļ�����Ϣ
				{
					FILE_BASIC_INFORMATION IBFileBasic = { 0 };
					ntStatus = ZwQueryInformationFile(//ʹ�� ZwQueryInformationfile �����ļ��Ļ�����Ϣ
						hFile,
						&ioStatus,
						&IBFileBasic,
						sizeof(IBFileBasic),
						FileBasicInformation
					);
					if (!NT_SUCCESS(ntStatus))//�����ļ���Ϣʧ��
					{
						DbgPrint(_ZwDeleteFile_H);
						DbgPrint("->IBinaryNtSetInformationFileDeleteFile(%wZ)->GetFileBasicInformation Failed!\n", &uDeletePathName);
					}
					IBFileBasic.FileAttributes = FILE_ATTRIBUTE_NORMAL; //��������ΪĬ������
					ntStatus = ZwSetInformationFile(
						hFile,
						&ioStatus,
						&IBFileBasic,
						sizeof(IBFileBasic),
						FileBasicInformation);//���ҵ� FileBasic �����������õ�����ļ���
					if (!NT_SUCCESS(ntStatus))
					{
						DbgPrint(_ZwDeleteFile_H);
						DbgPrint("->IBinaryNtSetInformationFileDeleteFile(%wZ)->SetFileBasicInformation Failed!\n", &uDeletePathName);
					}
					
					ZwClose(hFile);//����ɹ��ر��ļ����
					ntStatus = ZwCreateFile(&hFile,//���´����������Ϣ����ļ�.
						SYNCHRONIZE | FILE_WRITE_DATA | DELETE,
						&objAttri,
						&ioStatus,
						NULL,
						FILE_ATTRIBUTE_NORMAL,
						FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE,
						FILE_OPEN,
						FILE_SYNCHRONOUS_IO_NONALERT | FILE_DELETE_ON_CLOSE,
						NULL,
						0);
				}
				if (!NT_SUCCESS(ntStatus))
				{
					DbgPrint(_ZwDeleteFile_H);
					DbgPrint("->IBinaryNtSetInformationFileDeleteFile(%wZ)->Error Opening File!\n", &uDeletePathName);
				}
			}
		}
		
		/* ͨ�� ZwSetInformationFile ����ǿ��ɾ���ļ� */
		IBdelPostion.DeleteFile = TRUE;//�˱�־����Ϊ TRUE ����ɾ��
		ntStatus = ZwSetInformationFile(hFile, &ioStatus, &IBdelPostion, sizeof(IBdelPostion), FileDispositionInformation);
		if (!NT_SUCCESS(ntStatus))
		{
			ZwClose(hFile);
			DbgPrint(_ZwDeleteFile_H);
			DbgPrint("->IBinaryNtSetInformationFileDeleteFile(%wZ)->SetFileInformation Failed!\n", &uDeletePathName);
			return ntStatus;
		}
		ZwClose(hFile);
	}
	__except (1)
	{
		DbgPrint(_ZwDeleteFile_H);
		DbgPrint("->IBinaryNtSetInformationFileDeleteFile(%wZ)->Unspecitied Error!\n", &uDeletePathName);
	}
	
	return ntStatus;
}



/** ��������ں��� **/
/* ����ֹͣ */
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	listenerUnload();
	DbgPrint("\n");
	DbgPrint(_ZwDeleteFile_H);
	DbgPrint("->DriverUnload()\n");
	return;
}

/* �������� */
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgPrint("\n");
	DbgPrint(_ZwDeleteFile_H);
	DbgPrint("->DriverEntry()\n");
	NTSTATUS bRet = listenerEntry(pDriver);
	if (!NT_SUCCESS(bRet))
		return bRet;
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}