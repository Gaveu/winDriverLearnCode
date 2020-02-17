#include<ntddk.h>

#define CTLDRTINCODE CTL_CODE(FILE_DEVICE_UNKNOWN,0x1001,METHOD_IN_DIRECT,FILE_ANY_ACCESS)
#define CTLDRTOUTCODE CTL_CODE(FILE_DEVICE_UNKNOWN,0x1002,METHOD_OUT_DIRECT,FILE_ANY_ACCESS)

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING SymboliclinkName = RTL_CONSTANT_STRING(L"\\??\\CTLDirectIO");
	IoDeleteDevice(DriverObject->DeviceObject);
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

NTSTATUS DispatchIOCTL(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	NTSTATUS status;
	PCHAR InBuf;
	PCHAR OutBuf;
	int ibufLen;
	int obufLen;
	int retLen = 0;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	ULONG ctlCode = stack->Parameters.DeviceIoControl.IoControlCode;

	switch (ctlCode)
	{
	case CTLDRTINCODE:
	{
		InBuf = Irp->AssociatedIrp.SystemBuffer;
		ibufLen = stack->Parameters.DeviceIoControl.InputBufferLength;
		KdPrint(("InData:%s\n", InBuf));
		KdPrint(("InDataLen:%d\n", ibufLen));
		__try
		{
			retLen = strlen(InBuf);
			status = STATUS_SUCCESS;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			retLen = 0;
			status = GetExceptionCode();
			KdPrint(("Direct In Failed!%x\n",status));
		}
	}break;

	case CTLDRTOUTCODE:
	{
		PCHAR kData = "This is a string from Kernel!";
		OutBuf = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
		obufLen = stack->Parameters.DeviceIoControl.OutputBufferLength;
		retLen = strlen(kData) > obufLen ? obufLen : strlen(kData);
		__try
		{
			RtlCopyMemory(OutBuf, kData, retLen);
			OutBuf[retLen] = 0;
			status = STATUS_SUCCESS;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			retLen = 0;
			status = GetExceptionCode();
			KdPrint(("Data Write Failed!%x\n", status));
		}

	}break;

	default:
	{
		KdPrint(("Unknown CTLCODE!%d\n", ctlCode));
	}break;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = retLen;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	KdPrint(("Hello Driver!\n"));
	DriverObject->DriverUnload = Unload;
	int i;
	NTSTATUS status;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\CTLDirectIO");
	UNICODE_STRING SymboliclinkName = RTL_CONSTANT_STRING(L"\\??\\CTLDirectIO");
	PDEVICE_OBJECT DeviceObject = NULL;
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
		KdPrint(("DeviceObject Create Failed!%x\n", status));
		return status;
	}
	status = IoCreateSymbolicLink(&SymboliclinkName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("SymbolicLink Create Failed!%x\n", status));
		IoDeleteDevice(DeviceObject);
		return status;
	}

	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = DispatchRoutine;
	}
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIOCTL;

	DeviceObject->Flags |= 0;
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}