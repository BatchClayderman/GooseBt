#pragma  once
#include <ntifs.h>
#include <ntddk.h>

PVOID g_pCBRegistrationHandle = NULL;

#define OB_PROCESS_TERMINATE         0x0001  
#define OB_PROCESS_VM_OPERATION      0x0008  
#define OB_PROCESS_VM_READ           0x0010  
#define OB_PROCESS_VM_WRITE          0x0020 

OB_CALLBACK_REGISTRATION  CBObRegistration = { 0 };
OB_OPERATION_REGISTRATION CBOperationRegistrations[2] = { { 0 }, { 0 } };
UNICODE_STRING CBAltitude = { 0 };

BOOLEAN RealIsAllowTerminateProcess(HANDLE pid)
{
	UNREFERENCED_PARAMETER(pid);
	BOOLEAN ret = FALSE;
	return ret;
}

OB_PREOP_CALLBACK_STATUS CBTdPreOperationCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION pOperationInformation)
{
	HANDLE pid = NULL;

	UNREFERENCED_PARAMETER(RegistrationContext);

	if (pOperationInformation->ObjectType == *PsProcessType)
		pid = PsGetProcessId((PEPROCESS)pOperationInformation->Object);
	else if (pOperationInformation->ObjectType == *PsThreadType)
		pid = PsGetThreadProcessId((PETHREAD)pOperationInformation->Object);

	pid = (HANDLE)6380;
	DbgPrint("6380\n");
	if (pid != 0)
	{
		if (!RealIsAllowTerminateProcess(pid))
		{
			if (pOperationInformation->Operation == OB_OPERATION_HANDLE_CREATE)
			{
				if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & OB_PROCESS_TERMINATE) == OB_PROCESS_TERMINATE)
				{
					pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~OB_PROCESS_TERMINATE;
				}
				if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & OB_PROCESS_VM_OPERATION) == OB_PROCESS_VM_OPERATION)
				{
					pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~OB_PROCESS_VM_OPERATION;
				}
				if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & OB_PROCESS_VM_READ) == OB_PROCESS_VM_READ)
				{
					pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~OB_PROCESS_VM_READ;
				}
				if ((pOperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & OB_PROCESS_VM_WRITE) == OB_PROCESS_VM_WRITE)
				{
					pOperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~OB_PROCESS_VM_WRITE;
				}
			}
		}
	}

	return OB_PREOP_SUCCESS;
}

VOID CBTdPostOperationCallback(PVOID RegistrationContext, POB_POST_OPERATION_INFORMATION PostInfo)
{
	UNREFERENCED_PARAMETER(RegistrationContext);
	UNREFERENCED_PARAMETER(PostInfo);

	return;
}

NTSTATUS ObRegisterProtectProcess()
{
	NTSTATUS Status = STATUS_SUCCESS;
	ULONG i = 0;

	CBOperationRegistrations[0].ObjectType = PsProcessType;
	CBOperationRegistrations[0].Operations |= OB_OPERATION_HANDLE_CREATE;
	CBOperationRegistrations[0].Operations |= OB_OPERATION_HANDLE_DUPLICATE;
	CBOperationRegistrations[0].PreOperation = CBTdPreOperationCallback;
	CBOperationRegistrations[0].PostOperation = CBTdPostOperationCallback;

	CBOperationRegistrations[1].ObjectType = PsThreadType;
	CBOperationRegistrations[1].Operations |= OB_OPERATION_HANDLE_CREATE;
	CBOperationRegistrations[1].Operations |= OB_OPERATION_HANDLE_DUPLICATE;
	CBOperationRegistrations[1].PreOperation = CBTdPreOperationCallback;
	CBOperationRegistrations[1].PostOperation = CBTdPostOperationCallback;

	RtlInitUnicodeString(&CBAltitude, L"1000");

	CBObRegistration.Version = ObGetFilterVersion();
	CBObRegistration.OperationRegistrationCount = 2;
	CBObRegistration.Altitude = CBAltitude;
	CBObRegistration.RegistrationContext = NULL;
	CBObRegistration.OperationRegistration = CBOperationRegistrations;

	for (i = 0; i < 3; i++)
	{
		Status = ObRegisterCallbacks(&CBObRegistration, &g_pCBRegistrationHandle);
		if (NT_SUCCESS(Status))
			break;
	}

	return Status;
}

VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	DbgPrint("goodbye!");
	return;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgPrint("DriverEnty()\n");
	ObRegisterProtectProcess();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}
