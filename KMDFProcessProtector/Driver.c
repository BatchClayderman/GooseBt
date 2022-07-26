#include "ProcessProtect.h"
#ifndef _KMDFProcessProtector_H
#define _KMDFProcessProtector_H "KMDFProcessProtector"
#ifndef toProtectCnt
#define toProtectCnt 8
#endif
#ifndef PROCESS_NAME_LENGTH
#define PROCESS_NAME_LENGTH 1024
#endif
#define PROCESS_TERMINATE                  (0x0001)  
#define PROCESS_CREATE_THREAD              (0x0002)  
#define PROCESS_SET_SESSIONID              (0x0004)  
#define PROCESS_VM_OPERATION               (0x0008)  
#define PROCESS_VM_READ                    (0x0010)  
#define PROCESS_VM_WRITE                   (0x0020)  
#define PROCESS_DUP_HANDLE                 (0x0040)  
#define PROCESS_CREATE_PROCESS             (0x0080)  
#define PROCESS_SET_QUOTA                  (0x0100)  
#define PROCESS_SET_INFORMATION            (0x0200)  
#define PROCESS_QUERY_INFORMATION          (0x0400)  
#define PROCESS_SUSPEND_RESUME             (0x0800)  
#define PROCESS_QUERY_LIMITED_INFORMATION  (0x1000)  
#endif//_KMDFProcessProtector_H
static size_t s_cf_proc_name_offset = 0;
PVOID obHandle;//定义一个void*类型的变量，它将会作为 ObRegisterCallbacks 函数的第2个参数。


/** 结构体 **/
typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union {
		LIST_ENTRY HashLinks;
		struct
		{
			PVOID SectionPointer;
			ULONG CheckSum;
		}u;
	}v;
	union {
		struct
		{
			ULONG TimeDateStamp;
		}w;
		struct
		{
			PVOID LoadedImports;
		}x;
	}y;
	PVOID EntryPointActivationContext;
	PVOID PatchInformation;
}LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;


/** 自我保护函数 **/
OB_PREOP_CALLBACK_STATUS preCall(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION pOperationInformation)
{
	UNREFERENCED_PARAMETER(RegistrationContext);

	ANSI_STRING ansi_name;
	ULONG need_len;
	PEPROCESS curproc = (PEPROCESS)pOperationInformation->Object;
	UNICODE_STRING proc_name;
	WCHAR name_buf[PROCESS_NAME_LENGTH];
	RtlInitEmptyUnicodeString(&proc_name, name_buf, PROCESS_NAME_LENGTH * sizeof(WCHAR));
	// 这个名字是 ANSI 字符串，现在转化为 UNICODE 字符串。
	RtlInitAnsiString(&ansi_name, ((PCHAR)curproc + s_cf_proc_name_offset));
	need_len = RtlAnsiStringToUnicodeSize(&ansi_name);
	if (need_len > proc_name.MaximumLength)
		return OB_PREOP_SUCCESS;
	RtlAnsiStringToUnicodeString(&proc_name, &ansi_name, FALSE);

	UNICODE_STRING toProtect[toProtectCnt];
	RtlInitUnicodeString(&toProtect[0], L"GooseBtMain.exe");
	RtlInitUnicodeString(&toProtect[1], L"GooseBtMonitor.exe");
	RtlInitUnicodeString(&toProtect[2], L"GooseBtTray.exe");
	RtlInitUnicodeString(&toProtect[3], L"GooseBtSVC.exe");
	RtlInitUnicodeString(&toProtect[4], L"GooseBtUpdate.exe");
	RtlInitUnicodeString(&toProtect[5], L"Scanner.exe");
	RtlInitUnicodeString(&toProtect[6], L"wmip.exe");
	RtlInitUnicodeString(&toProtect[7], L"wmif.exe");
	BOOLEAN isFilter = FALSE;
	for (int i = 0; i < toProtectCnt; ++i)
		if (0 == RtlCompareUnicodeString(&toProtect[i], &proc_name, TRUE))//忽略大小写
		{
			isFilter = TRUE;
			break;
		}
	if (isFilter)
	{
		if (pOperationInformation->Operation == OB_OPERATION_HANDLE_CREATE)
		{
			if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_TERMINATE) == PROCESS_TERMINATE)
				pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_TERMINATE;
			if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_CREATE_THREAD) == PROCESS_CREATE_THREAD)
				pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_CREATE_THREAD;
			if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_SET_SESSIONID) == PROCESS_SET_SESSIONID)
				pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_SET_SESSIONID;
			//if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_OPERATION) == PROCESS_VM_OPERATION)
			//	pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_OPERATION;
			//if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_READ) == PROCESS_VM_READ)
			//	pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_READ;
			//if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_WRITE) == PROCESS_VM_WRITE)
			//	pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_WRITE;
			if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_DUP_HANDLE) == PROCESS_DUP_HANDLE)
				pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_DUP_HANDLE;
			if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_SET_QUOTA) == PROCESS_SET_QUOTA)
				pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_SET_QUOTA;
			if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_SET_INFORMATION) == PROCESS_SET_INFORMATION)
				pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_SET_INFORMATION;
			if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_QUERY_INFORMATION) == PROCESS_QUERY_INFORMATION)
				pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_QUERY_INFORMATION;
			//if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_SUSPEND_RESUME) == PROCESS_SUSPEND_RESUME)
			//	pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_SUSPEND_RESUME;
			if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_QUERY_LIMITED_INFORMATION) == PROCESS_QUERY_LIMITED_INFORMATION)
				pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_QUERY_LIMITED_INFORMATION;

		}
	}
	return OB_PREOP_SUCCESS;
}

