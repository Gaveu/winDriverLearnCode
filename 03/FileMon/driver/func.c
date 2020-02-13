#include"head.h"

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	PDEVICE_EXTENSION pDx = DriverObject->DeviceObject->DeviceExtension;
	UNICODE_STRING SymboliclinkName = RTL_CONSTANT_STRING(L"\\??\\FileMonTest");
	ExFreePool(pDx->mem);
	IoDeleteDevice(DriverObject->DeviceObject);
	IoDeleteSymbolicLink(&SymboliclinkName);
	KdPrint(("Goodbye Driver!\n"));
}

NTSTATUS DispatchRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP PIrp)
{
	NTSTATUS status;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(PIrp);
	static PCHAR irpName[] = {
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
	KdPrint(("%s\n", irpName[stack->MajorFunction]));
	PIrp->IoStatus.Information = 0;
	PIrp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(PIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;

}

NTSTATUS DispatchRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP PIrp)
{
	NTSTATUS status;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(PIrp);
	ULONG offset = stack->Parameters.Read.ByteOffset.LowPart;
	ULONG rLen = stack->Parameters.Read.Length;
	PDEVICE_EXTENSION pDx = DeviceObject->DeviceExtension;
	ULONG retLen = 0;
	__try
	{
		if (pDx->size <= offset)
		{	//读请求的偏移量超出已有范围
			status = STATUS_BUFFER_TOO_SMALL;
		}
		retLen = rLen < pDx->size - offset ? rLen : pDx->size - offset;
		RtlCopyMemory(PIrp->AssociatedIrp.SystemBuffer, pDx->mem + offset, retLen);
		status = STATUS_SUCCESS;	
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		KdPrint(("Read Exception!%d", GetExceptionCode()));
		status = STATUS_UNSUCCESSFUL;
	}

	PIrp->IoStatus.Information = retLen;
	PIrp->IoStatus.Status = status;
	IoCompleteRequest(PIrp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DispatchWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP PIrp)
{
	NTSTATUS status;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(PIrp);
	ULONG wLen = stack->Parameters.Write.Length;
	ULONG offset = stack->Parameters.Write.ByteOffset.LowPart;
	PDEVICE_EXTENSION pDx = DeviceObject->DeviceExtension;
	ULONG retLen = 0;

	__try
	{
		if (pDx->size <= offset)
		{	//写请求的偏移量超出已有范围
			status = STATUS_BUFFER_TOO_SMALL;
		}
		retLen = wLen < pDx->size - offset ? wLen : pDx->size - offset;
		RtlCopyMemory(pDx->mem + offset, PIrp->AssociatedIrp.SystemBuffer, retLen);
		status = STATUS_SUCCESS;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		KdPrint(("Write Exception!%d", GetExceptionCode()));
		status = STATUS_UNSUCCESSFUL;
	}

	PIrp->IoStatus.Information = retLen;
	PIrp->IoStatus.Status = status;
	IoCompleteRequest(PIrp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DispatchQuery(IN PDEVICE_OBJECT DeviceObject, IN PIRP PIrp)
{
	NTSTATUS status;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(PIrp);
	PDEVICE_EXTENSION pDx = DeviceObject->DeviceExtension;
	PFILE_STANDARD_INFORMATION pfic = NULL;
	ULONG retLen = 0;

	__try
	{
		pfic = (PFILE_STANDARD_INFORMATION)PIrp->AssociatedIrp.SystemBuffer;
		pfic->EndOfFile.LowPart = pDx->size;
		retLen = sizeof(FILE_STANDARD_INFORMATION);
		status = STATUS_SUCCESS;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = GetExceptionCode();
		KdPrint(("Query failed!%d\n",status));
	}

	PIrp->IoStatus.Information = retLen;
	PIrp->IoStatus.Status = status;
	IoCompleteRequest(PIrp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS InitDeviceExtension(IN PDEVICE_EXTENSION DeviceExtension)
{
	NTSTATUS status;
	__try
	{
		DeviceExtension->size = FILE_MAX_SIZE;
		DeviceExtension->type = PagedPool;
		DeviceExtension->mem = (PCHAR)ExAllocatePool(PagedPool, FILE_MAX_SIZE);
		status = STATUS_SUCCESS;
		__try
		{
			RtlZeroMemory(DeviceExtension->mem, DeviceExtension->size);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			KdPrint(("Device Extension's Mem Allocate Failed!%d\n", GetExceptionCode()));
			status = STATUS_INSUFFICIENT_RESOURCES;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		KdPrint(("Device Extension Init Failed!%d\n", GetExceptionCode()));
		status = STATUS_INVALID_PARAMETER;
	}
	return status;
}