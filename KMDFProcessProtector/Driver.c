#include "ProcessProtect.h"
#ifndef _KMDFProcessProtector_H
#define _KMDFProcessProtector_H "KMDFProcessProtector"
#ifndef toProtectCnt
#define toProtectCnt 9
#endif
#ifndef initProtectList
#define initProtectList											\
UNICODE_STRING toProtect[toProtectCnt];							\
RtlInitUnicodeString(&toProtect[0], L"GooseBtMain.exe");		\
RtlInitUnicodeString(&toProtect[1], L"GooseBtMonitor.exe");		\
RtlInitUnicodeString(&toProtect[2], L"GooseBtTray.exe");		\
RtlInitUnicodeString(&toProtect[3], L"GooseBtSVC.exe");			\
RtlInitUnicodeString(&toProtect[4], L"GooseBtUpdate.exe");		\
RtlInitUnicodeString(&toProtect[5], L"Scanner.exe");			\
RtlInitUnicodeString(&toProtect[6], L"wmip.exe");				\
RtlInitUnicodeString(&toProtect[7], L"wmif.exe");				\
RtlInitUnicodeString(&toProtect[8], L"notepad.exe");
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
UCHAR* PsGetProcessImageFileName(PEPROCESS Process);
static size_t s_cf_proc_name_offset = 0;
PVOID obHandle;//定义一个 void* 类型的变量，它将会作为 ObRegisterCallbacks 函数的第 2 个参数。


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
VOID cfCurProcNameInit()
{
	ULONG i;
	PEPROCESS curproc = PsGetCurrentProcess();

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

OB_PREOP_CALLBACK_STATUS preCall(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION pOperationInformation)
{
	UNREFERENCED_PARAMETER(RegistrationContext);

	ANSI_STRING ansi_name;
	ULONG need_len;
	PEPROCESS curproc = (PEPROCESS)pOperationInformation->Object;
	UNICODE_STRING proc_name;
	WCHAR name_buf[PROCESS_NAME_LENGTH];
	RtlInitEmptyUnicodeString(&proc_name, name_buf, PROCESS_NAME_LENGTH * sizeof(WCHAR));
	RtlInitAnsiString(&ansi_name, ((PCHAR)curproc + s_cf_proc_name_offset));
	need_len = RtlAnsiStringToUnicodeSize(&ansi_name);
	if (need_len > proc_name.MaximumLength)
		return OB_PREOP_SUCCESS;
	RtlAnsiStringToUnicodeString(&proc_name, &ansi_name, FALSE);// ANSI_STRING to UNICODE_STRING
	
	initProtectList
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
			DbgPrint("%s->Protected(%wZ)\n", _KMDFProcessProtector_H, &proc_name);
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

NTSTATUS ProtectProcessByName()
{
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


/** 隐藏保护函数 **/
/* 隐藏线程 */
VOID HideThreadsByEprocess(PEPROCESS pEprocess)
{
	RemoveEntryList((PLIST_ENTRY)(*(PULONGLONG)((char*)pEprocess + 0x030)));// KTHREAD -> ThreadListEntry
	RemoveEntryList((PLIST_ENTRY)(*(PULONGLONG)((char*)pEprocess + 0x5e0)));// ETHREAD -> ThreadListEntry
	return;
}

NTSTATUS HideThreadsByName()
{
	BOOLEAN bFlag = FALSE;
	NTSTATUS ntstatus = 0;
	PEPROCESS pEprocess = NULL;

	initProtectList

	/* 定位进程内核结构 */
	for (size_t i = 4; i < 0x100000; i += 4)
	{
		ntstatus = PsLookupProcessByProcessId((HANDLE)i, &pEprocess);

		if (NT_SUCCESS(ntstatus))
		{
			//释放内核对应引用计次
			ObDereferenceObject(pEprocess);
			//+0x5a8 ImageFileName : [15] UChar

			/* 获取进程名 */
			PUCHAR szProcessName = PsGetProcessImageFileName(pEprocess);
			ANSI_STRING ansiBuffer = { 0 };
			UNICODE_STRING unicodeBuffer = { 0 };
			size_t szProcessNamelen = strlen((PCHAR)szProcessName);
			ansiBuffer.Buffer = (PCHAR)szProcessName;
			ansiBuffer.Length = ansiBuffer.MaximumLength = (USHORT)szProcessNamelen;
			RtlAnsiStringToUnicodeString(&unicodeBuffer, &ansiBuffer, TRUE);

			/* 判断 */
			for (int j = 0; j < toProtectCnt; ++j)
				if (0 == RtlCompareUnicodeString(&toProtect[j], &unicodeBuffer, TRUE))//忽略大小写
				{
					bFlag = TRUE;
					HideThreadsByEprocess(pEprocess);
				}
		}
	}

	return bFlag ? STATUS_SUCCESS : STATUS_NOT_FOUND;
}

NTSTATUS HideProcessByName()
{
	PUCHAR szProcessName = PsGetProcessImageFileName(PsInitialSystemProcess);
	UNICODE_STRING currentName = { 0 };
	PLIST_ENTRY first = { 0 }, current = { 0 };
	first = (PLIST_ENTRY)(((PUCHAR)PsInitialSystemProcess + 0x448));
	current = first;
	
	initProtectList
	BOOLEAN isExist = FALSE;
	do
	{
		szProcessName = PsGetProcessImageFileName((PEPROCESS)((PUCHAR)current - 0x448));
		ANSI_STRING ansiBuffer = { 0 };
		size_t szProcessNamelen = strlen((PCHAR)szProcessName);
		ansiBuffer.Buffer = (PCHAR)szProcessName;
		ansiBuffer.Length = ansiBuffer.MaximumLength = (USHORT)szProcessNamelen;
		RtlAnsiStringToUnicodeString(&currentName, &ansiBuffer, TRUE);
		for (int i = 0; i < toProtectCnt; ++i)
			if (0 == RtlCompareUnicodeString(&toProtect[i], &currentName, TRUE))//忽略大小写
			{
				isExist = TRUE;
				current->Flink->Blink = current->Blink;//后一个结点的上一个结点指向当前结点的上一个结点
				current->Blink->Flink = current->Flink;//前一个结点的下一个结点指向当前结点的下一个结点
				break;//即使重复也过滤了所以可以直接 break 节约时间
			}
		current = current->Flink;//遍历下一个节点
	} while (current != NULL && current != first);//strcmp(szProcessName, "") == 0
	
	return isExist ? STATUS_SUCCESS : STATUS_NOT_FOUND;
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

/* 驱动入口 */
NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObj, IN PUNICODE_STRING pRegistryString)
{
	UNREFERENCED_PARAMETER(pRegistryString);
	DbgPrint("\n%s->DriverEntry()\n", _KMDFProcessProtector_H);

	NTSTATUS		status[4] = { STATUS_SUCCESS, STATUS_SUCCESS, STATUS_SUCCESS, STATUS_SUCCESS };
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
	
	/* Protect */
	DbgPrint("%s->Trying to start protection. \n", _KMDFProcessProtector_H);
	cfCurProcNameInit();
	PLDR_DATA_TABLE_ENTRY ldr = (PLDR_DATA_TABLE_ENTRY)pDriverObj->DriverSection;
	ldr->Flags |= 0x20;
	status[1] = ProtectProcessByName();
	status[2] = HideProcessByName();
	status[2] = HideThreadsByName();
	DbgPrint("%s->Protection Start! \n", _KMDFProcessProtector_H);
	DbgPrint("%s->Device = 0x%x\n", _KMDFProcessProtector_H, status[0]);
	DbgPrint("%s->Self-Protection = 0x%x\n", _KMDFProcessProtector_H, status[1]);
	DbgPrint("%s->Hidden-Process-Protection = 0x%x\n", _KMDFProcessProtector_H, status[2]);
	DbgPrint("%s->Hidden-Threads-Protection = 0x%x\n", _KMDFProcessProtector_H, status[3]);

	return STATUS_SUCCESS;
}