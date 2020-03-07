#include<ntddk.h>

KDPC dpc;		//全局DPC
KTIMER timer;	//定时器对象
LARGE_INTEGER timeout;//定时周期

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	//取消计时器
	KeCancelTimer(&timer);
	KdPrint(("Goodbye Driver!\n"));
}

VOID DpcRoutine(
	_In_     struct _KDPC *Dpc,
	_In_opt_ PVOID        DeferredContext,
	_In_opt_ PVOID        SystemArgument1,
	_In_opt_ PVOID        SystemArgument2
	)
{
	KdPrint(("DPC Tik!\n"));

	//使定时器能够重复定时
	KeSetTimer(&timer, timeout, &dpc);
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;
	
	//初始化定时器
	KeInitializeTimer(&timer);

	//设定定时间隔，-10 * (1000*1000),括号内为1000ms
	timeout = RtlConvertLongToLargeInteger(-10 * 1000 * 1000);
	
	//初始化DPC
	KeInitializeDpc(
		&dpc,		//待初始化的dpc
		DpcRoutine,	//定时触发的例程
		NULL		//上下文，纯地址
		);

	//启动DPC所在的定时器
	KeSetTimer(
		&timer,		//启动的定时器
		timeout,	//定时间隔
		&dpc		//定时器所在的DPC
		);	

	return STATUS_SUCCESS;
}