#include<ntddk.h>

typedef struct _DEVICE_EXTENSION
{
	ULONG Length;
	PCHAR Buffer;
}DEVICE_EXTENSION,*PDEVICE_EXTENSION;

void Unload(IN PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Driver Unload!\n"));
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDDK");

	IoDeleteDevice(DriverObject->DeviceObject);
	IoDeleteSymbolicLink(&SymbolicLinkName);

}

NTSTATUS DispatchRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	static PCHAR typeName[] = {
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
		"IRP_MJ_PNP						" };
	
	KdPrint(("Irp Type:%s\n", typeName[stack->MajorFunction]));
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DirectRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	NTSTATUS status;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	PCHAR Buffer = NULL;
	ULONG ByteOffset;
	ULONG ByteCount;
	ULONG uLength;
	PVOID Va = NULL;

	__try
	{
		ByteOffset = MmGetMdlByteOffset(Irp->MdlAddress);
		ByteCount = MmGetMdlByteCount(Irp->MdlAddress);
		Va = MmGetMdlVirtualAddress(Irp->MdlAddress);
		KdPrint(("ByteOffset:%d\n", ByteOffset));
		KdPrint(("ByteCount:%d\n", ByteCount));
		KdPrint(("Va:%p\n", Va));

		Buffer = (PCHAR)MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
		if (stack->Parameters.Read.Length <= ByteCount)
		{
			RtlCopyMemory(Buffer, "This is a string from kernel\0", strlen("This is a string from kernel\0"));
			uLength = strlen("This is a string from kernel\0");
			status = STATUS_SUCCESS;
		}
		else
		{
			uLength = 0;
			status = STATUS_UNSUCCESSFUL;
		}


	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = GetExceptionCode();
		uLength = 0;
		KdPrint(("DataReadFailed! 0x%x\n", status));
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = uLength;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloDDK");
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDDK");
	PDEVICE_OBJECT DeviceObject = NULL;
	PDEVICE_EXTENSION DeviceExtension = NULL;
	ULONG i;
	KdPrint(("Driver Load!\n"));

	status = IoCreateDevice(DriverObject, sizeof(DEVICE_EXTENSION), &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Device Create Failed! 0x%x\n",status));
		return status;
	}
	status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("SymbolicLink Create Failed! 0x%x\n", status));
		return status;
	}

	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = DispatchRoutine;
	}
	DriverObject->MajorFunction[IRP_MJ_READ] = DirectRead;

	DeviceObject->Flags |= DO_DIRECT_IO;
	DeviceObject->Flags ^= DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}