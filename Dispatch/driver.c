#include<ntddk.h>

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDDK");
	IoDeleteSymbolicLink(&SymbolicLinkName);
	IoDeleteDevice(DriverObject->DeviceObject);

	KdPrint(("DriverUnload!\n"));
}

NTSTATUS DispatchRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	static PCHAR IrpName[] = {
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
		"IRP_MJ_PNP						 ",
		"IRP_MJ_PNP_POWER				 ",
		"IRP_MJ_MAXIMUM_FUNCTION		 "
	};
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	KdPrint(("%s\n", IrpName[stack->MajorFunction]));
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistyPath)
{
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloDDK");
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDDK");
	PDEVICE_OBJECT DeviceObject;
	NTSTATUS status;

	KdPrint(("DriverLoad!\n"));
	DriverObject->DriverUnload = Unload;

	ULONG i;
	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = DispatchRoutine;
	}

	/*
	NTSTATUS IoCreateDevice(
	PDRIVER_OBJECT  DriverObject,
	ULONG           DeviceExtensionSize,
	PUNICODE_STRING DeviceName,
	DEVICE_TYPE     DeviceType,
	ULONG           DeviceCharacteristics,
	BOOLEAN         Exclusive,
	PDEVICE_OBJECT  *DeviceObject
	);
	The IoCreateDevice routine creates a device object for use by a driver.
	*/
	status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("IoCreateDevice Failed! 0x%x\n",status));
		return status;	
	}

	status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("IoCreateSymbolicLink Failed! 0x%x\n", status));
		IoDeleteDevice(DeviceObject);
		return status;
	}
	DeviceObject->Flags |= DO_BUFFERED_IO;
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;	//初始化完毕的状态赋予Flags


	return STATUS_SUCCESS;
}