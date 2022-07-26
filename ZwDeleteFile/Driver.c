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


/** 驱动级文件操作函数 **/
/* 目录和文件的创建 */
NTSTATUS IBinaryNtCreateFile(UNICODE_STRING ustr)//创建文件
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
	InitializeObjectAttributes(&ObjAttus,//初始化 ObjAttus 结构
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

NTSTATUS IBinaryNtCreateDirectory(UNICODE_STRING uPathName)//创建目录
{
	wchar_t tmp[MAX_PATH] = { 0 };
	wcscpy_s(tmp, MAX_PATH, uPathName.Buffer);
	if (uPathName.Length > 0 && tmp[uPathName.Length - 1] != '\\')//如果最后一个字符不是'\\'
	{
		wcscat_s(tmp, MAX_PATH, L"\\");
		uPathName.Buffer = tmp;
		++uPathName.Length;
	}
	NTSTATUS ntStatus;
	HANDLE hFile;
	OBJECT_ATTRIBUTES objAttus = { 0 };
	IO_STATUS_BLOCK ioStatus = { 0 };
	InitializeObjectAttributes(&objAttus,//初始化文件属性结构体
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
		FILE_ATTRIBUTE_DIRECTORY,//我们设置创建文件
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN_IF,
		FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,//表示创建的是目录并且是同步执行
		NULL,
		0
	);
	if (NT_SUCCESS(ntStatus))
		ZwClose(hFile);
	return ntStatus;
}

/* 文件解锁 */
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
			if (!NT_SUCCESS(status))//不是所有程序都有文件句柄
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

/* 文件读写 */
ULONG64 GetFileSize(HANDLE hfile)//获取文件大小
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

NTSTATUS IBinaryNtReadFile(PVOID pszBuffer, UNICODE_STRING uPathName)//读取文件
{
	OBJECT_ATTRIBUTES objAttri = { 0 };
	NTSTATUS ntStaus;
	HANDLE hFile;
	IO_STATUS_BLOCK ioStatus = { 0 };
	PVOID pReadBuffer = NULL;
	if (NULL == pszBuffer)
		return STATUS_INTEGER_DIVIDE_BY_ZERO;
	InitializeObjectAttributes(&objAttri,//打开文件读取文件
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
	pReadBuffer = ExAllocatePool2(PagedPool, 100, 'niBI');//读取文件//ExAllocatePoolWithTag(PagedPool, 100, 'niBI');//读取文件
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
	RtlCopyMemory(pszBuffer, pReadBuffer, 100);//将读取的内容拷贝到传入的缓冲区.
	ZwClose(hFile);//记得关闭文件
	if (NULL != pReadBuffer)
		ExFreePoolWithTag(pReadBuffer, 'niBI');
	return ntStaus;
}

NTSTATUS IBinaryNtWriteFile(UNICODE_STRING uPathName)//写文件
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
		FILE_OPEN,//注意此标志——打开文件文件不存在则失败
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;
	pWriteBuffer = ExAllocatePool2(PagedPool, 0x20, 'niBI');//开始写文件//ExAllocatePoolWithTag(PagedPool, 0x20, 'niBI');//开始写文件
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

/* 文件删除 */
NTSTATUS IBinaryNtZwDeleteFile(UNICODE_STRING uDeletePathName)//直接驱动级文件删除
{
	OBJECT_ATTRIBUTES obAttri = { 0 };
	InitializeObjectAttributes(&obAttri,//初始化源文件路径并且打开
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

NTSTATUS IBinaryNtSetInformationFileDeleteFile(UNICODE_STRING uDeletePathName)//读写文件信息以执行驱动删除
{
	/** 删除文件的第二种方式
	  * 思路：
	  * （1）初始化文件路径
	  * （2）在共享模式下使用读写方式打开文件
	  * （3）如果是访问遭到拒绝则以另一种方式打开文件并设置该文件的信息
	  * （4）设置成功之后就可以执行驱动级删除操作了
	  */
	
	OBJECT_ATTRIBUTES objAttri;
	NTSTATUS ntStatus = STATUS_FAILED_STACK_SWITCH;
	HANDLE hFile;
	IO_STATUS_BLOCK ioStatus;
	FILE_DISPOSITION_INFORMATION IBdelPostion = { 0 };//通过 ZwSetInformationFile 删除需要使用这个结构体
	__try
	{
		InitializeObjectAttributes(&objAttri,
			&uDeletePathName,
			OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
			NULL,
			NULL
		);
		ntStatus = ZwCreateFile(&hFile,
			DELETE | FILE_WRITE_DATA | SYNCHRONIZE,//文件权限：删除权限和写权限
			&objAttri,
			&ioStatus,
			NULL,
			FILE_ATTRIBUTE_NORMAL,//文件的属性是默认
			FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,//文件的共享模式：删除和读写
			FILE_OPEN,//文件的打开方式是“打开”，如果不存在则返回失败
			FILE_SYNCHRONOUS_IO_NONALERT | FILE_DELETE_ON_CLOSE,//文件的应用选项，如果是 FILE_DELETE_ON_CLOSE 则使用 ZwClose 关闭文件句柄的时候删除这个文件
			NULL,
			0
		);
		if (!NT_SUCCESS(ntStatus))
		{
			if (STATUS_ACCESS_DENIED == ntStatus)//如果不成功则判断文件是否拒绝访问，是的话我们就设置为可以访问并进行删除
			{
				ntStatus = ZwCreateFile(&hFile,
					SYNCHRONIZE | FILE_READ_ATTRIBUTES | FILE_WRITE_ATTRIBUTES,//删除权限失败就以读写模式
					&objAttri,
					&ioStatus,
					NULL,
					FILE_ATTRIBUTE_NORMAL,//文件的属性为默认
					FILE_SHARE_DELETE | FILE_SHARE_WRITE | FILE_SHARE_READ,//文件的共享属性为读写删除
					FILE_OPEN,//文件的打开方式为“打开”，不存在则失败
					FILE_SYNCHRONOUS_IO_NONALERT,//文件的应用选项
					NULL,
					0
				);
				if (NT_SUCCESS(ntStatus))//如果打开成功了则设置这个文件的信息
				{
					FILE_BASIC_INFORMATION IBFileBasic = { 0 };
					ntStatus = ZwQueryInformationFile(//使用 ZwQueryInformationfile 遍历文件的基本信息
						hFile,
						&ioStatus,
						&IBFileBasic,
						sizeof(IBFileBasic),
						FileBasicInformation
					);
					if (!NT_SUCCESS(ntStatus))//遍历文件信息失败
						DbgPrint("%s->IBinaryNtSetInformationFileDeleteFile(\"%wZ\")->GetFileBasicInformation Failed(%d)! \n", _ZwDeleteFile_H, &uDeletePathName, ntStatus);
					IBFileBasic.FileAttributes = FILE_ATTRIBUTE_NORMAL; //设置属性为默认属性
					ntStatus = ZwSetInformationFile(
						hFile,
						&ioStatus,
						&IBFileBasic,
						sizeof(IBFileBasic),
						FileBasicInformation
					);//将我的 FileBasic 基本属性设置到这个文件中
					if (!NT_SUCCESS(ntStatus))
						DbgPrint("%s->IBinaryNtSetInformationFileDeleteFile(\"%wZ\")->SetFileBasicInformation Failed(%d)! \n", _ZwDeleteFile_H, &uDeletePathName, ntStatus);
					
					ZwClose(hFile);//如果成功关闭文件句柄
					ntStatus = ZwCreateFile(&hFile,//重新打开这个设置信息后的文件.
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
		
		/* 通过 ZwSetInformationFile 进行强制删除文件 */
		IBdelPostion.DeleteFile = TRUE;//此标志设置为 TRUE 即可删除
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



/** 驱动出入口函数 **/
/* 驱动停止 */
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	listenerUnload();
	DbgPrint("\n%s->DriverUnload()\n", _ZwDeleteFile_H);
	return;
}

/* 驱动启动 */
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