NTSTATUS ProtectProcess(BOOLEAN Enable)
{
	UNREFERENCED_PARAMETER(Enable);
	OB_CALLBACK_REGISTRATION obReg;
	OB_OPERATION_REGISTRATION opReg;

	memset(&obReg, 0, sizeof(obReg));
	obReg.Version = ObGetFilterVersion();
	obReg.OperationRegistrationCount = 1;
	obReg.RegistrationContext = NULL;
	RtlInitUnicodeString(&obReg.Altitude, L"321000");

	memset(&opReg, 0, sizeof(opReg)); //初始化结构体变量

	/* 结构体的成员字段设置 */
	opReg.ObjectType = PsProcessType;
	opReg.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
	opReg.PreOperation = (POB_PRE_OPERATION_CALLBACK)&preCall; //在这里注册一个回调函数指针
	obReg.OperationRegistration = &opReg; //注意这一条语句
	return ObRegisterCallbacks(&obReg, &obHandle); //在这里注册回调函数
}

void cfCurProcNameInit()
{
	ULONG i;
	PEPROCESS  curproc;
	curproc = PsGetCurrentProcess();

	/* 搜索 EPROCESS 结构，在其中找到字符串 */
	for (i = 0; i < 3 * 4 * 1024; ++i)
	{
		if (!strncmp("System", (PCHAR)curproc + i, strlen("System")))
		{
			s_cf_proc_name_offset = i;
			break;
		}
	}
	return;
}


/** 隐藏保护 **/
//查找进程
PEPROCESS FindProcessByName(PWCHAR szName)
{
	NTSTATUS ntstatus = STATUS_SUCCESS;
	PEPROCESS pEprocess = NULL;

	//暴力枚举进程
	for (size_t i = 4; i < 0x10000000; i += 4)
	{
		//获取进程内核结构体
		ntstatus = PsLookupProcessByProcessId((HANDLE)i, &pEprocess);
		if (!NT_SUCCESS(ntstatus))
		{
			continue;
		}

		//获取进程路径
		PUNICODE_STRING ProcessPathName = NULL;
		ntstatus = SeLocateProcessImageName(pEprocess, &ProcessPathName);
		if (!NT_SUCCESS(ntstatus) || !ProcessPathName->Length)
		{
			continue;
		}

		//路径转换为大写
		_wcsupr(ProcessPathName->Buffer);
		DbgPrint("%ws \r\n", ProcessPathName->Buffer);

		//判断进程名
		if (wcsstr(ProcessPathName->Buffer, szName) != 0)
		{
			ExFreePoolWithTag(ProcessPathName, 0);
			return pEprocess;
		}

		ExFreePoolWithTag(ProcessPathName, 0);
	}

	return NULL;
}

