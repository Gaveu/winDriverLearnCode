#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING SymboliclinkName = RTL_CONSTANT_STRING(L"\\??\\DriverA");
	IoDeleteDevice(DriverObject->DeviceObject);
	IoDeleteSymbolicLink(&SymboliclinkName);


	KdPrint(("Goodbye Driver!\n"));
}

NTSTATUS DispatchRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP PIrp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(PIrp);
	static PCHAR name[] =
	{
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
		"IRP_MJ_PNP						"
	};
	KdPrint(("IRP:%s Hit!\n",name[stack->MajorFunction]));

	PIrp->IoStatus.Status = STATUS_SUCCESS;
	PIrp->IoStatus.Information = 0;
	IoCompleteRequest(PIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}


NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;
	
	NTSTATUS status;
	PDEVICE_OBJECT DeviceObject;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\DriverA");
	UNICODE_STRING SymboliclinkName = RTL_CONSTANT_STRING(L"\\??\\DriverA");

	status = IoCreateDevice(
		DriverObject,
		0,
		&DeviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&DeviceObject
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Device Create Failed!%x\n", status));
		return status;
	}

	status = IoCreateSymbolicLink(
		&SymboliclinkName,
		&DeviceName
		);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("SymbolicLink Create Failed!%x\n", status));
		IoDeleteDevice(DeviceObject);
		IoDeleteSymbolicLink(&SymboliclinkName);
		return status;
	}


	int i;
	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = DispatchRoutine;
	}

	DriverObject->Flags |= DO_BUFFERED_IO;
	DriverObject->Flags &= ~DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}