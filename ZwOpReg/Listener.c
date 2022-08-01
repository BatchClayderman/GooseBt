#include <ntifs.h>
#include <ntstrsafe.h>
#include "Listener.h"
#ifndef _ZwOpReg_H
#define _ZwOpReg_H "ZwOpReg"
#ifndef NULL
#define NULL ((void *)0)
#endif
#ifndef VOID
#defnie VOID void
#endif
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#ifdef DeviceName
#undef DeviceName
#endif
#define DeviceName L"\\Device\\GooseBtZwOpReg"
#ifdef CWK_CDO_SYB_NAME
#undef CWK_CDO_SYB_NAME
#endif
#define CWK_CDO_SYB_NAME  L"\\??\\GooseBtZwOpReg"//在 R0 层的链接符号

/* 从应用层给驱动发送一个字符串 */
#ifndef CWK_DVC_SEND_STR
#define CWK_DVC_SEND_STR       \
	(ULONG)CTL_CODE(           \
	FILE_DEVICE_UNKNOWN,       \
	0x911,METHOD_BUFFERED,     \
	FILE_WRITE_DATA            \
)
#endif

/* 从驱动读取一个字符串 */
#ifndef CWK_DVC_RECV_STR
#define CWK_DVC_RECV_STR       \
	(ULONG)CTL_CODE(           \
	FILE_DEVICE_UNKNOWN,       \
	0x912,METHOD_BUFFERED,     \
	FILE_READ_DATA             \
)
#endif
#endif//_ZwOpReg_H
PDEVICE_OBJECT g_cdo = NULL;//控制设备


/** 通信函数 **/
NTSTATUS cwkDispatch(IN PDEVICE_OBJECT dev, IN PIRP irp)//分发函数
{
	/* 获取当前 irp 栈空间 */
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(irp);
	NTSTATUS status = STATUS_SUCCESS;
	ULONG ret_len = 0;//请求长度

	/* 循环处理发送给 g_cdo 的请求，如果不是发送给 g_cdo，则明显错误 */
	while (dev == g_cdo)
	{
		if (irpsp->MajorFunction == IRP_MJ_CREATE || irpsp->MajorFunction == IRP_MJ_CLOSE)// 生成和关闭请求，一律跳过即可
			break;
		if (irpsp->MajorFunction == IRP_MJ_DEVICE_CONTROL)
		{
			/* 处理 DeviceIoControl */
			PVOID buffer = irp->AssociatedIrp.SystemBuffer;//缓冲区地址
			ULONG inlen = irpsp->Parameters.DeviceIoControl.InputBufferLength;//输入缓冲区长度
			ULONG outlen = irpsp->Parameters.DeviceIoControl.OutputBufferLength;//输出缓冲区长度

			/* 根据前面定义好的设备请求功能号，详细判断输入还是输出，我们在这里只判断输入 */
			switch (irpsp->Parameters.DeviceIoControl.IoControlCode)
			{
			case CWK_DVC_SEND_STR:
				ASSERT(buffer != NULL && inlen < MAX_PATH&& inlen > 0 && 0 == outlen);
				if (NULL == buffer || inlen >= MAX_PATH || inlen <= 0 || 0 != outlen)
				{
					DbgPrint("%s->ASSERT(buffer != NULL && inlen < MAX_PATH && inlen > 0 && 0 == outlen)", _ZwOpReg_H);
					break;
				}
				short int choice = 0;//请求方案
				char bufIN[MAX_PATH] = { 0 };
				strcpy_s(bufIN, sizeof(bufIN) / sizeof(char), (char*)buffer);
				if (strlen(bufIN) > 4//防止出错
					&& (bufIN[0] == 'A' || bufIN[0] == 'a')
					&& (bufIN[1] == 'D' || bufIN[1] == 'd')
					&& (bufIN[2] == 'D' || bufIN[2] == 'd')
					&& (bufIN[3] == ' ')
					)// add 指令
				{
					choice = 1;//设置 /add
					strcpy_s(bufIN, sizeof(bufIN) / sizeof(char), &bufIN[4]);
					DbgPrint("%s->GetWmirMsg()->add(\"%s\")\n", _ZwOpReg_H, bufIN);
				}
				else if (strlen(bufIN) > 4//防止出错
					&& (bufIN[0] == 'D' || bufIN[0] == 'd')
					&& (bufIN[1] == 'E' || bufIN[1] == 'e')
					&& (bufIN[2] == 'L' || bufIN[2] == 'l')
					&& (bufIN[3] == ' ')
					)// del 指令
				{
					choice = 2;//设置 del
					strcpy_s(bufIN, sizeof(bufIN) / sizeof(char), &bufIN[4]);
					DbgPrint("%s->GetWmirMsg()->del(\"%s\")\n", _ZwOpReg_H, bufIN);
				}
				else if (strlen(bufIN) > 4//防止出错
					&& (bufIN[0] == 'S' || bufIN[0] == 's')
					&& (bufIN[1] == 'E' || bufIN[1] == 'e')
					&& (bufIN[2] == 'T' || bufIN[2] == 't')
					&& (bufIN[3] == ' ')
					)// set 指令
				{
					choice = 3;//设置 set
					strcpy_s(bufIN, sizeof(bufIN) / sizeof(char), &bufIN[4]);
					DbgPrint("%s->GetWmirMsg()->set(\"%s\")\n", _ZwOpReg_H, bufIN);
				}
				else if (strlen(bufIN) > 6//防止出错
					&& (bufIN[0] == 'Q' || bufIN[0] == 'q')
					&& (bufIN[1] == 'U' || bufIN[1] == 'u')
					&& (bufIN[2] == 'E' || bufIN[2] == 'e')
					&& (bufIN[3] == 'R' || bufIN[3] == 'r')
					&& (bufIN[4] == 'Y' || bufIN[4] == 'y')
					&& (bufIN[5] == ' ')
					)// query 指令
				{
					choice = 4;//设置 query
					strcpy_s(bufIN, sizeof(bufIN) / sizeof(char), &bufIN[4]);
					DbgPrint("%s->GetWmirMsg()->query\"%s\")\n", _ZwOpReg_H, bufIN);
				}
				else
					DbgPrint("%s->UnknownMsg->\"%s\"\n", _ZwOpReg_H, bufIN);//无效指令充当接收信息
				switch (choice)
				{
				case 1://执行 add
					break;
				case 2://执行 del
					break;
				case 3://执行 set
					break;
				case 4://执行 query
					break;
				default:
					break;
				}
				break;
			case CWK_DVC_RECV_STR:
			default://到这里的请求都是不接收的（我们没设置接收）
				status = STATUS_INVALID_PARAMETER;//未知的请求一律返回非法参数错误
				break;
			}
		}
		break;
	}

	/* 不是发送给 g_cdo，明显错误 */
	irp->IoStatus.Information = ret_len;//填写返回信息
	irp->IoStatus.Status = status;//填写返回结果
	IoCompleteRequest(irp, IO_NO_INCREMENT);//完成请求
	return status;
}

