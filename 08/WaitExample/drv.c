#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{

	KdPrint(("Goodbye Driver!\n"));
}

//��ͣ�����΢����
VOID Wait1(ULONG ulMSeconds)
{
	KEVENT Event;
	LARGE_INTEGER timeout = RtlConvertLongToLargeInteger(-10 * ulMSeconds);
	KeInitializeEvent(
		&Event,	//����ʼ�����¼�����
		SynchronizationEvent,	//ͬ���¼�
		FALSE	//��ֱ�ӱ�KeSetEvent()���ã�����ȴ�KeWaitXXX������
		);	
	KdPrint(("Wait1 Started!\n"));
	KeWaitForSingleObject(
		&Event,		//�¼�����
		Executive,	//�ȴ�ԭ��Ϊִ��
		KernelMode,	//��������Ӧ��Ϊ�ں�̬
		FALSE,		//������
		&timeout	//�ȴ�ʱ��
		);
	KdPrint(("Wait1 Finished!\n"));
}

//��ͣ�����΢����
VOID Wait2(ULONG ulMSeconds)
{
	KTIMER timer;
	LARGE_INTEGER timeout = RtlConvertLongToLargeInteger(-10 * ulMSeconds);
	KeInitializeTimer(&timer);

	KeSetTimer(
		&timer,		//�����ƶ�ʱ��
		timeout,	//��ʱ���
		NULL		//���������κ�DPC��
		);

	KdPrint(("Wait2 Started!\n"));
	KeWaitForSingleObject(
		&timer,		//��ʱ������
		Executive,	//�ȴ�ԭ��Ϊִ��
		KernelMode,	//��������Ӧ��Ϊ�ں�̬
		FALSE,		//������
		&timeout	//�ȴ�ʱ��
		);
	KdPrint(("Wait2 Finished!\n"));
}

//��ͣ�����΢����
VOID Wait3(ULONG ulMSeconds)
{
	LARGE_INTEGER timeout = RtlConvertLongToLargeInteger(-10 * ulMSeconds);
	KdPrint(("Wait3 Started!\n"));
	KeDelayExecutionThread(
		KernelMode,	//�ں�ģʽ
		FALSE,		//������
		&timeout	//�ȴ�ʱ��
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