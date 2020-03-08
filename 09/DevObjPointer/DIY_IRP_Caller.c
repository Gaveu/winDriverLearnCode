#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{

	KdPrint(("Goodbye DriverCaller!\n"));
}

NTSTATUS DrvCallerTest4()
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


	//自行构建一个IRP请求，并自行初始化其所对应的栈单元
	pIrp = IoAllocateIrp(//在栈中申请一个Irp的空间
		DeviceObject->StackSize,	//指定待申请Irp在I/O请求栈中的位置
		FALSE						//不选定配额
		);
	if (pIrp != NULL)
	{
		//配置IRP
		pIrp->UserIosb = &IoStatus;	//配置IO状态块
		pIrp->UserEvent = &event;	//配置事件对象
		pIrp->AssociatedIrp.SystemBuffer = NULL;	//配置读写缓冲区
		pIrp->Tail.Overlay.Thread = PsGetCurrentThread();	//配置IRP的源线程
		//初始化IO栈单元
		PIO_STACK_LOCATION stack = IoGetNextIrpStackLocation(pIrp);
		stack->FileObject = pFileObject;	//配置栈单元的文件对象
		stack->DeviceObject = DeviceObject;	//配置IRP请求的设备对象
		stack->MajorFunction = IRP_MJ_READ;	//配置IRP请求的主功能码
		stack->MinorFunction = IRP_MN_NORMAL;//配置IRP请求的子功能码
	}

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

	DrvCallerTest4();

	return STATUS_SUCCESS;
}