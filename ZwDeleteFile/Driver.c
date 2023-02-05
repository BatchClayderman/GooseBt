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

NTSTATUS RetOpenFileHandle(UNICODE_STRING uDelFileName, PHANDLE pFileHandle)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	IO_STATUS_BLOCK iostu;
	HANDLE hFileHandle = 0;
	if (pFileHandle == NULL)
		return STATUS_UNSUCCESSFUL;
	if (KeGetCurrentIrql() > PASSIVE_LEVEL)
		return 0;
	if (uDelFileName.Length < 0 || uDelFileName.MaximumLength < 0)
	{
		return 0;
	}
	OBJECT_ATTRIBUTES ObjAttribute;
	ObjAttribute.ObjectName = &uDelFileName;
	ObjAttribute.SecurityDescriptor = NULL;
	ObjAttribute.SecurityQualityOfService = NULL;
	ObjAttribute.Attributes = OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE;
	ObjAttribute.Length = sizeof(OBJECT_ATTRIBUTES);
	/*  InitializeObjectAttributes(
		  &ObjAttribute,
		  &uDelFileName,
		  OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
		  NULL, NULL);*/
	ntStatus = IoCreateFile(&hFileHandle,
		FILE_READ_ATTRIBUTES,
		&ObjAttribute,
		&iostu,
		0,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_DELETE,
		FILE_OPEN,
		0,
		NULL,
		0,
		CreateFileTypeNone,
		NULL,
		IO_NO_PARAMETER_CHECKING
	);
	*pFileHandle = hFileHandle;
	return ntStatus;
}

NTSTATUS CallBackIrpCompleteionProc(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context)//ȥ���ļ�����ʵ�� CallBack �ص�
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Context);
	Irp->UserIosb->Status = Irp->IoStatus.Status;//����ϵͳ����������ص�
	Irp->UserIosb->Information = Irp->IoStatus.Information;
	KeSetEvent(Irp->UserEvent, IO_NO_INCREMENT, FALSE);
	IoFreeIrp(Irp);
	return STATUS_MORE_PROCESSING_REQUIRED;
}

NTSTATUS PassFileattribute(PFILE_OBJECT pFileObj)
{
	/** ɾ���ļ��ĵ����ַ�ʽ��
	  * ���裺
	  * ��1������IRP,��ʼ��IRP
	  * ��2����ʼ��ͬ���¼�,�Լ����ûص�.
	  * ��3�������ļ�����ΪĬ��
	  * ��4������IRP
	*/

	/* 1) ����IRP,��ʼ��IRP */
	PDEVICE_OBJECT pNextDeviceObj = NULL;
	PIRP pAllocIrp = NULL;
	KEVENT IrpSynEvent = { 0 };// Irp ͬ����Ҫ���¼�ͬ��
	FILE_BASIC_INFORMATION fileBasciInfo = { 0 };
	IO_STATUS_BLOCK iostu;
	PIO_STACK_LOCATION IrpStack;
	
	pNextDeviceObj = IoGetRelatedDeviceObject(pFileObj);//ͨ���ļ������ȡ���豸����ָ��
	if (pNextDeviceObj == NULL)
		return STATUS_UNSUCCESSFUL;
	
	pAllocIrp = IoAllocateIrp(pNextDeviceObj->StackSize, TRUE);//ͨ���豸����ָ��ȷ�������IRP�Ĵ�С��ע�������������������ͷ�
	if (pAllocIrp == NULL)
		return STATUS_UNSUCCESSFUL;
	//��ʼ��Irp
	//����Ϊ�Զ�,����Ϊ���ź�.
	KeInitializeEvent(&IrpSynEvent, SynchronizationEvent, FALSE);
	fileBasciInfo.FileAttributes = FILE_ATTRIBUTE_NORMAL;
	pAllocIrp->AssociatedIrp.SystemBuffer = &fileBasciInfo;
	pAllocIrp->UserIosb = &iostu;
	pAllocIrp->UserEvent = &IrpSynEvent;
	pAllocIrp->Tail.Overlay.OriginalFileObject = pFileObj;
	pAllocIrp->Tail.Overlay.Thread = (PETHREAD)KeGetCurrentThread();
	//��ȡ�²��ջ.��������.
	//IrpStack  =
	IrpStack = IoGetNextIrpStackLocation(pAllocIrp);
	IrpStack->MajorFunction = IRP_MJ_SET_INFORMATION;
	IrpStack->DeviceObject = pNextDeviceObj;
	IrpStack->FileObject = pFileObj;
	IrpStack->Parameters.SetFile.Length = sizeof(FILE_BASIC_INFORMATION);
	IrpStack->Parameters.SetFile.FileObject = pFileObj;
	IrpStack->Parameters.SetFile.FileInformationClass = FileBasicInformation;
	//�����������
	IoSetCompletionRoutine(pAllocIrp, CallBackIrpCompleteionProc, &IrpSynEvent, TRUE, TRUE, TRUE);
	//����IRP
	IoCallDriver(pNextDeviceObj, pAllocIrp);
	//�ȴ����.
	KeWaitForSingleObject(&IrpSynEvent, Executive, KernelMode, TRUE, NULL);
	return STATUS_SUCCESS;
}

