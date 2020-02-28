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
	PKMUTEX pMutex = (PKMUTEX)Context;
	KdPrint(("Thread 1 Start!\n"));
	KeWaitForSingleObject(
		pMutex,		//等待的互斥量
		Executive,	//等待原因为执行
		KernelMode,	//驱动内应设为KernelMode
		FALSE,		//不警告
		NULL		//不设置超时时限
		);
	KdPrint(("Thread 1 Wait!\n"));
	MySleep(5000);
	KeReleaseMutex(
		pMutex,	//待释放的信号量
		FALSE	//不需要后接KeWaitXXX类函数
		);

	KdPrint(("Thread 1 End!\n"));
	PsTerminateSystemThread(STATUS_SUCCESS);
}

VOID ThreadProc2(PVOID Context)
{
	PKMUTEX pMutex = (PKMUTEX)Context;
	KdPrint(("Thread 2 Start!\n"));
	KeWaitForSingleObject(
		pMutex,		//等待的互斥量
		Executive,	//等待原因为执行
		KernelMode,	//驱动内应设为KernelMode
		FALSE,		//不警告
		NULL		//不设置超时时限
		);
	KdPrint(("Thread 2 Wait!\n"));
	MySleep(5000);
	KeReleaseMutex(
		pMutex,	//待释放的信号量
		FALSE	//不需要后接KeWaitXXX类函数
		);

	KdPrint(("Thread 2 End!\n"));
	PsTerminateSystemThread(STATUS_SUCCESS);

}

VOID MutexTest()
{
	HANDLE hThread[2];
	PETHREAD thread[2];
	KMUTEX mutex;
	KeInitializeMutex(
		&mutex,	//待初始化的互斥量
		0		//保留，驱动应填为0
		);

	PsCreateSystemThread(
		&hThread[0],
		THREAD_ALL_ACCESS,
		NULL,
		NULL,
		NULL,
		ThreadProc1,
		&mutex
		);
	PsCreateSystemThread(
		&hThread[1],
		THREAD_ALL_ACCESS,
		NULL,
		NULL,
		NULL,
		ThreadProc2,
		&mutex
		);
	
	//根据句柄获取线程对象
	ObReferenceObjectByHandle(
		hThread[0],
		THREAD_ALL_ACCESS,
		*PsThreadType,
		KernelMode,
		&thread[0],
		NULL);
	ObReferenceObjectByHandle(
		hThread[1],
		THREAD_ALL_ACCESS,
		*PsThreadType,
		KernelMode,
		&thread[1],
		NULL);

	KeWaitForMultipleObjects(
		2,			//等待资源对象数目 
		thread,		//等待资源对象数组
		WaitAll,	//等待方式为全部等待
		Executive,	//等待原因为执行
		KernelMode,	//驱动因填为KernelMode
		FALSE,		//不警告
		NULL,		//不设定超时时限
		NULL		//不设定等待块
		);

	//返还对象
	ObDereferenceObject(thread[0]);
	ObDereferenceObject(thread[1]);

}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = Unload;
	KdPrint(("Hello Driver!\n"));

	MutexTest();



	return STATUS_SUCCESS;
}