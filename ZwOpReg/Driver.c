#include <ntddk.h>
#include <ntstrsafe.h>
#ifndef _ZwOpReg_H
#define _ZwOpReg_H "ZwOpReg"
#endif

//************************************
// Method:    ntIBinaryCreateKey
// FullName:  ntIBinaryCreateKey
// Access:    public 
// Returns:   NTSTATUS
// Qualifier: 创建注册表键值
// Parameter: UNICODE_STRING uPathKeyName
//************************************


/** 注册表 **/
/* 创建 */
NTSTATUS ntIBinaryCreateKey(UNICODE_STRING uPathKeyName)
{
	NTSTATUS status = STATUS_ERROR_PROCESS_NOT_IN_JOB;
	OBJECT_ATTRIBUTES objAttri;
	HANDLE hKeyHandle;
	UNICODE_STRING uSubKey;
	HANDLE hSubKey;
	OBJECT_ATTRIBUTES objSubAttri;
	ULONG isRegStatus;//注册表的状态——传出
	InitializeObjectAttributes(
		&objAttri,
		&uPathKeyName,
		OBJ_CASE_INSENSITIVE,//句柄只能内核访问而且只能一个打开
		NULL,
		NULL
	);
	status = ZwCreateKey(&hKeyHandle,
		KEY_ALL_ACCESS,
		&objAttri,
		0,
		NULL,
		REG_OPTION_BACKUP_RESTORE,
		(PULONG)(&isRegStatus)
	);
	if (!NT_SUCCESS(status))
	{
		ZwClose(hKeyHandle);
		return status;
	}
	RtlUnicodeStringInit(&uSubKey, L"MyReg");//创建子Key
	/*
	InitializeObjectAttributes(p, n, a, r, s)        \
	{                                                \
		(p)->Length = sizeof(OBJECT_ATTRIBUTES);     \
		(p)->RootDirectory = r;                      \
		(p)->Attributes = a;                         \
		(p)->ObjectName = n;                         \
		(p)->SecurityDescriptor = s;                 \
		(p)->SecurityQualityOfService = NULL;        \
	}
	*/
	//InitializeObjectAttributes(&objAttri, &uSubKey, OBJ_CASE_INSENSITIVE, hKeyHandle, NULL);
	/* 不使用宏而手工进行赋值 */
	objSubAttri.Length = sizeof(OBJECT_ATTRIBUTES);
	objSubAttri.Attributes = OBJ_CASE_INSENSITIVE;
	objSubAttri.ObjectName = &uSubKey;
	objSubAttri.SecurityDescriptor = NULL;
	objSubAttri.SecurityQualityOfService = NULL;
	objSubAttri.RootDirectory = hKeyHandle;//注意这里父目录设置为我们上面创建的 key
	status = ZwCreateKey(&hSubKey,//传出创建的 Key
		KEY_ALL_ACCESS,           //权限
		&objSubAttri,             //路径
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,  //创建的 Key 重启是否存在还是临时的
		&isRegStatus              //保存 Key 的状态——创建成功还是打开
	);
	if (!NT_SUCCESS(status))
	{
		ZwClose(hSubKey);
		ZwClose(hKeyHandle);
		return status;
	}
	ZwClose(hSubKey);
	ZwClose(hKeyHandle);
	DbgPrint(_ZwOpReg_H);
	KdPrint(("->Create Key Successfully!\n"));
	return status;
}

NTSTATUS ntIBinaryInit()
{
	NTSTATUS status = STATUS_ERROR_PROCESS_NOT_IN_JOB;
	UNICODE_STRING uKeyPath;
	RtlUnicodeStringInit(&uKeyPath, L"\\registry\\machine\\SoftWare\\IBinary");
	status = ntIBinaryCreateKey(uKeyPath);
	if (!NT_SUCCESS(status))
	{
		DbgPrint(_ZwOpReg_H);
		KdPrint(("->Create Key Failed!\n"));
	}
	return status;
}

