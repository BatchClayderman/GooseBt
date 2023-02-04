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

NTSTATUS CallBackIrpCompleteionProc(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context)//去掉文件属性实现 CallBack 回调
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Context);
	Irp->UserIosb->Status = Irp->IoStatus.Status;//操作系统会设置这个回调
	Irp->UserIosb->Information = Irp->IoStatus.Information;
	KeSetEvent(Irp->UserEvent, IO_NO_INCREMENT, FALSE);
	IoFreeIrp(Irp);
	return STATUS_MORE_PROCESSING_REQUIRED;
}

NTSTATUS PassFileattribute(PFILE_OBJECT pFileObj)
{
	/** 删除文件的第三种方式：
	  * 步骤：
	  * （1）申请IRP,初始化IRP
	  * （2）初始化同步事件,以及设置回调.
	  * （3）设置文件属性为默认
	  * （4）发送IRP
	*/

	/* 1) 申请IRP,初始化IRP */
	PDEVICE_OBJECT pNextDeviceObj = NULL;
	PIRP pAllocIrp = NULL;
	KEVENT IrpSynEvent = { 0 };// Irp 同步需要的事件同步
	FILE_BASIC_INFORMATION fileBasciInfo = { 0 };
	IO_STATUS_BLOCK iostu;
	PIO_STACK_LOCATION IrpStack;
	
	pNextDeviceObj = IoGetRelatedDeviceObject(pFileObj);//通过文件对象获取其设备对象指针
	if (pNextDeviceObj == NULL)
		return STATUS_UNSUCCESSFUL;
	
	pAllocIrp = IoAllocateIrp(pNextDeviceObj->StackSize, TRUE);//通过设备对象指针确定申请的IRP的大小，注意在完成设置里面进行释放
	if (pAllocIrp == NULL)
		return STATUS_UNSUCCESSFUL;
	//初始化Irp
	//设置为自动,设置为无信号.
	KeInitializeEvent(&IrpSynEvent, SynchronizationEvent, FALSE);
	fileBasciInfo.FileAttributes = FILE_ATTRIBUTE_NORMAL;
	pAllocIrp->AssociatedIrp.SystemBuffer = &fileBasciInfo;
	pAllocIrp->UserIosb = &iostu;
	pAllocIrp->UserEvent = &IrpSynEvent;
	pAllocIrp->Tail.Overlay.OriginalFileObject = pFileObj;
	pAllocIrp->Tail.Overlay.Thread = (PETHREAD)KeGetCurrentThread();
	//获取下层堆栈.进行设置.
	//IrpStack  =
	IrpStack = IoGetNextIrpStackLocation(pAllocIrp);
	IrpStack->MajorFunction = IRP_MJ_SET_INFORMATION;
	IrpStack->DeviceObject = pNextDeviceObj;
	IrpStack->FileObject = pFileObj;
	IrpStack->Parameters.SetFile.Length = sizeof(FILE_BASIC_INFORMATION);
	IrpStack->Parameters.SetFile.FileObject = pFileObj;
	IrpStack->Parameters.SetFile.FileInformationClass = FileBasicInformation;
	//设置完成例程
	IoSetCompletionRoutine(pAllocIrp, CallBackIrpCompleteionProc, &IrpSynEvent, TRUE, TRUE, TRUE);
	//发送IRP
	IoCallDriver(pNextDeviceObj, pAllocIrp);
	//等待完成.
	KeWaitForSingleObject(&IrpSynEvent, Executive, KernelMode, TRUE, NULL);
	return STATUS_SUCCESS;
}