//获取进程结构体链表偏移
ULONG GetActiveListOffset()//PEPROCESS pEprocess
{
	UNICODE_STRING FunName = { 0 };
	RtlInitUnicodeString(&FunName, L"PsGetProcessId");

	/* 获取函数地址 */
	PUCHAR pFun = (PUCHAR)MmGetSystemRoutineAddress(&FunName);
	if (pFun)
	{
		for (size_t i = 0; i < 0x100; i++)
		{
			//获取UniqueProcessId偏移2
			if (pFun[i] == 0x8B && pFun[i + 1] == 0x80)
			{
				//UniqueProcessId + 4为ActiveProcessLinks
				return *(PULONG)(pFun + i + 2) + 4;
			}
		}
	}

	return 0;

}

/* 隐藏进程 */
NTSTATUS HideProcess(PWCHAR szName)
{
	/* 查找进程 */
	PEPROCESS pEprocess = FindProcessByName(szName);
	if (!pEprocess)
		return STATUS_NOT_FOUND;

	/* 获取链表偏移 */
	ULONG uOffset = GetActiveListOffset(pEprocess);
	if (!uOffset)
		return STATUS_INVALID_LDT_OFFSET;

	/* 断链 */
	RemoveEntryList((PUCHAR)pEprocess + uOffset);
	return STATUS_SUCCESS;
}


ULONG find_eprocess_pid_offset()
{

	ULONG pid_ofs = 0;		// The offset we are looking for
	PEPROCESS eprocs;		// Process list
	PsLookupProcessByProcessId((HANDLE)10196, &eprocs);

	/*
	Go through the EPROCESS structure and look for the PID
	we can start at 0x20 because UniqueProcessId should
	not be in the first 0x20 bytes,
	also we should stop after 0x300 bytes with no success
	*/
	for (int i = 0x20; i < 0x300; i += 4)
		if (*(ULONG*)((UCHAR*)eprocs + i) == 10196)
		{
			pid_ofs = i;
			break;
		}

	ObDereferenceObject(eprocs);
	return pid_ofs;
}

VOID remove_links(PLIST_ENTRY Current)
{
	DbgPrint("%s->remove_links->Begin\n", _KMDFProcessProtector_H);
	PLIST_ENTRY Previous, Next;

	Previous = (Current->Blink);
	Next = (Current->Flink);

	// Loop over self (connect previous with next)
	Previous->Flink = Next;
	Next->Blink = Previous;

	// Re-write the current LIST_ENTRY to point to itself (avoiding BSOD)
	Current->Blink = (PLIST_ENTRY)&Current->Flink;
	Current->Flink = (PLIST_ENTRY)&Current->Flink;
	DbgPrint("%s->remove_links->End\n", _KMDFProcessProtector_H);
	return;
}

