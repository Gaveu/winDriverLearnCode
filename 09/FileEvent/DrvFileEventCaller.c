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
		&oa,	//待设定的设备对象属性体
		&DeviceName,	//对象路径
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,	//内核句柄且大小写敏感
		NULL,	//不设定根目录
		NULL	//不设定安全描述符
		);

	KeInitializeEvent(
		&event,
		SynchronizationEvent,
		FALSE
		);
	
	status = ZwCreateFile(
		&hDevice,	//返回打开文件的句柄
		GENERIC_ALL,//标准全部访问权限
		&oa,		//对象属性体
		&IoStatus,	//返回的Io状态体
		NULL,		//无需申请空间
		FILE_ATTRIBUTE_NORMAL,//正常文件属性
		FILE_SHARE_READ | FILE_SHARE_WRITE,//文件共享属性，共享读写
		FILE_OPEN,	//创建行为，创建后打开文件
		FILE_SYNCHRONOUS_IO_NONALERT,	//创建选项，启动后无视IO同步警告
		NULL,		//不设置EA缓冲区
		0			//EA缓冲区大小为0
		);

	if (!NT_SUCCESS(status))
	{
		KdPrint(("Create File Failed!%x\n", status));
		return status;
	}

	KdPrint(("Read Start!\n"));

	ZwReadFile(
		hDevice,	//读取的文件句柄
		NULL,		//用于同步的事件对象句柄
		MyApcRoutine,//Apc回调例程，此处用于处理异步执行
		&event,		//Apc回调例程上下文，此处传入事件句柄以处理异步执行
		&IoStatus,	//返回执行的Io状态体
		NULL,		//读数据的缓冲区
		0,			//缓冲区长度
		NULL,		//读的起始字节偏移
		NULL		//保留，填为NULL
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