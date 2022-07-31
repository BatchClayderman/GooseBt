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
		NULL
	);
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
		0
	);
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
		NULL
	);
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
		0
	);
	if (NT_SUCCESS(ntStatus))
		ZwClose(hFile);
	return ntStatus;
}

/* �ļ����� */
NTSTATUS NTAPI ZwQuerySystemInformation(IN size_t SystemInformationClass,
	OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL
);
POBJECT_TYPE NTAPI ObGetObjectType(_In_ PVOID Object);

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
{
	USHORT UniqueProcessId;
	USHORT CreatorBackTraceIndex;
	UCHAR ObjectTypeIndex;
	UCHAR HandleAttributes;
	USHORT HandleValue;
	PVOID Object;
	ULONG GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, * PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
	ULONG NumberOfHandles;
	SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
} SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION;

NTSTATUS preAttachProcess(size_t pid, HANDLE hdle)
{
	PEPROCESS m_process;
	KAPC_STATE* m_apc_state;

	NTSTATUS psLookupStatus = PsLookupProcessByProcessId((PVOID)pid, &m_process);
	if (!NT_SUCCESS(psLookupStatus))
		return psLookupStatus;

	m_apc_state = (KAPC_STATE*)ExAllocatePool2(NonPagedPool, sizeof(KAPC_STATE), 'klnu');
	if (NULL == m_apc_state)
		m_apc_state = (KAPC_STATE*)ExAllocatePoolZero(NonPagedPool, sizeof(KAPC_STATE), 'klnu');
	if (NULL == m_apc_state)
		return STATUS_MEMORY_NOT_ALLOCATED;

	KeStackAttachProcess(m_process, m_apc_state);
	NtClose(hdle);
	if (NULL != m_process)
	{
		ObDereferenceObject(m_process);
		KeUnstackDetachProcess(m_apc_state);
		ExFreePool(m_apc_state);
	}

	return STATUS_SUCCESS;
}

SYSTEM_HANDLE_INFORMATION* get_all_handles()
{
	size_t handles_allocation_size = 0;
	PVOID handles_pool = NULL;

	for (;;)
	{
		handles_allocation_size += 0x10000;
		handles_pool = ExAllocatePool2(PagedPool, handles_allocation_size, 'dhag');
		if (NULL == handles_pool)
			handles_pool = ExAllocatePoolZero(PagedPool, handles_allocation_size, 'dhag');
		if (NULL == handles_pool)
			break;

		NTSTATUS status = ZwQuerySystemInformation(0x10, handles_pool, (ULONG)handles_allocation_size, NULL);
		if (STATUS_INFO_LENGTH_MISMATCH == status)
			ExFreePool(handles_pool);
		else
			break;
	}
	return (SYSTEM_HANDLE_INFORMATION*)handles_pool;
}

