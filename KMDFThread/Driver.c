#include <ntddk.h>
#include <windef.h>
#include <stdio.h>
#include <string.h>
#ifndef _KMDFThread_H
#define _KMDFThread_H "KMDFThread"
#ifndef DeviceName
#define DeviceName L"\\Device\\KMDFThread"
#endif
#ifndef DosDeviceName
#define DosDeviceName L"\\DosDevices\\KMDFThread"
#endif
#ifndef SYSNAME
#define SYSNAME "System"  
#endif
#endif//_KMDFThread_H


/** 全局变量 **/
ULONG ProcessNameOffset = 0;
NTSTATUS PsLookupProcessByProcessId(IN ULONG ulProcId, OUT PEPROCESS* pEProcess);
HANDLE g_dwProcessId;
BOOL g_bMainThread;
//VOID ImageCreateMon(IN PUNICODE_STRING FullImageName, IN HANDLE ProcessId, IN PIMAGE_INFO ImageInfo );
char msg[MAX_PATH] = "NULL";


/** 命名管道 **/
NTSTATUS DeviceCreate(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	pIrp->IoStatus.Status = STATUS_SUCCESS;// getLastError() 得到的值
	pIrp->IoStatus.Information = 0;//返回给 R3 多少数据，没有填 0
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	DbgPrint("%s->CreateOver\n", _KMDFThread_H);//不许 sprintf
	return STATUS_SUCCESS;
}

NTSTATUS DeviceClose(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	DbgPrint("%s->CloseOver\n", _KMDFThread_H);//不许 sprintf
	return STATUS_SUCCESS;
}

NTSTATUS DeviceRead(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	if (strcmp(msg, "STOP") == 0)
	{
		DeviceClose(pDeviceObject, pIrp);
		return STATUS_PIPE_EMPTY;
	}
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	ULONG readLength = stack->Parameters.Read.Length;
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = min(readLength, strlen(msg));
	CHAR* outc = (CHAR*)pIrp->AssociatedIrp.SystemBuffer;
	strncpy(outc, msg, strlen(msg));
	//memset(pIrp->AssociatedIrp.SystemBuffer, 0x90, readLength);
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	strcpy_s(msg, sizeof(msg), "NULL");
	//DbgPrint("%s->ReadOver\n", _KMDFThread_H);//不许 sprintf
	return STATUS_SUCCESS;
}

NTSTATUS DeviceWrite(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	ULONG readLength = stack->Parameters.Read.Length;
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = readLength;
	memset(pIrp->AssociatedIrp.SystemBuffer, 0x90, readLength);
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	//DbgPrint("%s->WriteOver\n", _KMDFThread_H);//不许 sprintf
	return STATUS_SUCCESS;
}


/** 关键函数 **/
NTSTATUS CommonDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0L;
	IoCompleteRequest(Irp, 0);
	return Irp->IoStatus.Status;
}

VOID ProcessCreateMon(IN HANDLE hParentId, IN HANDLE PId, IN BOOLEAN bCreate)
{
	PEPROCESS   EProcess;
	//ULONG       ulCurrentProcessId;
	LPTSTR      lpCurProc;
	NTSTATUS    status;

#ifdef _AMD64_  
	ULONG ProcessId = HandleToUlong(PId);
	status = PsLookupProcessByProcessId(ProcessId, &EProcess);
#else  
	//HANDLE ProcessId = PId;
	status = PsLookupProcessByProcessId((ULONG)PId, &EProcess);
#endif  

	if (!NT_SUCCESS(status))
	{
		sprintf(msg, "%s->PsLookupProcessByProcessId()->Failed!\n", _KMDFThread_H);
		DbgPrint(msg);
		return;
	}

	if (bCreate)
	{
		g_bMainThread = TRUE;
		lpCurProc = (LPTSTR)EProcess;
		lpCurProc = lpCurProc + ProcessNameOffset;
		
		sprintf(msg, "%s->Created->Process = %s->PID = %d->Address = %x->PPID = %d\n",
			_KMDFThread_H,
			lpCurProc,
			(int)PId,
			(ULONG)EProcess,
			(int)hParentId
		);
		DbgPrint(msg);
	}
	else
	{
		sprintf(msg, "%s->Terminated->PID = %d\n", _KMDFThread_H, (int)PId);
		DbgPrint(msg);
	}
	return;
}

VOID ThreadCreateMon(IN HANDLE PId, IN HANDLE TId, IN BOOLEAN bCreate)
{
	PEPROCESS  EProcess, ParentEProcess;
	LPTSTR     lpCurProc, lpParnentProc;
	NTSTATUS   status;

#ifdef _AMD64_  
	ULONG System = 4;
	ULONG dwParentPID = HandleToUlong(PsGetCurrentProcessId());
	ULONG ProcessId = HandleToUlong(PId);
	status = PsLookupProcessByProcessId(ProcessId, &EProcess);
	status = PsLookupProcessByProcessId(dwParentPID, &ParentEProcess);
#else  
	HANDLE System = (HANDLE)4;
	HANDLE dwParentPID = PsGetCurrentProcessId();
	HANDLE ProcessId = PId;
	status = PsLookupProcessByProcessId((ULONG)ProcessId, &EProcess);
	status = PsLookupProcessByProcessId((ULONG)dwParentPID, &ParentEProcess);
#endif  

	if (!NT_SUCCESS(status))
	{
		sprintf(msg, "%s->PsLookupProcessByProcessId()->Failed!\n", _KMDFThread_H);
		DbgPrint(msg);
		return;
	}

	if (bCreate)
	{
		if ((g_bMainThread == TRUE) && (ProcessId != System) && (ProcessId != dwParentPID))
		{
			HANDLE dwParentTID = PsGetCurrentThreadId();
			lpCurProc = (LPTSTR)EProcess;
			lpParnentProc = (LPTSTR)ParentEProcess;
			lpCurProc += ProcessNameOffset;
			lpParnentProc += ProcessNameOffset;
			sprintf(msg, "%s->Call->(Process = %s->PID = %d->TID = %d)->(Process = %s->PID = %d->TID = %d)\n",
				_KMDFThread_H,
				lpParnentProc,
				(int)dwParentPID,
				(int)dwParentTID,
				lpCurProc,
				(int)ProcessId,
				(int)TId
			);
			DbgPrint(msg);
			g_bMainThread = FALSE;
		}

		lpCurProc = (LPTSTR)EProcess;
		lpCurProc = lpCurProc + ProcessNameOffset;
		sprintf(msg, "%s->Created->Process = %s->PID = %d->TID = %d\n",
			_KMDFThread_H,
			(char*)lpCurProc,
			(int)PId,
			(int)TId
		);
		DbgPrint(msg);
	}
	else
	{
		sprintf(msg, "%s->Terminated->TID = %d\n", _KMDFThread_H, (int)TId);
		DbgPrint(msg);
	}
	return;
}

