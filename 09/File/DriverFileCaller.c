#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{

	KdPrint(("Goodbye DriverCaller!\n"));
}

NTSTATUS DrvCallerTest1()
{
	HANDLE hDevice;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\DriverA");
	NTSTATUS status;
	OBJECT_ATTRIBUTES oa;
	IO_STATUS_BLOCK IoStatus;
	
	InitializeObjectAttributes(
		&oa,	//���趨���豸����������
		&DeviceName,	//����·��
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,	//�ں˾���Ҵ�Сд����
		NULL,	//���趨��Ŀ¼
		NULL	//���趨��ȫ������
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

	ZwReadFile(
		hDevice,	//��ȡ���ļ����
		NULL,		//����ͬ�����¼�������
		NULL,		//Apc���̣��˴�����Ҫ
		NULL,		//Apc���������ģ��˴�����Ҫ
		&IoStatus,	//����ִ�е�Io״̬��
		NULL,		//�����ݵĻ�����
		0,			//����������
		NULL,		//������ʼ�ֽ�ƫ��
		NULL		//��������ΪNULL
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

	DrvCallerTest1();

	return STATUS_SUCCESS;
}