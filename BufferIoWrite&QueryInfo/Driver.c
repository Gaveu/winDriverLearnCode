#include <ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Driver Unload!\n"));

}

NTSTATUS DispatchRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	static PCHAR typeIrp[] =
	{
		"IRP_MJ_CREATE					 ",
		"IRP_MJ_CREATE_NAMED_PIPE		 ",
		"IRP_MJ_CLOSE					 ",
		"IRP_MJ_READ					 ",
		"IRP_MJ_WRITE					 ",
		"IRP_MJ_QUERY_INFORMATION		 ",
		"IRP_MJ_SET_INFORMATION			 ",
		"IRP_MJ_QUERY_EA				 ",
		"IRP_MJ_SET_EA					 ",
		"IRP_MJ_FLUSH_BUFFERS			 ",
		"IRP_MJ_QUERY_VOLUME_INFORMATION ",
		"IRP_MJ_SET_VOLUME_INFORMATION	 ",
		"IRP_MJ_DIRECTORY_CONTROL		 ",
		"IRP_MJ_FILE_SYSTEM_CONTROL		 ",
		"IRP_MJ_DEVICE_CONTROL			 ",
		"IRP_MJ_INTERNAL_DEVICE_CONTROL	 ",
		"IRP_MJ_SHUTDOWN				 ",
		"IRP_MJ_LOCK_CONTROL			 ",
		"IRP_MJ_CLEANUP					 ",
		"IRP_MJ_CREATE_MAILSLOT			 ",
		"IRP_MJ_QUERY_SECURITY			 ",
		"IRP_MJ_SET_SECURITY			 ",
		"IRP_MJ_POWER					 ",
		"IRP_MJ_SYSTEM_CONTROL			 ",
		"IRP_MJ_DEVICE_CHANGE			 ",
		"IRP_MJ_QUERY_QUOTA				 ",
		"IRP_MJ_SET_QUOTA				 ",
		"IRP_MJ_PNP						 " };

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	KdPrint(("%s\n", typeIrp[stack->MajorFunction]));
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	ULONG Length;
	NTSTATUS status;

	__try
	{
		Length = stack->Parameters.Write.Length;
		if (Length > 1)
		{
			KdPrint(("%s\n", Irp->AssociatedIrp.SystemBuffer));
			status = STATUS_SUCCESS;
		}
		else
		{
			status = STATUS_INVALID_PARAMETER;
			Length = 0;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = GetExceptionCode();
		KdPrint(("Data write failed!\n 0x%x\n",status));
		Length = 0;
	}

	Irp->IoStatus.Information = Length;
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}

NTSTATUS DispatchQueryInfo(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status;
	ULONG Length;

	__try
	{
		Length = stack->Parameters.QueryFile.Length;
		if (stack->Parameters.QueryFile.FileInformationClass == FileStandardInformation &&
			Length >= sizeof(FileStandardInformation))
		{
			PFILE_STANDARD_INFORMATION pfsi = (PFILE_STANDARD_INFORMATION)Irp->AssociatedIrp.SystemBuffer;
			status = STATUS_SUCCESS;
			pfsi->EndOfFile.LowPart = 512 * 10;
			Length = sizeof(FileStandardInformation);
		}
		else
		{
			status = STATUS_BUFFER_TOO_SMALL;
			Length = 0;
		}

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = GetExceptionCode();
		Length = 0;
		KdPrint(("QueryInformation failed! 0x%x\n", status));
	}

	Irp->IoStatus.Information = Length;
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status; 
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Driver Load!\n"));

	DriverObject->DriverUnload = Unload;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloDDK");
	UNICODE_STRING SymbolicLinkPath = RTL_CONSTANT_STRING(L"\\??\\HelloDDK");

	ULONG i;
	NTSTATUS status;
	PDEVICE_OBJECT DeviceObject;

	status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Device create failed! 0x%x\n", status));
		return status;
	}
	status = IoCreateSymbolicLink(&SymbolicLinkPath, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(DeviceObject);
		KdPrint(("SymbolicLink create failed! 0x%x\n", status));
		return status;
	}

	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = DispatchRoutine;
	}
	DriverObject->MajorFunction[IRP_MJ_WRITE] = DispatchWrite;
	DriverObject->MajorFunction[IRP_MJ_QUERY_INFORMATION] = DispatchQueryInfo;

	DeviceObject->Flags |= DO_BUFFERED_IO;
	DeviceObject->Flags ^= DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}