NTSTATUS HiddenProcess(UINT32 pid)
{
	DbgPrint("%s->Hidden(%lu)->Begin\n", _KMDFProcessProtector_H, pid);
	LPSTR result = ExAllocatePool2(NonPagedPool, sizeof(ULONG) + 20, 'tPrP');// Tag 阅读顺序是反的
	if (NULL == result)//申请失败则尝试适配 Win 10 2004 及以前的版本
		result = ExAllocatePoolZero(NonPagedPool, sizeof(ULONG) + 20, 'tPrP');// Tag 阅读顺序是反的
	if (NULL == result)
	{
		DbgPrint("%s->HiddenProtection->%s\n", _KMDFProcessProtector_H, "ExAllocatePool failed! ");
		return STATUS_MEMORY_NOT_ALLOCATED;
	}

	// Get PID offset nt!_EPROCESS.UniqueProcessId
	ULONG PID_OFFSET = find_eprocess_pid_offset();

	// Check if offset discovery was successful 
	if (0 == PID_OFFSET)
	{
		DbgPrint("%s->HiddenProtection->%s\n", _KMDFProcessProtector_H, "Could not find PID offset! ");
		return STATUS_NOT_FOUND;
	}

	// Get LIST_ENTRY offset nt!_EPROCESS.ActiveProcessLinks
	ULONG LIST_OFFSET = PID_OFFSET;

	// Check Architecture using pointer size
	INT_PTR ptr;

	// Ptr size 8 if compiled for a 64-bit machine, 4 if compiled for 32-bit machine
	LIST_OFFSET += sizeof(ptr);

	// Record offsets for user buffer
	//DbgPrint("%s->HiddenProcess: result = %s, size = %lu, offsets = %lu & %lu", _KMDFProcessProtector_H, result, (sizeof(ULONG) << 1) + 30, PID_OFFSET, LIST_OFFSET);

	// Get current process
	PEPROCESS CurrentEPROCESS = PsGetCurrentProcess();

	// Initialize other variables
	PLIST_ENTRY CurrentList = (PLIST_ENTRY)((ULONG_PTR)CurrentEPROCESS + LIST_OFFSET);
	PUINT32 CurrentPID = (PUINT32)((ULONG_PTR)CurrentEPROCESS + PID_OFFSET);

	// Check self 
	if (*(UINT32*)CurrentPID == pid)
	{
		remove_links(CurrentList);
		return STATUS_SUCCESS;
	}

	// Record the starting position
	PEPROCESS StartProcess = CurrentEPROCESS;

	// Move to next item
	CurrentEPROCESS = (PEPROCESS)((ULONG_PTR)CurrentList->Flink - LIST_OFFSET);
	CurrentPID = (PUINT32)((ULONG_PTR)CurrentEPROCESS + PID_OFFSET);
	CurrentList = (PLIST_ENTRY)((ULONG_PTR)CurrentEPROCESS + LIST_OFFSET);

	// Loop until we find the right process to remove
	// Or until we circle back
	while ((ULONG_PTR)StartProcess != (ULONG_PTR)CurrentEPROCESS)
	{
		// Check item
		if (*(UINT32*)CurrentPID == pid)
		{
			remove_links(CurrentList);
			return STATUS_SUCCESS;
		}

		// Move to next item
		CurrentEPROCESS = (PEPROCESS)((ULONG_PTR)CurrentList->Flink - LIST_OFFSET);
		CurrentPID = (PUINT32)((ULONG_PTR)CurrentEPROCESS + PID_OFFSET);
		CurrentList = (PLIST_ENTRY)((ULONG_PTR)CurrentEPROCESS + LIST_OFFSET);
	}
	DbgPrint("%s->Hidden(%lu)->End\n", _KMDFProcessProtector_H, pid);
	return STATUS_SUCCESS;
}



