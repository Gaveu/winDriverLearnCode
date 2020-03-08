#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{

	KdPrint(("Goodbye DriverCaller!\n"));
}

VOID MyApcRoutine(
	_In_ PVOID ApcContext,
	_In_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ ULONG Reserved
	)
{
	KdPrint(("ApcRoutine Start!\n"));
	PKEVENT pEvent = (PKEVENT)ApcContext;
	KeSetEvent(
		pEvent,
		IO_NO_INCREMENT,
		FALSE
		);
	KdPrint(("ApcRoutine Finished!\n"));
}

NTSTATUS DrvCallerTest2()
{
	HANDLE hDevice;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\DriverA");
	NTSTATUS status;
	KEVENT event;
	OBJECT_ATTRIBUTES oa;
	IO_STATUS_BLOCK IoStatus;

	InitializeObjectAttributes(
		&oa,	//���趨���豸����������
		&DeviceName,	//����·��
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,	//�ں˾���Ҵ�Сд����
		NULL,	//���趨��Ŀ¼
		NULL	//���趨��ȫ������
		);

	KeInitializeEvent(
		&event,
		SynchronizationEvent,
		FALSE
		);
	
	status = ZwCreateFile(
		&hDevice,	//���ش��ļ��ľ��
		GENERIC_ALL,//��׼ȫ������Ȩ��
		&oa,		//����������
		&IoStatus,	//���ص�Io״̬��
		NULL,		//��������ռ�
		FILE_ATTRIBUTE_NORMAL,//�����ļ�����
		FILE_SHARE_READ | FILE_SHARE_WRITE,//�ļ��������ԣ������д
		FILE_OPEN,	//������Ϊ����������ļ�
		FILE_SYNCHRONOUS_IO_NONALERT,	//����ѡ�����������IOͬ������
		NULL,		//������EA������
		0			//EA��������СΪ0
		);

	if (!NT_SUCCESS(status))
	{
		KdPrint(("Create File Failed!%x\n", status));
		return status;
	}

	KdPrint(("Read Start!\n"));

	ZwReadFile(
		hDevice,	//��ȡ���ļ����
		NULL,		//����ͬ�����¼�������
		MyApcRoutine,//Apc�ص����̣��˴����ڴ����첽ִ��
		&event,		//Apc�ص����������ģ��˴������¼�����Դ����첽ִ��
		&IoStatus,	//����ִ�е�Io״̬��
		NULL,		//�����ݵĻ�����
		0,			//����������
		NULL,		//������ʼ�ֽ�ƫ��
		NULL		//��������ΪNULL
		);
	KdPrint(("Read Wait!\n"));
	KeWaitForSingleObject(
		&event,
		Executive,
		KernelMode,
		FALSE,
		NULL
		);
	KdPrint(("Read Finished!\n"));
	ZwClose(hDevice);

	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello DriverCaller!\n"));
	DriverObject->DriverUnload = Unload;

	DrvCallerTest2();

	return STATUS_SUCCESS;
}