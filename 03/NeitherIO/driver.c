#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	IoDeleteDevice(DriverObject->DeviceObject);
	UNICODE_STRING SymboliclinkName = RTL_CONSTANT_STRING(L"\\??\\NeitherIO");
	IoDeleteSymbolicLink(&SymboliclinkName);
	KdPrint(("GoodBye Driver!\n"));
}

NTSTATUS DispatchRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
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

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	KdPrint(("%s\n", IrpName[stack->MajorFunction]));

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS NeitherRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status;
	PCHAR data = "This is a data from Kernel!\n";
	int retLen = 0;
	__try
	{
		ProbeForWrite(
			Irp->UserBuffer,				//用户态应用程序传来的内存地址 
			stack->Parameters.Read.Length,	//可写内存长度
			4								//内存的字节对齐大小
			);
		retLen = strlen(data);
		RtlCopyMemory(Irp->UserBuffer, data, retLen);
		status = STATUS_SUCCESS;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = GetExceptionCode();
		retLen = 0;
		KdPrint(("Neither Read Failed!%d\n", status));
	}
	Irp->IoStatus.Information = retLen;
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS NeitherWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status;
	CHAR Data[1024] = { 0 };
	int retLen = 0;
	int wLen = 0;
	__try
	{
		wLen = stack->Parameters.Write.Length;
		ProbeForRead(
			Irp->UserBuffer,				//用户态应用程序传来的内存地址 
			wLen,	//可读内存长度
			4								//内存的字节对齐大小
			);
		if (wLen > 1024)
		{
			retLen = 1024;
		}
		else
		{
			retLen = wLen;
		}
		RtlCopyMemory(Data, Irp->UserBuffer, retLen);
		Data[retLen] = '\0';
		KdPrint(("%s\n", Data));
		status = STATUS_SUCCESS;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = GetExceptionCode();
		retLen = 0;
		KdPrint(("Neither Write Failed!%d\n", status));
	}
	Irp->IoStatus.Information = retLen;
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;

	int i = 0;
	NTSTATUS status;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\NeitherIO");
	UNICODE_STRING SymboliclinkName = RTL_CONSTANT_STRING(L"\\??\\NeitherIO");
	PDEVICE_OBJECT DeviceObject = NULL;

	status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Device Create Failed!%d\n", status));
		return status;
	}
	status = IoCreateSymbolicLink(&SymboliclinkName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("SymbolicLink Create Failed!%d\n", status));
		IoDeleteDevice(DeviceObject);
		return status;
	}

	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = DispatchRoutine;
	}
	DriverObject->MajorFunction[IRP_MJ_READ] = NeitherRead;
	DriverObject->MajorFunction[IRP_MJ_WRITE] = NeitherWrite;

	DriverObject->Flags |= 0;
	DriverObject->Flags &= ~DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}