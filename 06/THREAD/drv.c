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
		//�����̲߳�����STATUS_SUCCESS
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
		&hThread1,			//�������߳̾��
		THREAD_ALL_ACCESS,	//�̷߳���Ȩ��
		NULL,				//��������
		NULL,				//�������̸߳����Ľ��̾����NULL��ʾ������
		NULL,				//�ͻ�ID
		ThreadProc1,		//�߳���������
		NULL				//�������������ģ���һ��PVOID�͵�ַ
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