/* 删除 */
NTSTATUS ntIBinaryDeleteKey(UNICODE_STRING uPathKeyName)
{
	NTSTATUS ntStatus = STATUS_FAILED_STACK_SWITCH;
	HANDLE hKey;
	OBJECT_ATTRIBUTES ObjAttr;
	ObjAttr.Length = sizeof(OBJECT_ATTRIBUTES);
	ObjAttr.Attributes = OBJ_CASE_INSENSITIVE;
	ObjAttr.ObjectName = &uPathKeyName;
	ObjAttr.RootDirectory = NULL;
	ObjAttr.SecurityDescriptor = NULL;
	ObjAttr.SecurityQualityOfService = NULL;
	__try
	{
		ntStatus = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &ObjAttr);//打开Key在进行删除
		if (!NT_SUCCESS(ntStatus))
		{
			ZwClose(hKey);
			return ntStatus;
		}
		ntStatus = ZwDeleteKey(hKey);
		if (!NT_SUCCESS(ntStatus))
		{
			ZwClose(hKey);
			return ntStatus;
		}
		DbgPrint(_ZwOpReg_H);
		KdPrint(("->Delete Key Successfully!\n"));
	}
	__except (GetExceptionCode())
	{
		DbgPrint(_ZwOpReg_H);
		KdPrint(("->Delete Key Successfully!\n"));
	}
	return ntStatus;
}

