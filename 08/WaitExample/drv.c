#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{

	KdPrint(("Goodbye Driver!\n"));
}

//暂停传入的微秒数
VOID Wait1(ULONG ulMSeconds)
{
	KEVENT Event;
	LARGE_INTEGER timeout = RtlConvertLongToLargeInteger(-10 * ulMSeconds);
	KeInitializeEvent(
		&Event,	//待初始化的事件对象
		SynchronizationEvent,	//同步事件
		FALSE	//可直接被KeSetEvent()配置，无需等待KeWaitXXX类例程
		);	
	KdPrint(("Wait1 Started!\n"));
	KeWaitForSingleObject(
		&Event,		//事件对象
		Executive,	//等待原因为执行
		KernelMode,	//驱动对象应填为内核态
		FALSE,		//不警告
		&timeout	//等待时间
		);
	KdPrint(("Wait1 Finished!\n"));
}

//暂停传入的微秒数
VOID Wait2(ULONG ulMSeconds)
{
	KTIMER timer;
	LARGE_INTEGER timeout = RtlConvertLongToLargeInteger(-10 * ulMSeconds);
	KeInitializeTimer(&timer);

	KeSetTimer(
		&timer,		//代配制定时器
		timeout,	//定时间隔
		NULL		//不附加在任何DPC上
		);

	KdPrint(("Wait2 Started!\n"));
	KeWaitForSingleObject(
		&timer,		//定时器对象
		Executive,	//等待原因为执行
		KernelMode,	//驱动对象应填为内核态
		FALSE,		//不警告
		&timeout	//等待时间
		);
	KdPrint(("Wait2 Finished!\n"));
}

//暂停传入的微秒数
VOID Wait3(ULONG ulMSeconds)
{
	LARGE_INTEGER timeout = RtlConvertLongToLargeInteger(-10 * ulMSeconds);
	KdPrint(("Wait3 Started!\n"));
	KeDelayExecutionThread(
		KernelMode,	//内核模式
		FALSE,		//不警告
		&timeout	//等待时延
		);
	KdPrint(("Wait3 Started!\n"));
}

NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;

	Wait1(1000 * 1000);
	Wait2(2000 * 1000);
	Wait3(3000 * 1000);
	return STATUS_SUCCESS;
}