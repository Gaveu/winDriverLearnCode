#include<ntddk.h>

#define Delay_One_MicroSecond (-10)
#define Delay_One_MilliSecond (Delay_One_MicroSecond * 1000)
void MySleep(LONG msec)
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

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = Unload;
	KdPrint(("Hello Driver!\n"));

	ThreadTest();

	return STATUS_SUCCESS;
}