/* 查询 */
NTSTATUS ntIBinaryQueryKey(UNICODE_STRING uPathKeyName)
{
	NTSTATUS ntStatus = STATUS_FAILED_STACK_SWITCH;
	HANDLE hKey;
	OBJECT_ATTRIBUTES objAttri = { 0 };
	PKEY_FULL_INFORMATION pkfinfo = NULL;
	ULONG uSize = 0;
	ULONG iteratorValue = 0; //遍历的变量
	PKEY_BASIC_INFORMATION pBaseinfo = NULL;
	UNICODE_STRING uDbgValue = { 0 };//遍历出来的信息保存到 UNICODE_STRING 结构体中
	__try//首先打开 Key 然后遍历 Key
	{
		InitializeObjectAttributes(
			&objAttri,
			&uPathKeyName,
			OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
			NULL,
			NULL);
		ntStatus = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &objAttri);
		if (!NT_SUCCESS(ntStatus))
			return ntStatus;
		//遍历 Key 需要两次调用：第一次调用得出数据大小，第二次调用则是填充数据
		ntStatus = ZwQueryKey(hKey, KeyFullInformation, NULL, 0, &uSize);
		//得出 KEY_FUN_INFOMATION 结构的大小，进行内存申请即可
		//查询 MSDN 得出：ZwQuery 当数据不足会返回两个状态，所以判断一下即可
		//STATUS_BUFFER_OVERFLOW 或 STATUS_BUFFER_TOO_SMALL
		if (ntStatus != STATUS_BUFFER_OVERFLOW && ntStatus != STATUS_BUFFER_TOO_SMALL)
		{
			ZwClose(hKey);
			return ntStatus;
		}
		pkfinfo = (PKEY_FULL_INFORMATION)ExAllocatePool2(PagedPool, uSize, 'niBI');
		if (NULL == pkfinfo)
			pkfinfo = (PKEY_FULL_INFORMATION)ExAllocatePoolZero(PagedPool, uSize, 'niBI');
		if (NULL == pkfinfo)
		{
			ZwClose(hKey);
			return ntStatus;
		}
		//申请了 KEY_FULL_INFOMATION 结构数组大小，然后进行获取大小
		ntStatus = ZwQueryKey(hKey, KeyFullInformation, pkfinfo, uSize, &uSize);
		if (!NT_SUCCESS(ntStatus))
		{
			ExFreePoolWithTag(pkfinfo, 'niBI');
			ZwClose(hKey);
			return ntStatus;
		}
		for (iteratorValue = 0; iteratorValue < pkfinfo->SubKeys; iteratorValue++)
		{
			//遍历出 Key 就要进行枚举出 Key 的详细信息，使用 ZwEnumerateKey 即可，也是枚举一个结构
			ntStatus = ZwEnumerateKey(hKey,
				0,
				KeyBasicInformation,
				NULL,
				0,
				&uSize
			);
			if (ntStatus != STATUS_BUFFER_OVERFLOW && ntStatus != STATUS_BUFFER_TOO_SMALL)
			{
				ZwClose(hKey);
				return ntStatus;
			}
			pBaseinfo = (PKEY_BASIC_INFORMATION)ExAllocatePool2(PagedPool, uSize, 'niBI');
			if (NULL == pBaseinfo)
				pBaseinfo = (PKEY_BASIC_INFORMATION)ExAllocatePoolZero(PagedPool, uSize, 'niBI');
			if (NULL == pkfinfo)
			{
				ZwClose(hKey);
				return ntStatus;
			}
			ntStatus = ZwEnumerateKey(hKey,//继续申请一次得出需要的
				0,
				KeyBasicInformation,
				pBaseinfo,
				uSize,
				&uSize
			);
			if (!NT_SUCCESS(ntStatus))
			{
				if (NULL != pBaseinfo)
					ExFreePoolWithTag(pBaseinfo, 'niBI');
				if (NULL != pkfinfo)
					ExFreePoolWithTag(pkfinfo, 'niBI');
				ZwClose(hKey);
				return ntStatus;
			}
			//得出信息则可以进行进一步操作了：初始化 UNICODE 结构进行打印输出即可
			if (!pBaseinfo)
				return ntStatus;
			uDbgValue.Length = (USHORT)pBaseinfo->NameLength;
			uDbgValue.MaximumLength = (USHORT)pBaseinfo->NameLength;
			uDbgValue.Buffer = pBaseinfo->Name;
			DbgPrint(_ZwOpReg_H);
			KdPrint(("->Key = %wZ\n", &uDbgValue));
			ExFreePool(pBaseinfo); //同上释放内存
		}
		if (NULL != pkfinfo)//释放资源
			ExFreePool(pkfinfo);
		ZwClose(hKey);
	}
	__except (GetExceptionCode())
	{
		DbgPrint(_ZwOpReg_H);
		KdPrint(("->ntIBinaryQueryKey()->Error:%ld\n", GetExceptionCode()));
	}
	return ntStatus;
}

/* 修改 */
NTSTATUS ntIBinarySetKeyValue(UNICODE_STRING uPathKeyName)
{
	NTSTATUS ntStatus;
	OBJECT_ATTRIBUTES objAttri;
	HANDLE hKey;
	UNICODE_STRING uSetValueKeyName;
	ULONG Value = 10;
	InitializeObjectAttributes(&objAttri,
		&uPathKeyName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		NULL
	);
	ntStatus = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &objAttri);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;
	RtlUnicodeStringInit(&uSetValueKeyName, L"IBinaryFrist");//设置 KEY value 的值
	ntStatus = ZwSetValueKey(hKey,
		&uSetValueKeyName,
		0,
		REG_DWORD,
		&Value,
		sizeof(ULONG));
	if (!NT_SUCCESS(ntStatus))
	{
		ZwClose(hKey);
		return ntStatus;
	}
	DbgPrint(_ZwOpReg_H);
	KdPrint(("Set Key Successfully!\n"));
	ZwClose(hKey);
	return ntStatus;
}


/** 驱动出入口函数 **/
/* 驱动停止 */
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	DbgPrint("\n");
	DbgPrint(_ZwOpReg_H);
	DbgPrint("->DriverUnload()\n");
	return;
}

/* 驱动启动 */
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgPrint("\n");
	DbgPrint(_ZwOpReg_H);
	DbgPrint("->DriverEntry()\n");
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}