NTSTATUS FsDeleteFile(PFILE_OBJECT pFileObj)
{
	/*
  1.����IRP,��ʼ��IRP
  2.��ʼ��ͬ���¼�,�Լ����ûص�.
  3.�����ļ�����ΪĬ��
  4.����IRP
  ����:
	���������� FileObject�е���.����ɾ�����������е��ļ�
  */
	PDEVICE_OBJECT pNextDeviceObj = NULL;
	PIRP pAllocIrp = NULL;
	KEVENT IrpSynEvent = { 0 }; //Irpͬ����Ҫ���¼�ͬ��
	FILE_DISPOSITION_INFORMATION     fileBasciInfo = { 0 };  //ע���λ��.�Ѿ��仯Ϊ FILE_DISPOSITION_INFORMATION
	IO_STATUS_BLOCK iostu;
	PIO_STACK_LOCATION IrpStack;
	PSECTION_OBJECT_POINTERS pFileExe;  //ע�������Ҫ����Ϊ0.��ƭϵͳ����ɾ��
	//ͨ���ļ�����.��ȡ���豸����ָ��
	pNextDeviceObj = IoGetRelatedDeviceObject(pFileObj);
	if (pNextDeviceObj == NULL)
		return STATUS_UNSUCCESSFUL;
	//ͨ���豸����ָ��.ȷ�������IRP�Ĵ�С,ע�������������������ͷ�.
	pAllocIrp = IoAllocateIrp(pNextDeviceObj->StackSize, TRUE);
	if (pAllocIrp == NULL)
		return STATUS_UNSUCCESSFUL;
	//��ʼ��Irp
	//����Ϊ�Զ�,����Ϊ���ź�.
	KeInitializeEvent(&IrpSynEvent, SynchronizationEvent, FALSE);
	fileBasciInfo.DeleteFile = TRUE;        //���ñ��Ϊɾ��
	pAllocIrp->AssociatedIrp.SystemBuffer = &fileBasciInfo;
	pAllocIrp->UserIosb = &iostu;
	pAllocIrp->UserEvent = &IrpSynEvent;
	pAllocIrp->Tail.Overlay.OriginalFileObject = pFileObj;
	pAllocIrp->Tail.Overlay.Thread = (PETHREAD)KeGetCurrentThread();
	//��ȡ�²��ջ.��������.
	//IrpStack  =
	IrpStack = IoGetNextIrpStackLocation(pAllocIrp);
	IrpStack->MajorFunction = IRP_MJ_SET_INFORMATION;
	IrpStack->DeviceObject = pNextDeviceObj;
	IrpStack->FileObject = pFileObj;
	IrpStack->Parameters.SetFile.Length = sizeof(FILE_DISPOSITION_INFORMATION);
	IrpStack->Parameters.SetFile.FileObject = pFileObj;
	IrpStack->Parameters.SetFile.FileInformationClass = FileDispositionInformation;
	//�����������
	IoSetCompletionRoutine(pAllocIrp, CallBackIrpCompleteionProc, &IrpSynEvent, TRUE, TRUE, TRUE);
	//ɾ�����������е��ļ�.
	pFileExe = pFileObj->SectionObjectPointer;
	pFileExe->DataSectionObject = 0;
	pFileExe->ImageSectionObject = 0;
	//����IRP
	IoCallDriver(pNextDeviceObj, pAllocIrp);
	//�ȴ����.
	KeWaitForSingleObject(&IrpSynEvent, Executive, KernelMode, TRUE, NULL);
	return STATUS_SUCCESS;
}
NTSTATUS IrpDeleteFileRun(UNICODE_STRING uDelFileName)
{
	KdBreakPoint();
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	/*
	1.����ͨ������IRPȥ���ļ�������
	2.�����ļ�����Ϊɾ��.���з���IRPǿɾ�ļ�.
	*/
	HANDLE hFileHandle = { 0 };
	PFILE_OBJECT  pFileObject = NULL;
	//sep1 : OpenFile Get File Handle
	ntStatus = RetOpenFileHandle(uDelFileName, &hFileHandle);
	if (!NT_SUCCESS(ntStatus))
	{
		goto ExitAnRelease;
	}
	//sep2:  Chang File Handle to FileObject
	ntStatus = ObReferenceObjectByHandle(
		hFileHandle,
		GENERIC_ALL,
		*IoFileObjectType,
		KernelMode,
		&pFileObject,
		NULL);
	if (!NT_SUCCESS(ntStatus))
	{
		goto ExitAnRelease;
	}
	//setp 3:  Pass File Atribute
	KdBreakPoint();
	ntStatus = PassFileattribute(pFileObject);
	if (!NT_SUCCESS(ntStatus))
	{
		goto ExitAnRelease;
	}
	//setp 4: Send Irp DeleteFile
	KdBreakPoint();
	ntStatus = FsDeleteFile(pFileObject);
	if (!NT_SUCCESS(ntStatus))
	{
		goto ExitAnRelease;
	}
ExitAnRelease:
	if (pFileObject != NULL)
		ObDereferenceObject(pFileObject);
	if (hFileHandle != NULL)
		ZwClose(hFileHandle);
	return ntStatus;
}

