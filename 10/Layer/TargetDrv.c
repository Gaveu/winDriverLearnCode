#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING SymboliclinkName = RTL_CONSTANT_STRING(L"\\??\\BufferIO");
	IoDeleteDevice(DriverObject->DeviceObject);
	IoDeleteSymbolicLink(&SymboliclinkName);
	KdPrint(("Goodbye TargetDriver!\n"));
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

	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DispatchRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	NTSTATUS status;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	KdPrint(("IRP_MJ_READ\n"));
	PCHAR str = "This is a string form kernel!\n";
	int length;
	__try
	{
		if (stack->Parameters.Read.Length > 512)
		{
			length = strnlen(str, 1024);
			RtlCopyMemory(Irp->AssociatedIrp.SystemBuffer, str, length);
			status = STATUS_SUCCESS;
		}
		else
		{
			status = STATUS_INSUFFICIENT_RESOURCES;
			length = 0;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = GetExceptionCode();
		KdPrint(("EXCEPTION:%d", status));
		length = 0;
	}

	Irp->IoStatus.Information = length;
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DispatchWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	NTSTATUS status;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	KdPrint(("IRP_MJ_WRITE\n"));
	int length = 0;
	PCHAR Buf = ExAllocatePool(NonPagedPool, 1024);
	if (!Buf)
	{
		status = STATUS_INSUFFICIENT_RESOURCES;
		Irp->IoStatus.Information = length;
		Irp->IoStatus.Status = status;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	RtlZeroMemory(Buf, 1024);
	__try
	{
		if (stack->Parameters.Write.Length < 1024 && stack->Parameters.Write.Length > 0)
		{
			length = strnlen(Irp->AssociatedIrp.SystemBuffer, 1024);
			RtlCopyMemory(Buf, Irp->AssociatedIrp.SystemBuffer, length);
			KdPrint(("Write:%s\n", Buf));
			KdPrint(("WriteLen:%d\n", length));
			status = STATUS_SUCCESS;
		}
		else
		{
			status = STATUS_INSUFFICIENT_RESOURCES;
			length = 0;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = GetExceptionCode();
		KdPrint(("EXCEPTION:%d", status));
		length = 0;
	}

	ExFreePool(Buf);
	Irp->IoStatus.Information = length;
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = Unload;
	KdPrint(("Hello TargetDriver!\n"));

	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\BufferIO");
	UNICODE_STRING SymboliclinkName = RTL_CONSTANT_STRING(L"\\??\\BufferIO");
	PDEVICE_OBJECT DeviceObject = NULL;
	NTSTATUS status;
	int i;

	status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Device Create Failed!%d\n", status));
		return status;
	}
	status = IoCreateSymbolicLink(&SymboliclinkName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("SymboliclinkName Create Failed!%d\n", status));
		IoDeleteDevice(DeviceObject);
	}

	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		DriverObject->MajorFunction[i] = DispatchRoutine;
	}
	DriverObject->MajorFunction[IRP_MJ_READ] = DispatchRead;
	DriverObject->MajorFunction[IRP_MJ_WRITE] = DispatchWrite;

	DeviceObject->Flags |= DO_BUFFERED_IO;
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	KdPrint(("Driver Initialized Success!\n"));

	return STATUS_SUCCESS;
}