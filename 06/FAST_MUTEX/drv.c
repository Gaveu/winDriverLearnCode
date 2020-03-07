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
		pMutex,		//�ȴ��Ļ�����
		Executive,	//�ȴ�ԭ��Ϊִ��
		KernelMode,	//������Ӧ��ΪKernelMode
		FALSE,		//������
		NULL		//�����ó�ʱʱ��
		);
	KdPrint(("Thread 1 Wait!\n"));
	MySleep(5000);
	KeReleaseMutex(
		pMutex,	//���ͷŵ��ź���
		FALSE	//����Ҫ���KeWaitXXX�ຯ��
		);

	KdPrint(("Thread 1 End!\n"));
	PsTerminateSystemThread(STATUS_SUCCESS);
}

VOID ThreadProc2(PVOID Context)
{
	PKMUTEX pMutex = (PKMUTEX)Context;
	KdPrint(("Thread 2 Start!\n"));
	KeWaitForSingleObject(
		pMutex,		//�ȴ��Ļ�����
		Executive,	//�ȴ�ԭ��Ϊִ��
		KernelMode,	//������Ӧ��ΪKernelMode
		FALSE,		//������
		NULL		//�����ó�ʱʱ��
		);
	KdPrint(("Thread 2 Wait!\n"));
	MySleep(5000);
	KeReleaseMutex(
		pMutex,	//���ͷŵ��ź���
		FALSE	//����Ҫ���KeWaitXXX�ຯ��
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
		&mutex,	//����ʼ���Ļ�����
		0		//����������Ӧ��Ϊ0
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
	
	//���ݾ����ȡ�̶߳���
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
		2,			//�ȴ���Դ������Ŀ 
		thread,		//�ȴ���Դ��������
		WaitAll,	//�ȴ���ʽΪȫ���ȴ�
		Executive,	//�ȴ�ԭ��Ϊִ��
		KernelMode,	//��������ΪKernelMode
		FALSE,		//������
		NULL,		//���趨��ʱʱ��
		NULL		//���趨�ȴ���
		);

	//��������
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