BOOLEAN GooseBtZwDeleteFile(UNICODE_STRING us)
{
	close_all_file_handles(&us.Buffer[4]);
	return NT_SUCCESS(IBinaryNtZwDeleteFile(us)) || NT_SUCCESS(IBinaryNtSetInformationFileDeleteFile(us)) || NT_SUCCESS(IrpDeleteFileRun(us)) || dfDelFile(us.Buffer);
}




PDEVICE_OBJECT	g_HookDevice;

NTSTATUS dfQuerySymbolicLink(
	IN PUNICODE_STRING SymbolicLinkName,
	OUT PUNICODE_STRING LinkTarget
)
{
	OBJECT_ATTRIBUTES oa;
	NTSTATUS status;
	HANDLE handle;

	InitializeObjectAttributes(
		&oa,
		SymbolicLinkName,
		OBJ_CASE_INSENSITIVE,
		0,
		0
	);

	status = ZwOpenSymbolicLinkObject(&handle, GENERIC_READ, &oa);
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	LinkTarget->MaximumLength = 1024 * sizeof(WCHAR);
	LinkTarget->Length = 0;
	LinkTarget->Buffer = ExAllocatePool2(PagedPool, LinkTarget->MaximumLength, 'A0');
	if (!LinkTarget->Buffer)
	{
		ZwClose(handle);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	RtlZeroMemory(LinkTarget->Buffer, LinkTarget->MaximumLength);

	status = ZwQuerySymbolicLinkObject(handle, LinkTarget, NULL);
	ZwClose(handle);

	if (!NT_SUCCESS(status))
	{
		ExFreePool(LinkTarget->Buffer);
	}

	return status;
}

BOOLEAN dfCloseFileHandle(WCHAR* name)
{

	NTSTATUS					 status;
	PVOID						 buf = NULL;
	PSYSTEM_HANDLE_INFORMATION 	 pSysHandleInfo;
	SYSTEM_HANDLE_TABLE_ENTRY_INFO handleTEI;

	ULONG						size = 1;
	ULONG						NumOfHandle = 0;
	ULONG						i;
	CLIENT_ID 					cid;
	HANDLE						hHandle;
	HANDLE						hProcess;
	HANDLE 						hDupObj;
	//HANDLE						hFile;
	//HANDLE						link_handle;
	OBJECT_ATTRIBUTES 			oa;
	//ULONG						FileType;
	ULONG						processID;
	UNICODE_STRING 				uLinkName;
	UNICODE_STRING				uLink;
	//OBJECT_ATTRIBUTES 			objectAttributes;
	//IO_STATUS_BLOCK 		 	IoStatus;
	ULONG 						ulRet;
	PVOID    			 		fileObject;
	POBJECT_NAME_INFORMATION 	pObjName = { 0 };
	UNICODE_STRING				delFileName = { 0 };
	//int							length;
	WCHAR						wVolumeLetter[3];
	WCHAR* pFilePath;
	UNICODE_STRING				uVolume;
	UNICODE_STRING				uFilePath;
	//UNICODE_STRING 				NullString = RTL_CONSTANT_STRING(L"");
	BOOLEAN					bRet = FALSE;


	for (size = 1; ; size *= 2) //������Ƕ�̬�仯�ģ����Բ�֪������buffer��ź��ʣ���1B��ʼ ָ������
	{
		if (NULL == (buf = ExAllocatePool2(NonPagedPool, size, 'FILE')))
		{
			DbgPrint(("alloc mem failed\n"));
			goto Exit;
		}
		RtlZeroMemory(buf, size);
		status = ZwQuerySystemInformation(0x10, buf, size, NULL); //����ZwEnumerateValueKey�����Ѳ�����NULL�ͷ���buffer��ʵ�ʴ�С�����Ծ���Ҫ����buffer�Ĵ�С
		if (!NT_SUCCESS(status))
		{
			if (STATUS_INFO_LENGTH_MISMATCH == status) //����д��Ϊ���ڱ���׶η���status = STATUS_INFO_LENGTH_MISMATCH�Ĵ���
			{
				ExFreePool(buf);
				buf = NULL;
			}
			else
			{
				DbgPrint(("ZwQuerySystemInformation() failed"));
				goto Exit;
			}
		}
		else
		{
			break; //ֱ��buffer�ܴ��ȫ�־�����˳�ѭ��
		}
	}

	pSysHandleInfo = (PSYSTEM_HANDLE_INFORMATION)buf;
	NumOfHandle = pSysHandleInfo->NumberOfHandles; //�������


	//�����ֻ��handle�ʹ��������Ľ��̵�PID��������Ҫ�Ѿ��ת���ɾ�����ļ�����
	//����ת������������ļ�����\device\harddiskvolume3\haha.doc���ָ�ʽ�ģ���ɾ���ļ����ļ���Ӳ��������ָ�ʽ"\??\c:\haha.doc",���Ի���Ҫת��һ�¸�ʽ
	/* Get the volume character like C: */
	//\??\c:\haha.doc-->\device\harddiskvolume3\haha.doc

	wVolumeLetter[0] = name[4];
	wVolumeLetter[1] = name[5];
	wVolumeLetter[2] = 0;
	uLinkName.Buffer = ExAllocatePool2(NonPagedPool, 256 + sizeof(ULONG), 'A1');
	uLinkName.MaximumLength = 256;
	RtlInitUnicodeString(&uVolume, wVolumeLetter);
	RtlInitUnicodeString(&uLink, L"\\DosDevices\\");
	RtlCopyUnicodeString(&uLinkName, &uLink);

	status = RtlAppendUnicodeStringToString(&uLinkName, &uVolume);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("RtlAppendUnicodeStringToString() failed"));
		return FALSE;
	}

	dfQuerySymbolicLink(&uLinkName, &delFileName);
	RtlFreeUnicodeString(&uLinkName);
	KdPrint(("delFileName:%wZ", &delFileName));

	pFilePath = (WCHAR*)&name[6];
	RtlInitUnicodeString(&uFilePath, pFilePath);

	RtlAppendUnicodeStringToString(&delFileName, &uFilePath);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("RtlAppendUnicodeStringToString() failed"));
		return FALSE;
	}

	KdPrint(("delFile:%wZ", &delFileName));

	for (i = 0; i < NumOfHandle; i++) //����ȫ�־����
	{
		handleTEI = pSysHandleInfo->Handles[i];
		if (handleTEI.ObjectTypeIndex != 25 && handleTEI.ObjectTypeIndex != 28)//28�ļ�,25�豸����
			continue;
		processID = (ULONG)handleTEI.UniqueProcessId; //�򿪸þ���Ľ���PID
		cid.UniqueProcess = (HANDLE)processID;
		cid.UniqueThread = (HANDLE)0;
		hHandle = (HANDLE)handleTEI.HandleValue;
		InitializeObjectAttributes(&oa, NULL, 0, NULL, NULL);
		status = ZwOpenProcess(&hProcess, PROCESS_DUP_HANDLE, &oa, &cid); //�򿪴򿪸þ���Ľ��̣�Ŀ���ǰ�Ŀ����̵ľ����������ǰ������������Ϊ����������ֻ��ͬһ��������Ч������Ѿ��������һ�����̣�������Ч�ģ�ֻ��ͨ��Ŀ����������ʸþ��
		if (!NT_SUCCESS(status))
		{
			KdPrint(("ZwOpenProcess:%d Fail ", processID));
			continue;
		}

		status = ZwDuplicateObject(hProcess, hHandle, NtCurrentProcess(), &hDupObj, \
			PROCESS_ALL_ACCESS, 0, DUPLICATE_SAME_ACCESS); //��һ�ο������Ѿ����`��ռ`���Ľ��̿ռ���`����`����
		if (!NT_SUCCESS(status))
		{
			DbgPrint(("ZwDuplicateObject1 : Fail "));
			continue;
		}
		status = ObReferenceObjectByHandle( //���ݾ���õ��ļ����ں˶���
			hDupObj,
			FILE_ANY_ACCESS,
			0,
			KernelMode,
			&fileObject,
			NULL);

		if (!NT_SUCCESS(status))
		{
			DbgPrint(("ObReferenceObjectByHandle : Fail "));
			continue;
		}

		pObjName = (POBJECT_NAME_INFORMATION)ExAllocatePool2(NonPagedPool, \
			sizeof(OBJECT_NAME_INFORMATION) + 1024 * sizeof(WCHAR), 'A1');
		if (!pObjName)
			goto Exit;
		if (STATUS_SUCCESS != (status = ObQueryNameString(fileObject, pObjName, \
			sizeof(OBJECT_NAME_INFORMATION) + 1024 * sizeof(WCHAR), &ulRet))) //��ѯ�ļ��ں˶����Ӧ���ļ���
		{
			ObDereferenceObject(fileObject);
			continue;
		}
		if (RtlCompareUnicodeString(&pObjName->Name, &delFileName, TRUE) == 0) //��ȣ�����Ҫ�ұ���ռ��ǿɾ���ļ�
		{

			ObDereferenceObject(fileObject);
			ZwClose(hDupObj);

			status = ZwDuplicateObject(hProcess, hHandle, NtCurrentProcess(), &hDupObj, \
				PROCESS_ALL_ACCESS, 0, DUPLICATE_SAME_ACCESS | DUPLICATE_CLOSE_SOURCE); //�ڶ��ο������Ѿ����`��ռ`���Ľ��̿ռ���`����`������ͬʱ����������Ŀ�������`�رյ�`(�������δ����������`DUPLICATE_CLOSE_SOURCE`)
			if (!NT_SUCCESS(status))
			{
				DbgPrint(("ZwDuplicateObject2 : Fail "));
				//return FALSE;
			}
			else
			{
				ZwClose(hDupObj);
				bRet = TRUE;
				//return TRUE;
			}
			break;

		}

		ExFreePool(pObjName);
		pObjName = NULL;

		ObDereferenceObject(fileObject);
		ZwClose(hDupObj);
		ZwClose(hProcess);

	}