/** 驱动出入口函数 **/
/* 附加进程 */
NTSTATUS DispatchCreate(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchClose(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchCommon(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0L;
	IoCompleteRequest(pIrp, 0);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);
	NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;	 // STATUS_UNSUCCESSFUL
	PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
	ULONG uIoControlCode = 0;
	PVOID pIoBuffer = NULL;
	ULONG uInSize = 0;
	ULONG uOutSize = 0;

	// Get the IoCtrl Code
	uIoControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;

	pIoBuffer = pIrp->AssociatedIrp.SystemBuffer;
	uInSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
	uOutSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;

	switch (uIoControlCode)
	{
	case IOCTL_HELLO_WORLD:
		DbgPrint("%s->MY_CTL_CODE(0) = %d\n", _KMDFProcessProtector_H, IOCTL_HELLO_WORLD);
		status = STATUS_SUCCESS;
		break;
	case IOCTRL_REC_FROM_APP:// Receive data form Application
		if (uInSize > 0)// Do we have any data?
			DbgPrint("%s->Get Data from Application. \n", _KMDFProcessProtector_H);
		status = STATUS_SUCCESS;
		break;
	case IOCTRL_SEND_TO_APP:// Send data to Application
		// If we have enough room copy the data upto the App - note copy the terminating character as well...
		if (uOutSize >= strlen(DATA_TO_APP) + 1)
		{
			RtlCopyMemory(pIoBuffer,
				DATA_TO_APP,
				strlen(DATA_TO_APP) + 1);

			// Update the length for the App
			pIrp->IoStatus.Information = strlen(DATA_TO_APP) + 1;
			DbgPrint("%s->Send Data to Application. \n", _KMDFProcessProtector_H);
			status = STATUS_SUCCESS;
		}
		break;
	default:// Invalid code sent
		DbgPrint("%s->Unknown IOCTL: 0x%X (%04X,%04X)\n",
			_KMDFProcessProtector_H,
			uIoControlCode,
			DEVICE_TYPE_FROM_CTL_CODE(uIoControlCode),
			IoGetFunctionCodeFromCtlCode(uIoControlCode)
		);
		status = STATUS_INVALID_PARAMETER;
		break;
	}

	if (status == STATUS_SUCCESS)
		pIrp->IoStatus.Information = uOutSize;
	else
		pIrp->IoStatus.Information = 0;

	/* Complete the I/O Request */
	pIrp->IoStatus.Status = status;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return status;
}

/* 驱动停止 */
VOID DriverUnload(IN PDRIVER_OBJECT pDriverObj)
{
	UNICODE_STRING strLink;
	DbgPrint("\n%s->DriverUnload()\n", _KMDFProcessProtector_H);
	if (obHandle != NULL)
		ObUnRegisterCallbacks(obHandle); // obHandle 是上面定义的 PVOID obHandle;
	RtlInitUnicodeString(&strLink, SYMBOLIC_LINK_NAME);
	IoDeleteSymbolicLink(&strLink);
	IoDeleteDevice(pDriverObj->DeviceObject);
	DbgPrint("\n%s->Protection Stop! \n", _KMDFProcessProtector_H);
	return;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObj, IN PUNICODE_STRING pRegistryString)
{
	UNREFERENCED_PARAMETER(pRegistryString);
	DbgPrint("\n%s->DriverEntry()\n", _KMDFProcessProtector_H);

	NTSTATUS		status[3] = { STATUS_SUCCESS, STATUS_SUCCESS, STATUS_SUCCESS };
	UNICODE_STRING  ustrLinkName;
	UNICODE_STRING  ustrDevName;
	PDEVICE_OBJECT  pDevObj;

	pDriverObj->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
	pDriverObj->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;

	// Dispatch routine for communications
	pDriverObj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceControl;

	// Unload routine
	pDriverObj->DriverUnload = DriverUnload;

	// Initialize the device name.
	RtlInitUnicodeString(&ustrDevName, NT_DEVICE_NAME);

	// Create the device object and device extension
	status[0] = IoCreateDevice(pDriverObj,
		0,
		&ustrDevName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&pDevObj
	);

	if (!NT_SUCCESS(status[0]))
	{
		DbgPrint("%s->Error: IoCreateDevice = 0x%x\n", _KMDFProcessProtector_H, status[0]);
		return status[0];
	}

	if (IoIsWdmVersionAvailable(1, 0x10))//如果是支持符号链接用户相关性的系统
		RtlInitUnicodeString(&ustrLinkName, SYMBOLIC_LINK_GLOBAL_NAME);
	else//不支持
		RtlInitUnicodeString(&ustrLinkName, SYMBOLIC_LINK_NAME);

	/* Create a symbolic link to allow USER applications to access it. */
	status[0] = IoCreateSymbolicLink(&ustrLinkName, &ustrDevName);
	if (!NT_SUCCESS(status[0]))
	{
		DbgPrint("%s->Error: IoCreateSymbolicLink = 0x%x\n", _KMDFProcessProtector_H, status[0]);
		IoDeleteDevice(pDevObj);
		return status[0];
	}

	DbgPrint("%s->Trying to start protection. \n", _KMDFProcessProtector_H);
	cfCurProcNameInit();
	PLDR_DATA_TABLE_ENTRY ldr;
	ldr = (PLDR_DATA_TABLE_ENTRY)pDriverObj->DriverSection;
	ldr->Flags |= 0x20;
	status[1] = ProtectProcess(TRUE);
	//status[2] = HiddenProcess((ULONG)10196);
	status[2] = HideProcess(L"NOTEPAD.EXE");
	DbgPrint("%s->Protection Start! \n", _KMDFProcessProtector_H);
	DbgPrint("%s->Device = 0x%x\n", _KMDFProcessProtector_H, status[0]);
	DbgPrint("%s->Self-Protection = 0x%x\n", _KMDFProcessProtector_H, status[1]);
	DbgPrint("%s->Hidden-Protection = 0x%x\n", _KMDFProcessProtector_H, status[2]);

	return STATUS_SUCCESS;
}