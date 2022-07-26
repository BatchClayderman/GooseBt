#include <ntifs.h>
#include <ntddk.h>
#ifndef _NTNoProcessCreate_H
#define _NTNoProcessCreate_H "NTNoProcessCreate"
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
#endif//_NTNoProcessCreate_H
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


/** 禁止创建进程 **/
VOID PCreateProcessNotifyRoutineEx(PEPROCESS Process, HANDLE ProcessId, PPS_CREATE_NOTIFY_INFO CreateInfo)
{
	UNREFERENCED_PARAMETER(Process);
	if (CreateInfo)
	{
		DbgPrint("%s->PreventProcessCreating->%u->%u->%wZ\n", _NTNoProcessCreate_H, HandleToULong(CreateInfo->ParentProcessId), HandleToULong(ProcessId), CreateInfo->CommandLine);
		CreateInfo->CreationStatus = STATUS_ACCESS_DENIED;//阻止
	}
	return;
}


/** 禁止创建线程 **/
VOID PCreateThreadNotifyRoutine(HANDLE ProcessId, HANDLE ThreadId, BOOLEAN Create)
{
	DbgPrint("%s->PreventThreadCreating->%u->%u->%s\n", _NTNoProcessCreate_H, HandleToULong(ProcessId), HandleToULong(ThreadId), Create ? "TRUE" : "FALSE");
	//阻止操作被取消
	return;
}


/** 禁止加载模块 **/
VOID PLoadModuleNotifyRoutine(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{
	UNREFERENCED_PARAMETER(ImageInfo);
	DbgPrint("%s->PreventModuleLoading->%u->%wZ\n", _NTNoProcessCreate_H, HandleToULong(ProcessId), FullImageName);
	RtlInitUnicodeString(FullImageName, L"NULL");//劫持
	return;
}



/** 驱动出入口函数 **/
/* 驱动停止 */
VOID DriverUnload(IN PDRIVER_OBJECT pDriverObj)
{
	UNREFERENCED_PARAMETER(pDriverObj);
	DbgPrint("\n%s->DriverUnload()\n", _NTNoProcessCreate_H);
	PsSetCreateProcessNotifyRoutineEx(PCreateProcessNotifyRoutineEx, TRUE);
	PsRemoveCreateThreadNotifyRoutine(PCreateThreadNotifyRoutine);
	PsRemoveLoadImageNotifyRoutine(PLoadModuleNotifyRoutine);
	if (obHandle != NULL)
		ObUnRegisterCallbacks(obHandle); // obHandle 是上面定义的 PVOID obHandle;
	DbgPrint("\n%s->Prevention Stop! \n", _NTNoProcessCreate_H);
	return;
}

/* 驱动入口 */
NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObj, IN PUNICODE_STRING pRegistryString)
{
	UNREFERENCED_PARAMETER(pRegistryString);
	DbgPrint("\n%s->DriverEntry()\n", _NTNoProcessCreate_H);

	pDriverObj->DriverUnload = DriverUnload;

	DbgPrint("%s->Trying to start global prevention. \n", _NTNoProcessCreate_H);
	PLDR_DATA_TABLE_ENTRY ldr = (PLDR_DATA_TABLE_ENTRY)pDriverObj->DriverSection;
	ldr->Flags |= 0x20;
	NTSTATUS status_process = PsSetCreateProcessNotifyRoutineEx(PCreateProcessNotifyRoutineEx, FALSE);
	NTSTATUS status_thread = PsSetCreateThreadNotifyRoutine(PCreateThreadNotifyRoutine);
	NTSTATUS status_module = PsSetLoadImageNotifyRoutine(PLoadModuleNotifyRoutine);
	DbgPrint("%s->(%d, %d, %d)\n", _NTNoProcessCreate_H, status_process, status_thread, status_module);
	if (NT_SUCCESS(status_process))
		DbgPrint("%s->Prevention Start Successfully! \n", _NTNoProcessCreate_H);
	else
		DbgPrint("%s->Prevention Start Failed! \n", _NTNoProcessCreate_H);

	return status_process;
}