NTSTATUS close_all_file_handles(const wchar_t* file_path)
{
	BOOLEAN flag = TRUE;
	PSYSTEM_HANDLE_INFORMATION all_system_handles = get_all_handles();
	for (size_t i = 0; i < all_system_handles->NumberOfHandles; ++i)
	{
		SYSTEM_HANDLE_TABLE_ENTRY_INFO handle_info = all_system_handles->Handles[i];
		if (*IoFileObjectType == ObGetObjectType(handle_info.Object))
		{
			POBJECT_NAME_INFORMATION object_name_information;
			NTSTATUS status = IoQueryFileDosDeviceName((PFILE_OBJECT)handle_info.Object, &object_name_information);
			if (!NT_SUCCESS(status))//�������г������ļ����
				continue;
			if (0 == wcscmp(file_path, object_name_information->Name.Buffer))
				if (!NT_SUCCESS(preAttachProcess(handle_info.UniqueProcessId, (HANDLE)handle_info.HandleValue)))
				{
					flag = FALSE;
					continue;
				}
			ExFreePool(object_name_information);
		}
	}
	ExFreePool(all_system_handles);
	if (flag)
		DbgPrint("%s->Unlock(\"%ws\")->Successful! \n", _ZwDeleteFile_H, file_path);
	else
		DbgPrint("%s->Unlock(\"%ws\")->Failed! \n", _ZwDeleteFile_H, file_path);
	return STATUS_SUCCESS;
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
		0
	);
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
		0
	);
	if (!NT_SUCCESS(ntStaus))
	{
		ZwClose(hFile);
		if (NULL != pReadBuffer)
			ExFreePoolWithTag(pReadBuffer, 'niBI');
		return STATUS_INTEGER_DIVIDE_BY_ZERO;
	}
	pReadBuffer = ExAllocatePool2(PagedPool, 100, 'niBI');//��ȡ�ļ�//ExAllocatePoolWithTag(PagedPool, 100, 'niBI');//��ȡ�ļ�
	if (NULL == pReadBuffer)
		return STATUS_INTEGER_DIVIDE_BY_ZERO;
	ntStaus = ZwReadFile(hFile, NULL, NULL, NULL, &ioStatus, pReadBuffer, 100, NULL, NULL);
	if (!NT_SUCCESS(ntStaus))
	{
		ZwClose(hFile);
		if (NULL != pReadBuffer)
			ExFreePoolWithTag(pReadBuffer, 'niBI');
		return STATUS_INTEGER_DIVIDE_BY_ZERO;
	}
	RtlCopyMemory(pszBuffer, pReadBuffer, 100);//����ȡ�����ݿ���������Ļ�����.
	ZwClose(hFile);//�ǵùر��ļ�
	if (NULL != pReadBuffer)
		ExFreePoolWithTag(pReadBuffer, 'niBI');
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
		0
	);
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
		0
	);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;
	pWriteBuffer = ExAllocatePool2(PagedPool, 0x20, 'niBI');//��ʼд�ļ�//ExAllocatePoolWithTag(PagedPool, 0x20, 'niBI');//��ʼд�ļ�
	if (NULL == pWriteBuffer)
	{
		DbgPrint("%s->IBinaryNtWriteFile(%wZ)->Memory Error! \n", _ZwDeleteFile_H, uPathName);
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
		NULL
	);
	if (!NT_SUCCESS(ntStatus))
	{
		ZwClose(hFile);
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	ZwClose(hFile);
	ExFreePoolWithTag(pWriteBuffer, 'niBI');
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
	if (NT_SUCCESS(bRet))
		DbgPrint("%s->IBinaryNtZwDeleteFile(\"%wZ\")->Successful! \n", _ZwDeleteFile_H, &uDeletePathName);
	else
		DbgPrint("%s->IBinaryNtZwDeleteFile(\"%wZ\")->Failed(%d)! \n", _ZwDeleteFile_H, &uDeletePathName, bRet);
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
						DbgPrint("%s->IBinaryNtSetInformationFileDeleteFile(\"%wZ\")->GetFileBasicInformation Failed(%d)! \n", _ZwDeleteFile_H, &uDeletePathName, ntStatus);
					IBFileBasic.FileAttributes = FILE_ATTRIBUTE_NORMAL; //��������ΪĬ������
					ntStatus = ZwSetInformationFile(
						hFile,
						&ioStatus,
						&IBFileBasic,
						sizeof(IBFileBasic),
						FileBasicInformation
					);//���ҵ� FileBasic �����������õ�����ļ���
					if (!NT_SUCCESS(ntStatus))
						DbgPrint("%s->IBinaryNtSetInformationFileDeleteFile(\"%wZ\")->SetFileBasicInformation Failed(%d)! \n", _ZwDeleteFile_H, &uDeletePathName, ntStatus);
					
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
						0
					);
				}
				if (!NT_SUCCESS(ntStatus))
					DbgPrint("%s->IBinaryNtSetInformationFileDeleteFile(\"%wZ\")->Error Opening File(%d)! \n", _ZwDeleteFile_H, &uDeletePathName, ntStatus);
			}
		}
		
		/* ͨ�� ZwSetInformationFile ����ǿ��ɾ���ļ� */
		IBdelPostion.DeleteFile = TRUE;//�˱�־����Ϊ TRUE ����ɾ��
		ntStatus = ZwSetInformationFile(hFile, &ioStatus, &IBdelPostion, sizeof(IBdelPostion), FileDispositionInformation);
		if (!NT_SUCCESS(ntStatus))
		{
			ZwClose(hFile);
			DbgPrint("%s->IBinaryNtSetInformationFileDeleteFile(\"%wZ\")->SetFileInformation Failed(%d)! \n", _ZwDeleteFile_H, &uDeletePathName, ntStatus);
			return ntStatus;
		}
		ZwClose(hFile);
	}
	__except (1)
	{
		DbgPrint("%s->IBinaryNtSetInformationFileDeleteFile(\"%wZ\")->Unspecitied Error! \n", _ZwDeleteFile_H, &uDeletePathName);
	}
	
	return ntStatus;
}

BOOLEAN GooseBtZwDeleteFile(UNICODE_STRING us)
{
	close_all_file_handles(&us.Buffer[4]);
	return NT_SUCCESS(IBinaryNtZwDeleteFile(us)) || NT_SUCCESS(IBinaryNtSetInformationFileDeleteFile(us));
}



/** ��������ں��� **/
/* ����ֹͣ */
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	listenerUnload();
	DbgPrint("\n%s->DriverUnload()\n", _ZwDeleteFile_H);
	return;
}

/* �������� */
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgPrint("\n%s->DriverEntry()\n", _ZwDeleteFile_H);
	NTSTATUS bRet = listenerEntry(pDriver);
	if (!NT_SUCCESS(bRet))
		return bRet;
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}