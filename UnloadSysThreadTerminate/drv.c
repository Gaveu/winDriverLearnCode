#include<ntddk.h>

/*该代码实现驱动卸载时终止系统线程的执行，防止蓝屏*/

//待处理线程的_ETHREAD指针和用于处理系统线程的事件对象
KEVENT Event;
PETHREAD pEthread;

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	NTSTATUS status;
	KeSetEvent(
		&Event,
		IO_NO_INCREMENT,
		FALSE
	);

	KdPrint(("Unload:ThreadWait!\n"));
	status = KeWaitForSingleObject(
		pEthread,
		Executive,
		KernelMode,
		FALSE,
		NULL
	);
	if (NT_SUCCESS(status))
	{
		KdPrint(("Unload:System Thread Exit!\n"));
	}
	else
	{
		KdPrint(("Unload:System Thread Still Running!\n"));
	}

	KdPrint(("Unload:Goodbye Driver!\n"));
}

//驱动启动后开始同步执行，在驱动卸载时被卸载函数结束运行
VOID ThreadProc(IN PVOID StartContext)
{
	NTSTATUS status;
	LARGE_INTEGER timeout = RtlConvertLongToLargeInteger(-10 * 1000 * 1000);
	pEthread = PsGetCurrentThread();
	while (TRUE)
	{
		KdPrint(("ThreadProc:System Thread Running!\n"));
		status = KeWaitForSingleObject(
			&Event,
			Executive,
			KernelMode,
			FALSE,
			&timeout
		);
		if (STATUS_TIMEOUT == status)
		{
			continue;
		}
		else
		{
			KdPrint(("ThreadProc:Event Set!\n"));
			break;
		}
	}
	KdPrint(("ThreadProc:End!\n"));
	PsTerminateSystemThread(STATUS_SUCCESS);
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	HANDLE hThread;
	KdPrint(("DriverEntry:Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;
	KeInitializeEvent(
		&Event,
		SynchronizationEvent,
		FALSE
	);

	PsCreateSystemThread(
		&hThread,
		THREAD_ALL_ACCESS,
		NULL,
		NULL,
		NULL,
		ThreadProc,
		NULL
	);


	return STATUS_SUCCESS;
}