Exit:
	if (pObjName != NULL)
	{
		ExFreePool(pObjName);
		pObjName = NULL;
	}
	if (delFileName.Buffer != NULL)
	{
		ExFreePool(delFileName.Buffer);
	}
	if (buf != NULL)
	{
		ExFreePool(buf);
		buf = NULL;
	}
	return(bRet);

}

NTSTATUS
dfOpenFile(WCHAR* name, PHANDLE phFileHandle, ACCESS_MASK access, ULONG share)
{

	IO_STATUS_BLOCK iosb;
	NTSTATUS stat;
	OBJECT_ATTRIBUTES oba;
	UNICODE_STRING nameus;

	if (KeGetCurrentIrql() > PASSIVE_LEVEL) { return 0; }
	RtlInitUnicodeString(&nameus, name);
	InitializeObjectAttributes(
		&oba,
		&nameus,
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
		0,
		0);
	stat = IoCreateFile(
		phFileHandle,
		access,
		&oba,
		&iosb,
		0,
		FILE_ATTRIBUTE_NORMAL,
		share,
		FILE_OPEN,
		0,
		NULL,
		0,
		0,
		NULL,
		IO_NO_PARAMETER_CHECKING); //��Ҫ���в���У�飬IoCreateFile�Ż���óɹ�

	return stat;
}

