#include <ntddk.h>
#include <ntstrsafe.h>
#ifndef _KMDFSerial_H
#define _KMDFSerial_H "KMDFSerial"
#define DELAY_SECOND_CNT 5
#define DELAY_ONE_MICROSECOND (-10)
#define DELAY_ONE_MILLISECOND (DELAY_ONE_MICROSECOND  * 1000)
#define DELAY_ONE_SECOND (DELAY_ONE_MILLISECOND * 1000)
//#define NTSTRSAFE_LIB
//#define NTSTRSAFE_LIB "ntstrsafe.lib"
//#pragma comment(lib, "ntstrsafe.lib")
#ifdef NTSTRSAFE_LIB
#undef NTSTRSAFE_LIB
#endif//防止编译失败
#ifndef SetFlag
#define SetFlag(_F,_SF) ((_F) |= (_SF))
#endif
#ifndef ClearFlag
#define ClearFlag(_F,_SF) ((_F) &= ~(_SF))
#endif
#ifndef CCP_MAX_COM_ID
#define CCP_MAX_COM_ID 32
#endif
#ifndef MAX_NAME_LEN
#define MAX_NAME_LEN 32
#endif
#endif//_KMDFSerial_H


/* 过滤设备和真实设备的全局变量 */
static PDEVICE_OBJECT s_fltobj[CCP_MAX_COM_ID] = { 0 };
static PDEVICE_OBJECT s_nextobj[CCP_MAX_COM_ID] = { 0 };


/* 打开一个端口设备 */
PDEVICE_OBJECT ccpOpenCom(ULONG id, NTSTATUS* status)
{
	UNICODE_STRING name_str;
	static WCHAR name[MAX_NAME_LEN] = { 0 };
	PFILE_OBJECT fileobj = NULL;
	PDEVICE_OBJECT devobj = NULL;

	memset(name, 0, sizeof(WCHAR) * 32);//输入字符串
	RtlStringCchPrintfW(name, 
		MAX_NAME_LEN, 
		L"\\Device\\Serial%d", 
		id
	);
	RtlInitUnicodeString(&name_str, name);

	/* 打开设备对象 */
	*status = IoGetDeviceObjectPointer(&name_str, FILE_ALL_ACCESS, &fileobj, &devobj);
	if (*status == STATUS_SUCCESS)
		ObDereferenceObject(fileobj);

	return devobj;
}

NTSTATUS ccpAttachDevice(PDRIVER_OBJECT driver, PDEVICE_OBJECT oldobj, PDEVICE_OBJECT* fltobj, PDEVICE_OBJECT* next)
{
	NTSTATUS status;
	PDEVICE_OBJECT topdev = NULL;

	/* 生成设备并绑定 */
	status = IoCreateDevice(driver,
		0,
		NULL,
		oldobj->DeviceType,
		0,
		FALSE,
		fltobj
	);

	if (status != STATUS_SUCCESS)
		return status;

	/* 拷贝重要标志位 */
	if (oldobj->Flags & DO_BUFFERED_IO)
		(*fltobj)->Flags |= DO_BUFFERED_IO;
	if (oldobj->Flags & DO_DIRECT_IO)
		(*fltobj)->Flags |= DO_DIRECT_IO;
	if (oldobj->Flags & DO_BUFFERED_IO)
		(*fltobj)->Flags |= DO_BUFFERED_IO;
	if (oldobj->Characteristics & FILE_DEVICE_SECURE_OPEN)
		(*fltobj)->Characteristics |= FILE_DEVICE_SECURE_OPEN;
	(*fltobj)->Flags |= DO_POWER_PAGABLE;
	
	/* 绑定一个设备到另一个设备上 */
	topdev = IoAttachDeviceToDeviceStack(*fltobj, oldobj);
	if (NULL == topdev)//绑定失败则销毁设备
	{
		IoDeleteDevice(*fltobj);
		*fltobj = NULL;
		status = STATUS_UNSUCCESSFUL;
		return status;
	}
	*next = topdev;

	/* 设置这个设备已经启动 */
	(*fltobj)->Flags = (*fltobj)->Flags & ~DO_DEVICE_INITIALIZING;
	return STATUS_SUCCESS;
}

