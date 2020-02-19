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
		&oa,										//待初始化的OBJECT_ATTRIBUTES结构体
		&FilePath,									//文件路径
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,	//文件属性，不区分大小写|内核对象
		NULL,										//打开或新建文件的根目录
		NULL										//安全描述符
		);
	status = ZwCreateFile(
		&hFile,					//返回的文件句柄
		GENERIC_ALL,			//完整的访问权限
		&oa,					//对象属性块
		&IoStatus,				//返回的状态块
		NULL,					//不分配初始大小
		FILE_ATTRIBUTE_NORMAL,	//常规的文件属性
		FILE_SHARE_READ | FILE_SHARE_WRITE,	//文件共享模式，该文件共享读写，若独占则设为0
		FILE_OPEN_IF,			//文件若存在则打开该文件
		FILE_SYNCHRONOUS_IO_NONALERT,	//文件同步无警告
		NULL,					//EA缓冲区地址，不需要则设为空指针
		0						//EA缓冲区长度，不需要则设为0
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
		&oa,										//待初始化的OBJECT_ATTRIBUTES结构体
		&FilePath,									//文件路径
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,	//不区分大小写|内核对象
		NULL,										//打开或新建文件的根目录
		NULL										//安全描述符
		);
	status = IoCreateFile(
		&hFile,								//返回的文件句柄
		GENERIC_ALL,						//完整的访问权限
		&oa,								//对象属性块
		&IoStatus,							//返回的状态块
		NULL,								//不分配初始大小
		FILE_ATTRIBUTE_NORMAL,				//常规的文件属性
		FILE_SHARE_READ | FILE_SHARE_WRITE,	//文件共享模式，该文件共享读写，若独占则设为0
		FILE_OPEN_IF,						//文件若存在则打开该文件
		FILE_SYNCHRONOUS_IO_NONALERT,		//文件同步无警告
		NULL,								//EA缓冲区地址，不需要则设为空指针
		0,									//EA缓冲区长度，不需要则设为0
		CreateFileTypeNone,					//驱动内必须设为CreateFileTypeNone
		NULL,								//驱动应当设置该参数为NULL
		0									//I/O管理器不需要为响应创建请求进行任何操作			
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