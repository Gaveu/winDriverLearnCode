#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{

	KdPrint(("GoodBye Driver!\n"));
}

VOID CreateFileTest1()
{
	HANDLE hFile;
	NTSTATUS status;
	IO_STATUS_BLOCK IoStatus;
	UNICODE_STRING FilePath = RTL_CONSTANT_STRING(L"\\??\\C:\\test01.log");
	OBJECT_ATTRIBUTES oa;
	InitializeObjectAttributes(
		&oa,										//����ʼ����OBJECT_ATTRIBUTES�ṹ��
		&FilePath,									//�ļ�·��
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,	//�ļ����ԣ������ִ�Сд|�ں˶���
		NULL,										//�򿪻��½��ļ��ĸ�Ŀ¼
		NULL										//��ȫ������
		);
	status = ZwCreateFile(
		&hFile,					//���ص��ļ����
		GENERIC_ALL,			//�����ķ���Ȩ��
		&oa,					//�������Կ�
		&IoStatus,				//���ص�״̬��
		NULL,					//�������ʼ��С
		FILE_ATTRIBUTE_NORMAL,	//������ļ�����
		FILE_SHARE_READ | FILE_SHARE_WRITE,	//�ļ�����ģʽ�����ļ������д������ռ����Ϊ0
		FILE_OPEN_IF,			//�ļ���������򿪸��ļ�
		FILE_SYNCHRONOUS_IO_NONALERT,	//�ļ�ͬ���޾���
		NULL,					//EA��������ַ������Ҫ����Ϊ��ָ��
		0						//EA���������ȣ�����Ҫ����Ϊ0
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("ZwCreateFile Failed!%x\n", status));
	}
	else
	{
		KdPrint(("ZwCreateFile Success!\n"));
	}
	ZwClose(hFile);
}

VOID CreateFileTest2()
{
	HANDLE hFile;
	NTSTATUS status;
	IO_STATUS_BLOCK IoStatus;
	UNICODE_STRING FilePath = RTL_CONSTANT_STRING(L"\\??\\C:\\test02.log");
	OBJECT_ATTRIBUTES oa;
	InitializeObjectAttributes(
		&oa,										//����ʼ����OBJECT_ATTRIBUTES�ṹ��
		&FilePath,									//�ļ�·��
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,	//�����ִ�Сд|�ں˶���
		NULL,										//�򿪻��½��ļ��ĸ�Ŀ¼
		NULL										//��ȫ������
		);
	status = IoCreateFile(
		&hFile,								//���ص��ļ����
		GENERIC_ALL,						//�����ķ���Ȩ��
		&oa,								//�������Կ�
		&IoStatus,							//���ص�״̬��
		NULL,								//�������ʼ��С
		FILE_ATTRIBUTE_NORMAL,				//������ļ�����
		FILE_SHARE_READ | FILE_SHARE_WRITE,	//�ļ�����ģʽ�����ļ������д������ռ����Ϊ0
		FILE_OPEN_IF,						//�ļ���������򿪸��ļ�
		FILE_SYNCHRONOUS_IO_NONALERT,		//�ļ�ͬ���޾���
		NULL,								//EA��������ַ������Ҫ����Ϊ��ָ��
		0,									//EA���������ȣ�����Ҫ����Ϊ0
		CreateFileTypeNone,					//�����ڱ�����ΪCreateFileTypeNone
		NULL,								//����Ӧ�����øò���ΪNULL
		0									//I/O����������ҪΪ��Ӧ������������κβ���			
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("IoCreateFile Failed!%x\n", status));
	}
	else
	{
		KdPrint(("IoCreateFile Success!\n"));
	}
	ZwClose(hFile);
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;

	CreateFileTest1();
	CreateFileTest2();


	return STATUS_SUCCESS;
}