#include<ntddk.h>

#define Delay_One_MicroSecond (-10)
#define Delay_One_MilliSecond (Delay_One_MicroSecond * 1000)
VOID MySleep(LONG msec)
{
	LARGE_INTEGER li;
	li.QuadPart = Delay_One_MilliSecond;
	li.QuadPart *= msec;
	KeDelayExecutionThread(KernelMode, 0, &li);
}

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{

	KdPrint(("Goodbye Driver!\n"));
}

VOID ThreadProc1(PVOID Context)
{
	KdPrint(("Thread1 Start!\n"));
	MySleep(5000);
	
	KdPrint(("Thread1 End!\n"));
	PsTerminateSystemThread(
		//结束线程并返回STATUS_SUCCESS
		STATUS_SUCCESS);
}

VOID ThreadProc2(PVOID Context)
{
	KdPrint(("Thread2 Start!\n"));
	MySleep(1000);
	KdPrint(("Thread2 End!\n"));
	PsTerminateSystemThread(STATUS_SUCCESS);
}

VOID ThreadTest()
{
	HANDLE hThread1;
	HANDLE hThread2;

	PsCreateSystemThread(
		&hThread1,			//创建的线程句柄
		THREAD_ALL_ACCESS,	//线程访问权限
		NULL,				//对象属性
		NULL,				//待创建线程附属的进程句柄，NULL表示本进程
		NULL,				//客户ID
		ThreadProc1,		//线程启动例程
		NULL				//启动例程上下文，即一个PVOID型地址
		);
	PsCreateSystemThread(
		&hThread2,
		THREAD_ALL_ACCESS,
		NULL,
		NULL,
		NULL,
		ThreadProc2,
		NULL);
	KdPrint(("All Threads Created!\n"));
}

VOID ThreadProc3(PVOID Context)
{
	PKEVENT pEvent = (PKEVENT)Context;
	KdPrint(("Event Start!\n"));
	MySleep(10000);
	KeSetEvent(
		pEvent,		//待结束的事件对象
		IO_NO_INCREMENT,	//表明对象活动已正常结束
		FALSE		//指明该事件不必需在被KeWaitXXX类例
					//程调用后才能被KeSetEvent()调用
		);

	KdPrint(("Event End!\n"));
	PsTerminateSystemThread(STATUS_SUCCESS);
}

VOID EventTest()
{
	KEVENT Event;
	HANDLE hThread;

	KeInitializeEvent(
		&Event,				//待初始化的事件 
		NotificationEvent,	//待初始化的事件类型，此处为通知型事件
		FALSE				//指明该事件不必需在被KeWaitXXX类例
							//程调用后才能被KeSetEvent()调用
		);

	PsCreateSystemThread(
		&hThread,
		THREAD_ALL_ACCESS,
		NULL,
		NULL,
		NULL,
		ThreadProc3,
		&Event
		);
	KdPrint(("Thread Start!\n"));
	KeWaitForSingleObject(
		&Event,		//阻塞等待的响应对象
		Executive,	//等待原因，驱动应设为Executive
		KernelMode,	//等待模式，在驱动中应设为KernelMode
		FALSE,		//不告警
		NULL		//不设置超时时限
		);
	MySleep(10000);
	KdPrint(("Thread End!\n"));
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = Unload;
	KdPrint(("Hello Driver!\n"));

	EventTest();

	return STATUS_SUCCESS;
}