/* 绑定所有的串口 */
VOID ccpAttachAllComs(PDRIVER_OBJECT driver)
{
	ULONG i;
	PDEVICE_OBJECT com_ob;
	NTSTATUS status;
	for (i = 0; i < CCP_MAX_COM_ID; ++i)
	{
		com_ob = ccpOpenCom(i, &status);//获得 object 引用
		if (NULL == com_ob)//获取失败
			continue;
		ccpAttachDevice(driver, com_ob, &s_fltobj[i], &s_nextobj[i]);//取消 object 引用
	}
	return;
}

NTSTATUS ccpDispatch(PDEVICE_OBJECT device, PIRP irp)
{
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(irp);

	for (ULONG i = 0; i < CCP_MAX_COM_ID; ++i)//首先得知道发送给了哪个设备。设备一共最多 CCP_MAX_COM_ID 个，是前面的代码保存好的，都在 s_fltobj 中
		if (s_fltobj[i] == device)
		{
			if (irpsp->MajorFunction == IRP_MJ_POWER)//所有电源操作全部直接放过
			{
				PoStartNextPowerIrp(irp);//直接发送然后返回说已经被处理了
				IoSkipCurrentIrpStackLocation(irp);
				return PoCallDriver(s_nextobj[i], irp);
			}

			if (irpsp->MajorFunction == IRP_MJ_WRITE)//仅显示写请求
			{
				ULONG len = irpsp->Parameters.Write.Length;//先获得长度
				PUCHAR buf = NULL;
				if (irp->MdlAddress != NULL)//然后获得缓冲区
					buf = (PUCHAR)MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
				else
					buf = (PUCHAR)irp->UserBuffer;
				if (NULL == buf)
					buf = (PUCHAR)irp->AssociatedIrp.SystemBuffer;
				if (NULL == buf)
				{
					DbgPrint("%s->%wZ->Get buffer failed! \n", _KMDFSerial_H, device->DriverObject->DriverName);
					continue;
				}

				for (ULONG j = 0; j < len; ++j)//打印内容
					DbgPrint("%s->%wZ->Send Data->%2x\n", _KMDFSerial_H, device->DriverObject->DriverName, buf[j]);
			}

			IoSkipCurrentIrpStackLocation(irp);//仅作监控处理
			return IoCallDriver(s_nextobj[i], irp);
		}

	/* 如果根本就不在被绑定的设备中，那是有问题的，直接返回参数错误 */
	irp->IoStatus.Information = 0;
	irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}



/** 驱动出入口函数 **/
/* 驱动停止 */
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);//手动醒目
	LARGE_INTEGER interval;

	DbgPrint("%s->DriverUnload()->IoDetachDevice\n", _KMDFSerial_H);
	for (ULONG i = 0; i < CCP_MAX_COM_ID; ++i)//解除绑定
		if (s_nextobj[i] != NULL)//非空则解除绑定
			IoDetachDevice(s_nextobj[i]);

	DbgPrint("%s->DriverUnload()->WaitingforIrpEnding->%ds\n", _KMDFSerial_H, DELAY_SECOND_CNT);
	interval.QuadPart = (DELAY_SECOND_CNT * DELAY_ONE_SECOND);//等待所有 irp 处理结束
	KeDelayExecutionThread(KernelMode, FALSE, &interval);

	DbgPrint("%s->DriverUnload()->IoDeleteDevice\n", _KMDFSerial_H);
	for (ULONG i = 0; i < CCP_MAX_COM_ID; ++i)//删除这些设备
		if (s_fltobj[i] != NULL)//非空则解除绑定
			IoDeleteDevice(s_fltobj[i]);

	DbgPrint("%s->DriverUnload()\n", _KMDFSerial_H);
	return;
}

/* 驱动启动 */
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
	UNREFERENCED_PARAMETER(reg_path);//手动醒目
	DbgPrint("%s->DriverEntry()\n", _KMDFSerial_H);

	for (size_t i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)//所有派发函数都设置成一样的。
		driver->MajorFunction[i] = ccpDispatch;
	
	driver->DriverUnload = DriverUnload;//支持动态卸载
	ccpAttachAllComs(driver);//绑定所有的串口

	return STATUS_SUCCESS;
}