VOID listenerUnload()//清除通讯符号
{
	UNICODE_STRING cdo_syb = RTL_CONSTANT_STRING(CWK_CDO_SYB_NAME);//符号链接字符串
	ASSERT(g_cdo != NULL);
	IoDeleteSymbolicLink(&cdo_syb);//删除符号链接
	IoDeleteDevice(g_cdo);//删除控制设备
	return;
}

NTSTATUS listenerEntry(PDRIVER_OBJECT pDriver)
{
	/* 生成一个控制设备然后生成符号链接 */
	//UNICODE_STRING sddl = RTL_CONSTANT_STRING(L"D:P(A;;GA;;;WD)");//通用（直接复制即可）
	UNICODE_STRING cdo_name = RTL_CONSTANT_STRING(DeviceName);//设备名
	UNICODE_STRING cdo_syb = RTL_CONSTANT_STRING(CWK_CDO_SYB_NAME);//符号链接

	/* 生成一个设备对象 */
	NTSTATUS status = IoCreateDevice(
		pDriver,
		0, &cdo_name,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&g_cdo
	);
	if (!NT_SUCCESS(status))
		return status;

	/* 生成链接符号并与设备绑定 */
	IoDeleteSymbolicLink(&cdo_syb);//为防止冲突先预先删除
	status = IoCreateSymbolicLink(&cdo_syb, &cdo_name);//将链接符号与设备绑定
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(g_cdo);//删除设备
		DbgPrint(_ZwOpReg_H);
		DbgPrint("->IoCreateDevice()->Failed!\n");
		return status;
	}

	/* 将所有的分发函数设置为自定义的 */
	for (ULONG i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
		pDriver->MajorFunction[i] = cwkDispatch;
	DbgPrint(_ZwOpReg_H);
	DbgPrint("->IoCreateDevice()->Successful!\n");

	/* 清除控制设备的初始化标记 */
	g_cdo->Flags &= ~DO_DEVICE_INITIALIZING;
	return STATUS_SUCCESS;
}