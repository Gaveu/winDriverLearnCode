#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING Symboliclink = RTL_CONSTANT_STRING(L"\\??\\DirectIOtest");
	IoDeleteDevice(DriverObject->DeviceObject);
	IoDeleteSymbolicLink(&Symboliclink);
	KdPrint(("GoodBye Driver!\n"));
}

NTSTATUS DispatchRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	static PCHAR IrpName[] = {
		"IRP_MJ_CREATE					",
		"IRP_MJ_CREATE_NAMED_PIPE		",
		"IRP_MJ_CLOSE					",
		"IRP_MJ_READ					",
		"IRP_MJ_WRITE					",
		"IRP_MJ_QUERY_INFORMATION		",
		"IRP_MJ_SET_INFORMATION			",
		"IRP_MJ_QUERY_EA				",
		"IRP_MJ_SET_EA					",
		"IRP_MJ_FLUSH_BUFFERS			",
		"IRP_MJ_QUERY_VOLUME_INFORMATION",
		"IRP_MJ_SET_VOLUME_INFORMATION	",
		"IRP_MJ_DIRECTORY_CONTROL		",
		"IRP_MJ_FILE_SYSTEM_CONTROL		",
		"IRP_MJ_DEVICE_CONTROL			",
		"IRP_MJ_INTERNAL_DEVICE_CONTROL	",
		"IRP_MJ_SHUTDOWN				",
		"IRP_MJ_LOCK_CONTROL			",
		"IRP_MJ_CLEANUP					",
		"IRP_MJ_CREATE_MAILSLOT			",
		"IRP_MJ_QUERY_SECURITY			",
		"IRP_MJ_SET_SECURITY			",
		"IRP_MJ_POWER					",
		"IRP_MJ_SYSTEM_CONTROL			",
		"IRP_MJ_DEVICE_CHANGE			",
		"IRP_MJ_QUERY_QUOTA				",
		"IRP_MJ_SET_QUOTA				",
		"IRP_MJ_PNP						",
		"IRP_MJ_PNP_POWER				",
		"IRP_MJ_MAXIMUM_FUNCTION		"
	};

	KdPrint(("%s\n", IrpName[stack->MajorFunction]));
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DirectRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	NTSTATUS status;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	PCHAR buffer;
	PCHAR Data = "This is a string from kernel!\n";
	ULONG ByteOffset;
	ULONG ByteCount;
	PVOID Va;
	ULONG RetLen = 0;

	KdPrint(("DriectRead\n\n"));
	__try
	{
		ByteOffset = MmGetMdlByteOffset(Irp->MdlAddress);	//可读写内存在用户态进程内存空间的相对偏移
		ByteCount = MmGetMdlByteCount(Irp->MdlAddress);	//获取可读写地址的长度(非数据长度)
		Va = MmGetMdlBaseVa(Irp->MdlAddress);	//Va常为用户态进程空间的基址
												//Va + ByteOffset即为用户态程序中可读写内存的虚拟地址
												//该地址不能在驱动中直接读写,需要经由MDL映射的地址来进行读写
		/*
		KdPrint(("ByteOffset:%d\n", ByteOffset));
		KdPrint(("ByteCount:%d\n", ByteCount));	
		KdPrint(("Va:%p\n", Va));
		KdPrint(("Va+ByteOffset:%p\n", (PCHAR)Va + ByteOffset));
		*/
		buffer = (PCHAR)MmGetSystemAddressForMdlSafe(
			Irp->MdlAddress, 
			NormalPagePriority);	//获取MDL映射的可读写内存虚拟基址，驱动可直接读写
		KdPrint(("Buffer:%p\n", buffer));
		RetLen = ByteCount < strnlen(Data, 1024) ? ByteCount : strnlen(Data, 1024);
		RtlCopyMemory(buffer, Data, RetLen);
		status = STATUS_SUCCESS;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = GetExceptionCode();
		KdPrint(("Direct Read Failed!%x\n", status));
	}
	
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = RetLen;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DirectWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	NTSTATUS status;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	PCHAR buffer;
	PCHAR wBuf[1024] = { 0 };
	ULONG ByteOffset;
	ULONG ByteCount;
	PVOID Va;
	ULONG RetLen = 0;
	KdPrint(("DriectWrite\n\n"));
	__try
	{
		ByteOffset = MmGetMdlByteOffset(Irp->MdlAddress);
		ByteCount = MmGetMdlByteCount(Irp->MdlAddress);
		Va = MmGetMdlBaseVa(Irp->MdlAddress);

		KdPrint(("ByteOffset:%d\n", ByteOffset));
		KdPrint(("ByteCount:%d\n", ByteCount));	//获取可读写地址的长度(非数据长度)
		KdPrint(("Va:%p\n", Va));
		KdPrint(("Va+ByteOffset:%p\n", (PCHAR)Va + ByteOffset));

		buffer = (PCHAR)MmGetSystemAddressForMdlSafe(
			Irp->MdlAddress,
			NormalPagePriority);	//获取可读写内存虚拟基址
		KdPrint(("Buffer:%p\n", buffer));
		RetLen = 1024 < strnlen(buffer, 1024) ? 1023 : strnlen(buffer, 1024);
		RtlCopyMemory(wBuf, buffer, RetLen);
		wBuf[RetLen] = '\0';

		KdPrint(("WriteIn:%s\n", wBuf));
		KdPrint(("WriteInLen:%d\n", RetLen));
		status = STATUS_SUCCESS;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = GetExceptionCode();
		KdPrint(("Direct Read Failed!%x\n", status));
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = RetLen;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\DirectIOtest");
	UNICODE_STRING Symboliclink = RTL_CONSTANT_STRING(L"\\??\\DirectIOtest");
	PDEVICE_OBJECT DeviceObject = NULL;
	NTSTATUS status;
	int i;

	status = IoCreateDevice(
		DriverObject,
		0,
		&DeviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&DeviceObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Device Create Failed!%d\n", status));
		return status;
	}
	status = IoCreateSymbolicLink(
		&Symboliclink,
		&DeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Symboliclink Create Failed!%d\n", status));
		IoDeleteDevice(DeviceObject);
		return status;
	}

	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = DispatchRoutine;
	}
	DriverObject->MajorFunction[IRP_MJ_READ] = DirectRead;
	DriverObject->MajorFunction[IRP_MJ_WRITE] = DirectWrite;

	DeviceObject->Flags |= DO_DIRECT_IO;
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;


	return STATUS_SUCCESS;
}