VOID ImageCreateMon(IN PUNICODE_STRING FullImageName, IN HANDLE ProcessId, IN PIMAGE_INFO ImageInfo)
{
	sprintf(msg, "%s->ImageCreate->(FullImageName = %S->PID = %d)->(ImageBase = %x->ImageSize = %d)",
		_KMDFThread_H,
		FullImageName->Buffer,
		(int)ProcessId,
		(ULONG)(ImageInfo->ImageBase),
		(int)ImageInfo->ImageSize
	);
	DbgPrint(msg);
	return;
}

ULONG GetProcessNameOffset()
{
	PEPROCESS curproc = PsGetCurrentProcess();
	for (int i = 0; i < 3 * PAGE_SIZE; ++i)
		if (!strncmp(SYSNAME, (PCHAR)curproc + i, strlen(SYSNAME)))
			return i;
	return 0;//找不到进程
}



/** 驱动出入口函数 **/
/* 驱动停止 */
VOID DriverUnload(IN PDRIVER_OBJECT DriverObject)
{
	strcpy_s(msg, sizeof(msg), "STOP");
	UNICODE_STRING linkString;
	//PsRemoveLoadImageNotifyRoutine(ImageCreateMon);  
	PsRemoveCreateThreadNotifyRoutine(ThreadCreateMon);
	PsSetCreateProcessNotifyRoutine(ProcessCreateMon, TRUE);
	RtlInitUnicodeString(&linkString, DosDeviceName);
	IoDeleteSymbolicLink(&linkString);
	IoDeleteDevice(DriverObject->DeviceObject);
	DbgPrint("\n%s->DriverUnload()\n", _KMDFThread_H);//不许 sprintf
	return;
}

/* 驱动启动 */
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	DbgPrint("\n%s->DriverEntry()\n", _KMDFThread_H);
	UNREFERENCED_PARAMETER(RegistryPath);
	NTSTATUS status = STATUS_SUCCESS;

	PDEVICE_OBJECT deviceObject;
	UNICODE_STRING nameString, linkString;
	RtlInitUnicodeString(&nameString, DeviceName);
	RtlInitUnicodeString(&linkString, DosDeviceName);
	status = IoCreateDevice(DriverObject,
		0,
		&nameString,
		FILE_DEVICE_UNKNOWN,
		0,
		TRUE,
		&deviceObject
	);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("%s->IoCreateDevice Failed!\n", _KMDFThread_H);
		return STATUS_SUCCESS;// return ntstatus;
	}
	status = IoCreateSymbolicLink(&linkString, &nameString);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(DriverObject->DeviceObject);
		return status;
	}
	ProcessNameOffset = GetProcessNameOffset();
	if (ProcessNameOffset == 0)
	{
		IoDeleteDevice(DriverObject->DeviceObject);
		return STATUS_UNSUCCESSFUL;
	}
	
	/*
	status = PsSetLoadImageNotifyRoutine(ImageCreateMon);  
	if (!NT_SUCCESS( status ))  
	{  
		IoDeleteDevice(DriverObject->DeviceObject);  
		DbgPrint(msg, "\n%s->PsSetLoadImageNotifyRoutine()->Failed!\n", _KMDFThread_H);
		return status;  
	}
	*/

	status = PsSetCreateThreadNotifyRoutine(ThreadCreateMon);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(DriverObject->DeviceObject);
		DbgPrint("\n%s->PsSetCreateThreadNotifyRoutine()->Failed!\n", _KMDFThread_H);
		return status;
	}

	/*
	status = PsSetCreateProcessNotifyRoutine(ProcessCreateMon, FALSE);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(DriverObject->DeviceObject);
		DbgPrint("\n%s->PsSetCreateProcessNotifyRoutine()\n", _KMDFThread_H);
		return status;
	}
	*/

	//for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
	//	DriverObject->MajorFunction[i] = CommonDispatch;

	deviceObject->Flags |= DO_BUFFERED_IO;

	DriverObject->DriverUnload = DriverUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = DeviceCreate;//创建
	DriverObject->MajorFunction[IRP_MJ_READ] = DeviceRead;//读
	DriverObject->MajorFunction[IRP_MJ_WRITE] = DeviceWrite;//写
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DeviceClose;//关闭
	DriverObject->MajorFunction[IRP_MJ_MAXIMUM_FUNCTION - 1] = CommonDispatch;

	return STATUS_SUCCESS;
}