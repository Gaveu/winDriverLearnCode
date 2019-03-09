#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Driver Unload!\n"));
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDDK");
	IoDeleteDevice(DriverObject->DeviceObject);
	IoDeleteSymbolicLink(&SymbolicLinkName);
}

NTSTATUS DispatchRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	static PCHAR typeIrp[] = {
		  "IRP_MJ_CREATE			     "
		, "IRP_MJ_CREATE_NAMED_PIPE		 "
		, "IRP_MJ_CLOSE					 "
		, "IRP_MJ_READ					 "
		, "IRP_MJ_WRITE					 "
		, "IRP_MJ_QUERY_INFORMATION		 "
		, "IRP_MJ_SET_INFORMATION		 "
		, "IRP_MJ_QUERY_EA				 "
		, "IRP_MJ_SET_EA				 "
		, "IRP_MJ_FLUSH_BUFFERS			 "
		, "IRP_MJ_QUERY_VOLUME_INFORMATION"
		, "IRP_MJ_SET_VOLUME_INFORMATION "
		, "IRP_MJ_DIRECTORY_CONTROL		 "
		, "IRP_MJ_FILE_SYSTEM_CONTROL	 "
		, "IRP_MJ_DEVICE_CONTROL		 "
		, "IRP_MJ_INTERNAL_DEVICE_CONTROL"
		, "IRP_MJ_SHUTDOWN				 "
		, "IRP_MJ_LOCK_CONTROL			 "
		, "IRP_MJ_CLEANUP				 "
		, "IRP_MJ_CREATE_MAILSLOT		 "
		, "IRP_MJ_QUERY_SECURITY		 "
		, "IRP_MJ_SET_SECURITY			 "
		, "IRP_MJ_POWER					 "
		, "IRP_MJ_SYSTEM_CONTROL		 "
		, "IRP_MJ_DEVICE_CHANGE			 "
		, "IRP_MJ_QUERY_QUOTA			 "
		, "IRP_MJ_SET_QUOTA				 "
		, "IRP_MJ_PNP					 " };

	KdPrint(("%s\n", typeIrp[stack->MajorFunction]));
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;

}

NTSTATUS DispatchRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_SUCCESS;
	ULONG Length;
	/*
	windows中的try-except机制，较c++中的try-catch功能要更完善些
	如果__try复合语句中受保护的代码执行中出现了异常，
	首先按函数调用顺序从新向旧搜索所有包含了出现异常的执行点的try-except语句，执行每个except的过滤函数，
	直到某个try-except语句的过滤函数结果值为EXCEPTION_EXECUTE_HANDLER。
	这时，再从包含异常出现的执行点的最内层try-finally语句开始由内向外执行每个finally块中的代码，
	直至回退到那个过滤函数结果值为EXCEPTION_EXECUTE_HANDLER的try-except语句执行其except块，
	最后执行该try-except语句后面的其他代码。
	
	具体用法可参考msdn
	*/
	
	__try
	{
		//获取用户空间读内存的长度
		Length = stack->Parameters.Read.Length;
		if (Length > 512)
		{
			RtlCopyMemory(Irp->AssociatedIrp.SystemBuffer, "This is a string from kernel", strlen("This is a string from kernel"));
			status = STATUS_SUCCESS;
			Length = strlen("This is a string from kernel");
		}
		else
		{
			status = STATUS_INSUFFICIENT_RESOURCES;
			Length = 0;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		//产生异常时，调用GetExceptionCode来获取与本次异常相关的NTSTATUS码
		status = GetExceptionCode();
		KdPrint(("DispatchRead failed! 0x%x\n", status));
		Length = 0;
	}

	Irp->IoStatus.Information = Length;	//返回至dwRet
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	PDEVICE_OBJECT DeviceObject;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloDDK");
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDDK");
	DriverObject->DriverUnload = Unload;

	status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("CreateDevice failed! 0x%x\n",status));
		return status;
	}
	
	status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("CreateSymbolicLink failed! 0x%x\n",status));
		IoDeleteDevice(DeviceObject);
		return status;
	}

	ULONG i;
	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = DispatchRoutine;
	}
	DriverObject->MajorFunction[IRP_MJ_READ] = DispatchRead;

	DeviceObject->Flags |= DO_BUFFERED_IO;				//通知操作系统该设备为缓冲区读写方式
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;		//通知操作系统该设备已可以使用了

	return STATUS_SUCCESS;
}