NTSTATUS FsDeleteFile(PFILE_OBJECT pFileObj)
{
	/*
  1.申请IRP,初始化IRP
  2.初始化同步事件,以及设置回调.
  3.设置文件属性为默认
  4.发送IRP
  核心:
	核心是设置 FileObject中的域.进而删除正在运行中的文件
  */
	PDEVICE_OBJECT pNextDeviceObj = NULL;
	PIRP pAllocIrp = NULL;
	KEVENT IrpSynEvent = { 0 }; //Irp同步需要的事件同步
	FILE_DISPOSITION_INFORMATION     fileBasciInfo = { 0 };  //注意此位置.已经变化为 FILE_DISPOSITION_INFORMATION
	IO_STATUS_BLOCK iostu;
	PIO_STACK_LOCATION IrpStack;
	PSECTION_OBJECT_POINTERS pFileExe;  //注意此属性要设置为0.欺骗系统进行删除
	//通过文件对象.获取其设备对象指针
	pNextDeviceObj = IoGetRelatedDeviceObject(pFileObj);
	if (pNextDeviceObj == NULL)
		return STATUS_UNSUCCESSFUL;
	//通过设备对象指针.确定申请的IRP的大小,注意在完成设置里面进行释放.
	pAllocIrp = IoAllocateIrp(pNextDeviceObj->StackSize, TRUE);
	if (pAllocIrp == NULL)
		return STATUS_UNSUCCESSFUL;
	//初始化Irp
	//设置为自动,设置为无信号.
	KeInitializeEvent(&IrpSynEvent, SynchronizationEvent, FALSE);
	fileBasciInfo.DeleteFile = TRUE;        //设置标记为删除
	pAllocIrp->AssociatedIrp.SystemBuffer = &fileBasciInfo;
	pAllocIrp->UserIosb = &iostu;
	pAllocIrp->UserEvent = &IrpSynEvent;
	pAllocIrp->Tail.Overlay.OriginalFileObject = pFileObj;
	pAllocIrp->Tail.Overlay.Thread = (PETHREAD)KeGetCurrentThread();
	//获取下层堆栈.进行设置.
	//IrpStack  =
	IrpStack = IoGetNextIrpStackLocation(pAllocIrp);
	IrpStack->MajorFunction = IRP_MJ_SET_INFORMATION;
	IrpStack->DeviceObject = pNextDeviceObj;
	IrpStack->FileObject = pFileObj;
	IrpStack->Parameters.SetFile.Length = sizeof(FILE_DISPOSITION_INFORMATION);
	IrpStack->Parameters.SetFile.FileObject = pFileObj;
	IrpStack->Parameters.SetFile.FileInformationClass = FileDispositionInformation;
	//设置完成例程
	IoSetCompletionRoutine(pAllocIrp, CallBackIrpCompleteionProc, &IrpSynEvent, TRUE, TRUE, TRUE);
	//删除正在运行中的文件.
	pFileExe = pFileObj->SectionObjectPointer;
	pFileExe->DataSectionObject = 0;
	pFileExe->ImageSectionObject = 0;
	//发送IRP
	IoCallDriver(pNextDeviceObj, pAllocIrp);
	//等待完成.
	KeWaitForSingleObject(&IrpSynEvent, Executive, KernelMode, TRUE, NULL);
	return STATUS_SUCCESS;
}
NTSTATUS IrpDeleteFileRun(UNICODE_STRING uDelFileName)
{
	KdBreakPoint();
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	/*
	1.首先通过发送IRP去掉文件的属性
	2.设置文件属性为删除.进行发送IRP强删文件.
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


	for (size = 1; ; size *= 2) //句柄表是动态变化的，所以不知道多大的buffer存放合适，从1B开始 指数增长
	{
		if (NULL == (buf = ExAllocatePool2(NonPagedPool, size, 'FILE')))
		{
			DbgPrint(("alloc mem failed\n"));
			goto Exit;
		}
		RtlZeroMemory(buf, size);
		status = ZwQuerySystemInformation(0x10, buf, size, NULL); //不像ZwEnumerateValueKey函数把参数传NULL就返回buffer的实际大小，所以就需要调整buffer的大小
		if (!NT_SUCCESS(status))
		{
			if (STATUS_INFO_LENGTH_MISMATCH == status) //这样写是为了在编译阶段发现status = STATUS_INFO_LENGTH_MISMATCH的错误
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
			break; //直到buffer能存放全局句柄表退出循环
		}
	}

	pSysHandleInfo = (PSYSTEM_HANDLE_INFORMATION)buf;
	NumOfHandle = pSysHandleInfo->NumberOfHandles; //句柄个数


	//句柄表只有handle和打开这个句柄的进程的PID，所以需要把句柄转换成具体的文件名，
	//而且转换过来的这个文件名是\device\harddiskvolume3\haha.doc这种格式的，但删除文件的文件名硬编码成这种格式"\??\c:\haha.doc",所以还需要转换一下格式
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

	for (i = 0; i < NumOfHandle; i++) //遍历全局句柄表
	{
		handleTEI = pSysHandleInfo->Handles[i];
		if (handleTEI.ObjectTypeIndex != 25 && handleTEI.ObjectTypeIndex != 28)//28文件,25设备对象
			continue;
		processID = (ULONG)handleTEI.UniqueProcessId; //打开该句柄的进程PID
		cid.UniqueProcess = (HANDLE)processID;
		cid.UniqueThread = (HANDLE)0;
		hHandle = (HANDLE)handleTEI.HandleValue;
		InitializeObjectAttributes(&oa, NULL, 0, NULL, NULL);
		status = ZwOpenProcess(&hProcess, PROCESS_DUP_HANDLE, &oa, &cid); //打开打开该句柄的进程，目的是把目标进程的句柄拷贝到当前进程中来，因为句柄不跨进程只在同一个进程有效，如果把句柄传给另一个进程，它是无效的，只能通过目标进程来访问该句柄
		if (!NT_SUCCESS(status))
		{
			KdPrint(("ZwOpenProcess:%d Fail ", processID));
			continue;
		}

		status = ZwDuplicateObject(hProcess, hHandle, NtCurrentProcess(), &hDupObj, \
			PROCESS_ALL_ACCESS, 0, DUPLICATE_SAME_ACCESS); //第一次拷贝，把句柄从`独占`它的进程空间中`拷贝`过来
		if (!NT_SUCCESS(status))
		{
			DbgPrint(("ZwDuplicateObject1 : Fail "));
			continue;
		}
		status = ObReferenceObjectByHandle( //根据句柄得到文件的内核对象
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
			sizeof(OBJECT_NAME_INFORMATION) + 1024 * sizeof(WCHAR), &ulRet))) //查询文件内核对象对应的文件名
		{
			ObDereferenceObject(fileObject);
			continue;
		}
		if (RtlCompareUnicodeString(&pObjName->Name, &delFileName, TRUE) == 0) //相等，则是要找被独占的强删的文件
		{

			ObDereferenceObject(fileObject);
			ZwClose(hDupObj);

			status = ZwDuplicateObject(hProcess, hHandle, NtCurrentProcess(), &hDupObj, \
				PROCESS_ALL_ACCESS, 0, DUPLICATE_SAME_ACCESS | DUPLICATE_CLOSE_SOURCE); //第二次拷贝，把句柄从`独占`它的进程空间中`拷贝`过来，同时把这个句柄从目标进程中`关闭掉`(函数传参传入这个参数`DUPLICATE_CLOSE_SOURCE`)
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
		IO_NO_PARAMETER_CHECKING); //不要进行参数校验，IoCreateFile才会调用成功

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
	/// 1.打开文件
	ntStatus = dfOpenFile(name, &handle, FILE_READ_ATTRIBUTES | DELETE, FILE_SHARE_DELETE);
	if (ntStatus == STATUS_OBJECT_NAME_NOT_FOUND || //传的名字错了或者路径错了
		ntStatus == STATUS_OBJECT_PATH_NOT_FOUND)
	{
		return FALSE;
	}
	else if (!NT_SUCCESS(ntStatus))
	{
		/// 2.a抹除文件的只读属性
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
				&basicInfo, sizeof(basicInfo), FileBasicInformation); //把文件的基本属性读出来
			if (!NT_SUCCESS(ntStatus))
			{

			}

			basicInfo.FileAttributes = FILE_ATTRIBUTE_NORMAL; //改成normal
			ntStatus = ZwSetInformationFile(handle, &ioStatus,
				&basicInfo, sizeof(basicInfo), FileBasicInformation); //把修改后的属性在写回去
			if (!NT_SUCCESS(ntStatus))
			{

			}
		}
		/// 2.b 遍历全局句柄表，关闭独占打开的句柄
		if (dfCloseFileHandle(name))
		{
			ntStatus = dfOpenFile(name, &handle, FILE_READ_ATTRIBUTES | DELETE, FILE_SHARE_DELETE); //再次打开文件，就会得到文件的句柄
			if (!NT_SUCCESS(ntStatus))
				return FALSE;
		}
		else
		{
			return FALSE;
		}
	}

	ntStatus = ObReferenceObjectByHandle(handle, //根据句柄拿到内核对象，因为句柄不跨进程只在同一个进程有效，如果把句柄传给另一个进程，它是无效的。所以一般是拿到handle之后直接得到它的`fileobject`
		DELETE,
		*IoFileObjectType, //如果没有指定一个句柄的类型，攻击者可以传入非文件类型的句柄从而造成系统漏洞，得到其他类型的内核对象，对应的结构体的定义里很可能没有一些成员，就会行为未定义或者无效内存，下面如果访问这些缺少的成员，系统会崩溃，造成蓝屏。
		KernelMode,
		&fileObject,
		NULL);

	if (!NT_SUCCESS(ntStatus))
	{
		DbgPrint("ObReferenceObjectByHandle()");
		ZwClose(handle);
		return FALSE;
	}

	DeviceObject = IoGetRelatedDeviceObject(fileObject); //通过fileObject拿到文件所在的设备对象，用来接受Irp
	/// 3. 构建IRP删除文件
	Irp = IoAllocateIrp(DeviceObject->StackSize, TRUE);

	if (Irp == NULL)
	{
		ObDereferenceObject(fileObject);
		ZwClose(handle);
		return FALSE;
	}

	KeInitializeEvent(&event, SynchronizationEvent, FALSE); //初始化事件，synchronization事件:自动恢复，比如声控灯。FALSE表示最初为无信号状态

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

	/// 为当前IRP设置一个完成例程，相当于回调函数，当下层驱动将Irp结束之后就会调用这个完成例程
	IoSetCompletionRoutine(
		Irp,
		dfSkillSetFileCompletion, //完成例程，在完成例程里面设置事件，通过这个事件通知上层驱动，上层驱动才知道Irp完成了，并且知道Irp的完成状态
		&event,
		TRUE,
		TRUE,
		TRUE);

	/// 2.b 删除正在运行中的exe所做的处理，欺骗操作系统
	pSectionObjectPointer = fileObject->SectionObjectPointer;
	if (pSectionObjectPointer)
	{
		pSectionObjectPointer->ImageSectionObject = 0;
		pSectionObjectPointer->DataSectionObject = 0;
	}
	/// 3. 将Irp往下发
	ntStatus = IoCallDriver(DeviceObject, Irp);
	if (!NT_SUCCESS(ntStatus))
	{
		ObDereferenceObject(fileObject);
		ZwClose(handle);
		return FALSE;
	}
	/// 等待完成例程中事件发生，NULL表示无限等待
	KeWaitForSingleObject(&event, Executive, KernelMode, TRUE, NULL);
	//IoFreeIrp(Irp);
	ObDereferenceObject(fileObject);
	ZwClose(handle);
	return TRUE;

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
	UNICODE_STRING uDelFileName = { 0 };
	RtlInitUnicodeString(&uDelFileName, L"\\DosDevices\\C:\\Users\\Universe\\Desktop\\GooseBtMain.exe");
	//GooseBtZwDeleteFile(uDelFileName);
	dfDelFile(L"\\DosDevices\\D:\\1.txt");
	return STATUS_SUCCESS;
}