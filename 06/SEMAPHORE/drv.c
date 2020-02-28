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

VOID ThreadProc(PVOID Context)
{
	PKSEMAPHORE pSpr = (PKSEMAPHORE)Context;
	KdPrint(("Create Thread\n"));
	MySleep(10000);
	KeReleaseSemaphore(
		pSpr,			//待释放的信号量
		IO_NO_INCREMENT,//释放时的特权级,此处无需等待
		1,				//表示释放时信号量计数+1
		FALSE			//FALSE表示函数执行后,无需立即执行KeWaitxxx类的函数
		);
	KdPrint(("Semaphore Released,Count+1!\n"));
	KdPrint(("End Thread\n"));
	PsTerminateSystemThread(STATUS_SUCCESS);
}

VOID SemaphoreTest()
{
	HANDLE hThread;
	KSEMAPHORE spr;
	//初始化信号量,计数为2,最大计数为2
	KeInitializeSemaphore(&spr, 2, 2);

	KdPrint(("Wait 1\n"));
	KeWaitForSingleObject(&spr, Executive, KernelMode, FALSE, NULL);
	//读取当前信号计数并调试输出
	KdPrint(("Count:%d\n", KeReadStateSemaphore(&spr)));

	KdPrint(("Wait 2\n"));
	KeWaitForSingleObject(&spr, Executive, KernelMode, FALSE, NULL);
	KdPrint(("Count:%d\n", KeReadStateSemaphore(&spr)));

	PsCreateSystemThread(
		&hThread,
		THREAD_ALL_ACCESS,
		NULL,
		NULL,
		NULL,
		ThreadProc,
		&spr
		);

	KdPrint(("Wait 3\n"));
	KeWaitForSingleObject(&spr, Executive, KernelMode, FALSE, NULL);
	KdPrint(("Count:%d\n", KeReadStateSemaphore(&spr)));

}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = Unload;
	KdPrint(("Hello Driver!\n"));

	SemaphoreTest();

	return STATUS_SUCCESS;
}