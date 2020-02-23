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

VOID QueryInfoTest()
{
	IO_STATUS_BLOCK IoStatus;
	NTSTATUS status;
	HANDLE hFile;
	UNICODE_STRING FilePath = RTL_CONSTANT_STRING(L"\\??\\C:\\test\\DispatchTest01.sys");
	OBJECT_ATTRIBUTES oa;
	FILE_STANDARD_INFORMATION fileInfo;

	InitializeObjectAttributes(
		&oa,										//����ʼ����OBJECT_ATTRIBUTES�ṹ��
		&FilePath,									//�ļ�·��
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,	//�ļ����ԣ������ִ�Сд|�ں˶���
		NULL,										//�򿪻��½��ļ��ĸ�Ŀ¼
		NULL										//��ȫ������
		);
	status = ZwOpenFile(
		&hFile,
		FILE_ALL_ACCESS,
		&oa,
		&IoStatus,
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		FILE_SYNCHRONOUS_IO_NONALERT
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("File Open Failed!%x\n", status));
		return;
	}

	//��ѯ�ļ���Ϣ
	status = ZwQueryInformationFile(
		hFile,
		&IoStatus,
		&fileInfo,
		sizeof(fileInfo),
		FileStandardInformation
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("File Info Query Failed!%x\n", status));
		ZwClose(hFile);
		return;
	}
	KdPrint(("File size:%d\n", fileInfo.EndOfFile.LowPart));
	ZwClose(hFile);
}

VOID WriteTest()
{
	IO_STATUS_BLOCK IoStatus;
	NTSTATUS status;
	HANDLE hFile;
	UNICODE_STRING FilePath = RTL_CONSTANT_STRING(L"\\??\\C:\\test03.log");
	OBJECT_ATTRIBUTES oa;
	FILE_STANDARD_INFORMATION fileInfo;

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
		KdPrint(("File Create Failed!%x\n",status));
		return;
	}
	
	PCHAR data = "This is the data for writing!\n";
	status = ZwWriteFile(
		hFile,				//��д����ļ����
		NULL,				//�¼�����������ò���
		NULL,				//APC���̣��ò���
		NULL,				//APC�����ģ��ò���
		&IoStatus,			//����ִ��״̬
		data,				//��д�������
		strlen(data),		//��д������ݳ���
		NULL,				//���ļ��д�д���ƫ��λ�ã�NULL��ָ�ļ���ʼ��
		NULL				//��־���ò���
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("File Write Failed!%x\n", status));
		ZwClose(hFile);
		return;
	}

	ZwClose(hFile);
}

VOID ReadTest()
{
	IO_STATUS_BLOCK IoStatus;
	NTSTATUS status;
	HANDLE hFile;
	UNICODE_STRING FilePath = RTL_CONSTANT_STRING(L"\\??\\C:\\test03.log");
	OBJECT_ATTRIBUTES oa;
	FILE_STANDARD_INFORMATION fileInfo;

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
		KdPrint(("File Create Failed!%x\n", status));
		return;
	}

	CHAR buffer[1024] = { 0 };
	status = ZwReadFile(
		hFile,				//����ȡ���ļ����
		NULL,				//�¼�����������ò���
		NULL,				//APC���̣��ò���
		NULL,				//APC�����ģ��ò���
		&IoStatus,			//����ִ��״̬
		buffer,				//���ض�ȡ���ݵĻ�����
		sizeof(buffer),		//��ȡ���ݵĳ���
		NULL,				//���ļ��д�д���ƫ��λ�ã�NULL��ָ�ļ���ʼ��
		NULL				//��־���ò���
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("File Read Failed!%x\n", status));
		ZwClose(hFile);
		return;
	}
	KdPrint(("Data Read:%s\n", buffer));
	ZwClose(hFile);
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;

	//CreateFileTest1();
	//CreateFileTest2();
	//QueryInfoTest();

	WriteTest();
	ReadTest();

	return STATUS_SUCCESS;
}