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

VOID QueryInfoTest()
{
	IO_STATUS_BLOCK IoStatus;
	NTSTATUS status;
	HANDLE hFile;
	UNICODE_STRING FilePath = RTL_CONSTANT_STRING(L"\\??\\C:\\test\\DispatchTest01.sys");
	OBJECT_ATTRIBUTES oa;
	FILE_STANDARD_INFORMATION fileInfo;

	InitializeObjectAttributes(
		&oa,										//待初始化的OBJECT_ATTRIBUTES结构体
		&FilePath,									//文件路径
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,	//文件属性，不区分大小写|内核对象
		NULL,										//打开或新建文件的根目录
		NULL										//安全描述符
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

	//查询文件信息
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
		KdPrint(("File Create Failed!%x\n",status));
		return;
	}
	
	PCHAR data = "This is the data for writing!\n";
	status = ZwWriteFile(
		hFile,				//待写入的文件句柄
		NULL,				//事件句柄，这里用不上
		NULL,				//APC例程，用不上
		NULL,				//APC上下文，用不上
		&IoStatus,			//返回执行状态
		data,				//待写入的数据
		strlen(data),		//待写入的数据长度
		NULL,				//在文件中待写入的偏移位置，NULL则指文件开始处
		NULL				//标志，用不上
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
		KdPrint(("File Create Failed!%x\n", status));
		return;
	}

	CHAR buffer[1024] = { 0 };
	status = ZwReadFile(
		hFile,				//待读取的文件句柄
		NULL,				//事件句柄，这里用不上
		NULL,				//APC例程，用不上
		NULL,				//APC上下文，用不上
		&IoStatus,			//返回执行状态
		buffer,				//返回读取数据的缓冲区
		sizeof(buffer),		//读取数据的长度
		NULL,				//在文件中待写入的偏移位置，NULL则指文件开始处
		NULL				//标志，用不上
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