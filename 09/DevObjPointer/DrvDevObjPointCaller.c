#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{

	KdPrint(("Goodbye DriverCaller!\n"));
}

NTSTATUS DrvCallerTest3()
{
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\DriverA");
	NTSTATUS status;
	KEVENT event;
	IO_STATUS_BLOCK IoStatus;
	PFILE_OBJECT pFileObject;
	PDEVICE_OBJECT DeviceObject;
	PIRP pIrp;

	//根据设备名寻找对应设备对象指针及其文件对象指针
	status = IoGetDeviceObjectPointer(
		&DeviceName,	//待寻找的设备名
		FILE_ANY_ACCESS,//文件访问权限
		&pFileObject,	//返回的文件对象指针
		&DeviceObject	//返回的设备对象指针
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("DeviceObject Find Failed!%x\n", status));
		return status;
	}

	KdPrint(("DeviceObject Get Success!\n"));

	KeInitializeEvent(
		&event,
		SynchronizationEvent,
		FALSE
		);

	//构造同步系统文件请求IRP,以函数返回值的形式返回该构造好的IRP
	//同步方式
	//pIrp = IoBuildSynchronousFsdRequest(
	//	IRP_MJ_READ,	//待传送的IRP请求类型
	//	DeviceObject,	//IRP请求传送的目标设备对象
	//	NULL,			//缓冲区地址
	//	0,				//缓冲区长度
	//	NULL,			//读写字节起始偏移量
	//	&event,			//同步事件对象，当IRP传送成功后会释放该事件
	//	&IoStatus		//返回的IO状态体
	//	);

	//构造同步系统文件请求IRP,以函数返回值的形式返回该构造好的IRP
	//异步方式
	pIrp = IoBuildAsynchronousFsdRequest(
		IRP_MJ_READ,	//待传送的IRP请求类型
		DeviceObject,	//IRP请求传送的目标设备对象
		NULL,			//缓冲区地址
		0,				//缓冲区长度
		NULL,			//读写字节起始偏移量
		&IoStatus		//返回的IO状态体
		);
	pIrp->UserEvent = &event;

	//初始化IO栈单元
	PIO_STACK_LOCATION stack = IoGetNextIrpStackLocation(pIrp);
	stack->FileObject = pFileObject;

	//向指定设备对象发送IRP请求
	KdPrint(("Read Start!\n"));
	status = IoCallDriver(DeviceObject, pIrp);
	KdPrint(("Read Wait!\n"));
	KeWaitForSingleObject(
		&event,
		Executive,
		KernelMode,
		FALSE,
		NULL
		);
	KdPrint(("Read Finished!\n"));

	//返还文件对象
	ObDereferenceObject(pFileObject);

	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello DriverCaller!\n"));
	DriverObject->DriverUnload = Unload;

	DrvCallerTest3();

	return STATUS_SUCCESS;
}