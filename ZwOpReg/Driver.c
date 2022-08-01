#include <ntddk.h>
#include <ntstrsafe.h>
#include "Listener.h"
#ifndef _ZwOpReg_H
#define _ZwOpReg_H "ZwOpReg"
#define GooseBtKeyPath L"\\registry\\machine\\SoftWare\\GooseBt"
#define ZwOpRegSubKey L"ZwOpReg"
#endif


/**/
/*
 * uPath：注册表路径
 * uSubKey：上述路径下的子项或子键
 * uType：类型
 * uValue：值
 * uSize：值类型大小
 * isSubFolder：指定是否为子项
 */


/** 注册表 **/
/* 创建 */
NTSTATUS ntIBinaryCreateKey(UNICODE_STRING uPath, UNICODE_STRING uSubKey)
{
	OBJECT_ATTRIBUTES attrPath, attrSubKey;
	HANDLE hPath, hSubKey;
	ULONG isRegStatus;//注册表的状态——传出
	InitializeObjectAttributes(
		&attrPath, 
		&uPath, 
		OBJ_CASE_INSENSITIVE,//句柄只能内核访问而且只能一个打开
		NULL, 
		NULL
	);
	NTSTATUS ntStatus = ZwCreateKey(&hPath,
		KEY_CREATE_SUB_KEY, 
		&attrPath, 
		0, 
		NULL, 
		REG_OPTION_BACKUP_RESTORE, 
		(PULONG)(&isRegStatus)
	);
	if (!NT_SUCCESS(ntStatus))
	{
		DbgPrint("%s->Create->uPath = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, ntStatus);
		ZwClose(hPath);
		return ntStatus;
	}

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
	InitializeObjectAttributes(&objAttri, &uSubKey, OBJ_CASE_INSENSITIVE, hKeyHandle, NULL);
	*/

	/* 不使用宏而手工进行赋值 */
	attrSubKey.Length = sizeof(OBJECT_ATTRIBUTES);
	attrSubKey.Attributes = OBJ_CASE_INSENSITIVE;
	attrSubKey.ObjectName = &uSubKey;
	attrSubKey.SecurityDescriptor = NULL;
	attrSubKey.SecurityQualityOfService = NULL;
	attrSubKey.RootDirectory = hPath;//注意这里父目录设置 uPath 的句柄
	ntStatus = ZwCreateKey(&hSubKey, //传出创建的 Key
		KEY_CREATE_SUB_KEY,        //权限（最小权限原则）
		&attrSubKey,               //路径
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,   //创建的 Key 重启是否存在还是临时的
		&isRegStatus               //保存 Key 的状态——创建成功还是打开
	);
	ZwClose(hSubKey);
	ZwClose(hPath);
	if (NT_SUCCESS(ntStatus))
		DbgPrint("%s->Create->uPath = \"%wZ\"->uSubKey = \"%wZ\"->Successful\n", _ZwOpReg_H, &uPath, &uSubKey);
	else
		DbgPrint("%s->Create->uPath = \"%wZ\"->uSubKey = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, &uSubKey, ntStatus);
	return ntStatus;
}

/* 删除 */
NTSTATUS ntIBinaryDeleteKey(UNICODE_STRING uPath, UNICODE_STRING uSubKey, BOOLEAN isSubFolder)
{
	NTSTATUS ntStatus = STATUS_FAILED_STACK_SWITCH;
	HANDLE hPath;
	OBJECT_ATTRIBUTES attrPath;
	attrPath.Length = sizeof(OBJECT_ATTRIBUTES);
	attrPath.Attributes = OBJ_CASE_INSENSITIVE;
	attrPath.ObjectName = &uPath;
	attrPath.RootDirectory = NULL;
	attrPath.SecurityDescriptor = NULL;
	attrPath.SecurityQualityOfService = NULL;
	__try
	{
		ntStatus = ZwOpenKey(&hPath, KEY_ALL_ACCESS, &attrPath);//打开 Key 再进行删除
		if (!NT_SUCCESS(ntStatus))
		{
			DbgPrint("%s->Delete->uPath = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, ntStatus);
			ZwClose(hPath);
			return ntStatus;
		}
		ntStatus = isSubFolder ? ZwDeleteKey(hPath) : ZwDeleteValueKey(hPath, &uSubKey);
		ZwClose(hPath);
		if (NT_SUCCESS(ntStatus))
			DbgPrint("%s->Delete->uPath = \"%wZ\"->uSubKey = \"%wZ\"->Successful\n", _ZwOpReg_H, &uPath, &uSubKey);
		else
			DbgPrint("%s->Delete->uPath = \"%wZ\"->uSubKey = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, &uSubKey, ntStatus);
	}
	__except (GetExceptionCode())
	{
		DbgPrint("%s->Delete->uPath = \"%wZ\"->uSubKey = \"%wZ\"->Unexpected(%d)\n", _ZwOpReg_H, &uPath, &uSubKey, GetExceptionCode());
	}
	return ntStatus;
}

/* 修改 */
NTSTATUS ntIBinarySetKeyValue(UNICODE_STRING uPath, UNICODE_STRING uSubKey, ULONG uType, PVOID uValue, ULONG uSize)
{
	OBJECT_ATTRIBUTES attrPath;
	HANDLE hPath;
	InitializeObjectAttributes(&attrPath, 
		&uPath, 
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 
		NULL, 
		NULL
	);
	NTSTATUS ntStatus = ZwOpenKey(&hPath, KEY_SET_VALUE, &attrPath);
	if (!NT_SUCCESS(ntStatus))
	{
		DbgPrint("%s->Set->uPath = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, ntStatus);
		return ntStatus;
	}
	ntStatus = ZwSetValueKey(hPath, 
		&uSubKey, 
		0, 
		uType, 
		uValue, 
		uSize
	);
	ZwClose(hPath);
	if (NT_SUCCESS(ntStatus))
		DbgPrint("%s->Set->uPath = \"%wZ\"->uSubKey = \"%wZ\"->Successful\n", _ZwOpReg_H, &uPath, &uSubKey);
	else
		DbgPrint("%s->Set->uPath = \"%wZ\"->uSubKey = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, &uSubKey, ntStatus);
	return ntStatus;
}

/* 查询 */
NTSTATUS ntIBinaryQueryKey(UNICODE_STRING uPath)
{
	NTSTATUS ntStatus = STATUS_FAILED_STACK_SWITCH;
	HANDLE hKey;
	OBJECT_ATTRIBUTES objAttri = { 0 };
	PKEY_FULL_INFORMATION pkfinfo = NULL;
	ULONG uSize = 0;
	PKEY_BASIC_INFORMATION pBaseinfo = NULL;
	UNICODE_STRING uDbgValue = { 0 };//遍历出来的信息保存到 UNICODE_STRING 结构体中
	__try//首先打开 Key 然后遍历 Key
	{
		InitializeObjectAttributes(
			&objAttri,
			&uPath,
			OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
			NULL,
			NULL
		);
		ntStatus = ZwOpenKey(&hKey, KEY_ENUMERATE_SUB_KEYS, &objAttri);
		if (!NT_SUCCESS(ntStatus))
		{
			DbgPrint("%s->Query->uPath = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, ntStatus);
			return ntStatus;
		}
		//遍历 Key 需要两次调用：第一次调用得出数据大小，第二次调用则是填充数据
		ntStatus = ZwQueryKey(hKey, KeyFullInformation, NULL, 0, &uSize);
		//得出 KEY_FUN_INFOMATION 结构的大小，进行内存申请即可
		//查询 MSDN 得出：ZwQuery 当数据不足会返回两个状态，所以判断一下即可
		
		/* STATUS_BUFFER_OVERFLOW 或 STATUS_BUFFER_TOO_SMALL */
		if (ntStatus != STATUS_BUFFER_OVERFLOW && ntStatus != STATUS_BUFFER_TOO_SMALL)
		{
			ZwClose(hKey);
			DbgPrint("%s->Query->uPath = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, ntStatus);
			return ntStatus;
		}
		pkfinfo = (PKEY_FULL_INFORMATION)ExAllocatePool2(PagedPool, uSize, 'niBI');
		if (NULL == pkfinfo)
			pkfinfo = (PKEY_FULL_INFORMATION)ExAllocatePoolZero(PagedPool, uSize, 'niBI');
		if (NULL == pkfinfo)
		{
			DbgPrint("%s->Query->uPath = \"%wZ\"->Failed(pkfinfo = %d)\n", _ZwOpReg_H, &uPath, STATUS_MEMORY_NOT_ALLOCATED);
			ZwClose(hKey);
			return STATUS_MEMORY_NOT_ALLOCATED;
		}
		
		/* 申请了 KEY_FULL_INFOMATION 结构数组大小，然后进行获取大小 */
		ntStatus = ZwQueryKey(hKey, KeyFullInformation, pkfinfo, uSize, &uSize);
		if (!NT_SUCCESS(ntStatus))
		{
			ExFreePoolWithTag(pkfinfo, 'niBI');
			ZwClose(hKey);
			return ntStatus;
		}
		for (ULONG iteratorValue = 0; iteratorValue < pkfinfo->SubKeys; ++iteratorValue)
		{
			ntStatus = ZwEnumerateKey(hKey,
				0,
				KeyBasicInformation,
				NULL,
				0,
				&uSize
			);//遍历出 Key 就要进行枚举出 Key 的详细信息，使用 ZwEnumerateKey 即可，也是枚举一个结构
			if (ntStatus != STATUS_BUFFER_OVERFLOW && ntStatus != STATUS_BUFFER_TOO_SMALL)
			{
				ZwClose(hKey);
				DbgPrint("%s->Query->uPath = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, ntStatus);
				return ntStatus;
			}
			pBaseinfo = (PKEY_BASIC_INFORMATION)ExAllocatePool2(PagedPool, uSize, 'niBI');
			if (NULL == pBaseinfo)
				pBaseinfo = (PKEY_BASIC_INFORMATION)ExAllocatePoolZero(PagedPool, uSize, 'niBI');
			if (NULL == pBaseinfo)
			{
				ZwClose(hKey);
				DbgPrint("%s->Query->uPath = \"%wZ\"->Failed(pBaseinfo = %d)\n", _ZwOpReg_H, &uPath, STATUS_MEMORY_NOT_ALLOCATED);
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
				DbgPrint("%s->Query->uPath = \"%wZ\"->Failed(%d)\n", _ZwOpReg_H, &uPath, ntStatus);
				return ntStatus;
			}

			/* 得出信息则可以进行进一步操作了：初始化 UNICODE 结构进行打印输出即可 */
			if (!pBaseinfo)
			{
				DbgPrint("%s->Query->uPath = \"%wZ\"->Failed(pBaseinfo = %d)\n", _ZwOpReg_H, &uPath, STATUS_MEMORY_NOT_ALLOCATED);
				return ntStatus;
			}
			uDbgValue.Length = (USHORT)pBaseinfo->NameLength;
			uDbgValue.MaximumLength = (USHORT)pBaseinfo->NameLength;
			uDbgValue.Buffer = pBaseinfo->Name;
			DbgPrint("%s->Query->uPath = \"%wZ\"->Key = \"%wZ\"->Successful\n", _ZwOpReg_H, &uPath, &uDbgValue);
			ExFreePool(pBaseinfo);//同上释放内存
		}
		if (NULL != pkfinfo)//释放资源
			ExFreePool(pkfinfo);
		ZwClose(hKey);
	}
	__except (GetExceptionCode())
	{
		DbgPrint("%s->Query->uPath = \"%wZ\"->Unexpected(%d)\n", _ZwOpReg_H, &uPath, GetExceptionCode());
	}
	return ntStatus;
}



/** 驱动出入口函数 **/
/* 初始化 */
NTSTATUS ntIBinaryInit()
{
	UNICODE_STRING uPath, uSubKey;
	RtlUnicodeStringInit(&uPath, GooseBtKeyPath);
	RtlUnicodeStringInit(&uSubKey, ZwOpRegSubKey);
	ULONG ul = 3;//驱动级
	NTSTATUS status = ntIBinarySetKeyValue(uPath, uSubKey, REG_DWORD, &ul, sizeof(ULONG));
	if (NT_SUCCESS(status))
		DbgPrint("%s->Initial->%ws->%ws->Successful\n", _ZwOpReg_H, GooseBtKeyPath, ZwOpRegSubKey);
	else
		DbgPrint("%s->Initial->%ws->%ws->Failed(%d)\n", _ZwOpReg_H, GooseBtKeyPath, ZwOpRegSubKey, status);
	return status;
}

/* 驱动停止 */
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	DbgPrint("\n%s->DriverUnload()\n", _ZwOpReg_H);
	listenerUnload();
	return;
}

/* 驱动启动 */
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgPrint("\n%s->DriverEntry()\n", _ZwOpReg_H);
	pDriver->DriverUnload = DriverUnload;
	NTSTATUS bRet = listenerEntry(pDriver);
	if (!NT_SUCCESS(bRet))
	{
		DbgPrint("%s->Initial->Listener->Failed! ", _ZwOpReg_H);
		return bRet;
	}
	UNICODE_STRING uPath, uSubKey;
	RtlInitUnicodeString(&uPath, GooseBtKeyPath);
	RtlInitUnicodeString(&uSubKey, ZwOpRegSubKey);
	return ntIBinaryInit();
}