#include<ntddk.h>

KDPC dpc;		//ȫ��DPC
KTIMER timer;	//��ʱ������
LARGE_INTEGER timeout;//��ʱ����

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	//ȡ����ʱ��
	KeCancelTimer(&timer);
	KdPrint(("Goodbye Driver!\n"));
}

VOID DpcRoutine(
	_In_     struct _KDPC *Dpc,
	_In_opt_ PVOID        DeferredContext,
	_In_opt_ PVOID        SystemArgument1,
	_In_opt_ PVOID        SystemArgument2
	)
{
	KdPrint(("DPC Tik!\n"));

	//ʹ��ʱ���ܹ��ظ���ʱ
	KeSetTimer(&timer, timeout, &dpc);
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;
	
	//��ʼ����ʱ��
	KeInitializeTimer(&timer);

	//�趨��ʱ�����-10 * (1000*1000),������Ϊ1000ms
	timeout = RtlConvertLongToLargeInteger(-10 * 1000 * 1000);
	
	//��ʼ��DPC
	KeInitializeDpc(
		&dpc,		//����ʼ����dpc
		DpcRoutine,	//��ʱ����������
		NULL		//�����ģ�����ַ
		);

	//����DPC���ڵĶ�ʱ��
	KeSetTimer(
		&timer,		//�����Ķ�ʱ��
		timeout,	//��ʱ���
		&dpc		//��ʱ�����ڵ�DPC
		);	

	return STATUS_SUCCESS;
}