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
		pSpr,			//���ͷŵ��ź���
		IO_NO_INCREMENT,//�ͷ�ʱ����Ȩ��,�˴�����ȴ�
		1,				//��ʾ�ͷ�ʱ�ź�������+1
		FALSE			//FALSE��ʾ����ִ�к�,��������ִ��KeWaitxxx��ĺ���
		);
	KdPrint(("Semaphore Released,Count+1!\n"));
	KdPrint(("End Thread\n"));
	PsTerminateSystemThread(STATUS_SUCCESS);
}

VOID SemaphoreTest()
{
	HANDLE hThread;
	KSEMAPHORE spr;
	//��ʼ���ź���,����Ϊ2,������Ϊ2
	KeInitializeSemaphore(&spr, 2, 2);

	KdPrint(("Wait 1\n"));
	KeWaitForSingleObject(&spr, Executive, KernelMode, FALSE, NULL);
	//��ȡ��ǰ�źż������������
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