NTSTATUS
dfSkillSetFileCompletion(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp,
	IN PVOID Context
)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Context);
	Irp->UserIosb->Status = Irp->IoStatus.Status;
	Irp->UserIosb->Information = Irp->IoStatus.Information;

	KeSetEvent(Irp->UserEvent, IO_NO_INCREMENT, FALSE);

	IoFreeIrp(Irp);

	return STATUS_MORE_PROCESSING_REQUIRED;
}
BOOLEAN dfDelFile(WCHAR* name)
{
	NTSTATUS        ntStatus = STATUS_SUCCESS;
	PFILE_OBJECT    fileObject;
	PDEVICE_OBJECT  DeviceObject;
	PIRP            Irp;
	KEVENT          event;
	FILE_DISPOSITION_INFORMATION  FileInformation;
	IO_STATUS_BLOCK ioStatus;
	PIO_STACK_LOCATION irpSp;
	PSECTION_OBJECT_POINTERS pSectionObjectPointer;
	HANDLE handle;
	OBJECT_ATTRIBUTES                	objAttributes = { 0 };
	InitializeObjectAttributes(&objAttributes,
		&name,
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
		NULL,
		NULL);
	/// 1.���ļ�
	ntStatus = dfOpenFile(name, &handle, FILE_READ_ATTRIBUTES | DELETE, FILE_SHARE_DELETE);
	if (ntStatus == STATUS_OBJECT_NAME_NOT_FOUND || //�������ִ��˻���·������
		ntStatus == STATUS_OBJECT_PATH_NOT_FOUND)
	{
		return FALSE;
	}
	else if (!NT_SUCCESS(ntStatus))
	{
		/// 2.aĨ���ļ���ֻ������
		//ntStatus = dfOpenFile(name, &handle, FILE_READ_ATTRIBUTES, FILE_SHARE_DELETE);
		ntStatus = ZwCreateFile(
			&handle,
			SYNCHRONIZE | FILE_READ_ATTRIBUTES | FILE_WRITE_ATTRIBUTES,
			&objAttributes,
			&ioStatus,
			NULL,
			FILE_ATTRIBUTE_NORMAL,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			FILE_OPEN,
			FILE_SYNCHRONOUS_IO_NONALERT,
			NULL,
			0);
		if (NT_SUCCESS(ntStatus))
		{
			FILE_BASIC_INFORMATION        basicInfo = { 0 };

			ntStatus = ZwQueryInformationFile(handle, &ioStatus,
				&basicInfo, sizeof(basicInfo), FileBasicInformation); //���ļ��Ļ������Զ�����
			if (!NT_SUCCESS(ntStatus))
			{

			}

			basicInfo.FileAttributes = FILE_ATTRIBUTE_NORMAL; //�ĳ�normal
			ntStatus = ZwSetInformationFile(handle, &ioStatus,
				&basicInfo, sizeof(basicInfo), FileBasicInformation); //���޸ĺ��������д��ȥ
			if (!NT_SUCCESS(ntStatus))
			{

			}
		}
		/// 2.b ����ȫ�־�����رն�ռ�򿪵ľ��
		if (dfCloseFileHandle(name))
		{
			ntStatus = dfOpenFile(name, &handle, FILE_READ_ATTRIBUTES | DELETE, FILE_SHARE_DELETE); //�ٴδ��ļ����ͻ�õ��ļ��ľ��
			if (!NT_SUCCESS(ntStatus))
				return FALSE;
		}
		else
		{
			return FALSE;
		}
	}

	ntStatus = ObReferenceObjectByHandle(handle, //���ݾ���õ��ں˶�����Ϊ����������ֻ��ͬһ��������Ч������Ѿ��������һ�����̣�������Ч�ġ�����һ�����õ�handle֮��ֱ�ӵõ�����`fileobject`
		DELETE,
		*IoFileObjectType, //���û��ָ��һ����������ͣ������߿��Դ�����ļ����͵ľ���Ӷ����ϵͳ©�����õ��������͵��ں˶��󣬶�Ӧ�Ľṹ��Ķ�����ܿ���û��һЩ��Ա���ͻ���Ϊδ���������Ч�ڴ棬�������������Щȱ�ٵĳ�Ա��ϵͳ����������������
		KernelMode,
		&fileObject,
		NULL);

	if (!NT_SUCCESS(ntStatus))
	{
		DbgPrint("ObReferenceObjectByHandle()");
		ZwClose(handle);
		return FALSE;
	}

	DeviceObject = IoGetRelatedDeviceObject(fileObject); //ͨ��fileObject�õ��ļ����ڵ��豸������������Irp
	/// 3. ����IRPɾ���ļ�
	Irp = IoAllocateIrp(DeviceObject->StackSize, TRUE);

	if (Irp == NULL)
	{
		ObDereferenceObject(fileObject);
		ZwClose(handle);
		return FALSE;
	}

	KeInitializeEvent(&event, SynchronizationEvent, FALSE); //��ʼ���¼���synchronization�¼�:�Զ��ָ����������صơ�FALSE��ʾ���Ϊ���ź�״̬

	FileInformation.DeleteFile = TRUE;

	Irp->AssociatedIrp.SystemBuffer = &FileInformation;
	Irp->UserEvent = &event;
	Irp->UserIosb = &ioStatus;
	Irp->Tail.Overlay.OriginalFileObject = fileObject;
	Irp->Tail.Overlay.Thread = (PETHREAD)KeGetCurrentThread();
	Irp->RequestorMode = KernelMode;

	irpSp = IoGetNextIrpStackLocation(Irp);
	irpSp->MajorFunction = IRP_MJ_SET_INFORMATION;
	irpSp->DeviceObject = DeviceObject;
	irpSp->FileObject = fileObject;
	irpSp->Parameters.SetFile.Length = sizeof(FILE_DISPOSITION_INFORMATION);
	irpSp->Parameters.SetFile.FileInformationClass = FileDispositionInformation;
	irpSp->Parameters.SetFile.FileObject = fileObject;

	/// Ϊ��ǰIRP����һ��������̣��൱�ڻص����������²�������Irp����֮��ͻ��������������
	IoSetCompletionRoutine(
		Irp,
		dfSkillSetFileCompletion, //������̣�������������������¼���ͨ������¼�֪ͨ�ϲ��������ϲ�������֪��Irp����ˣ�����֪��Irp�����״̬
		&event,
		TRUE,
		TRUE,
		TRUE);

	/// 2.b ɾ�����������е�exe�����Ĵ�����ƭ����ϵͳ
	pSectionObjectPointer = fileObject->SectionObjectPointer;
	if (pSectionObjectPointer)
	{
		pSectionObjectPointer->ImageSectionObject = 0;
		pSectionObjectPointer->DataSectionObject = 0;
	}
	/// 3. ��Irp���·�
	ntStatus = IoCallDriver(DeviceObject, Irp);
	if (!NT_SUCCESS(ntStatus))
	{
		ObDereferenceObject(fileObject);
		ZwClose(handle);
		return FALSE;
	}
	/// �ȴ�����������¼�������NULL��ʾ���޵ȴ�
	KeWaitForSingleObject(&event, Executive, KernelMode, TRUE, NULL);
	//IoFreeIrp(Irp);
	ObDereferenceObject(fileObject);
	ZwClose(handle);
	return TRUE;

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
	UNICODE_STRING uDelFileName = { 0 };
	RtlInitUnicodeString(&uDelFileName, L"\\DosDevices\\C:\\Users\\Universe\\Desktop\\GooseBtMain.exe");
	//GooseBtZwDeleteFile(uDelFileName);
	dfDelFile(L"\\DosDevices\\D:\\1.txt");
	return STATUS_SUCCESS;
}